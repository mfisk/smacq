#include <python2.2/Python.h>
#include <smacq.h>
#include <glib.h>

	/*
	 * Here's what we want to do:
	 * Export to Python a module with Consume() and Produce() methods.
	 * The Consume() function will:
	 * 	- save the thread state (closure) 
	 * 	- swap to a anti-greedy thread that will terminate the eval
	 * When the dataflow engine gives us more data, we restore the thread
	 * and return it to the user.
	 *
	 * The Produce() functon enqueue the datum on a queue that we check
         * everytime we return to the dataflow engine.
	 *
	 * The Dynamic Type System will be interfaced with an extension class
	 * whose __getattr__() method does flow_getfield() calls.
	 *
	 */

struct state {
	smacq_environment * env;
	PyObject * globals; 
	PyObject * pyObject;
	PyObject * pyArgs;
};

staticforward PyTypeObject DTSType;
typedef struct {
	PyObject_HEAD
	dts_object * ob_datum;
	char * ob_name;
	char ** ob_field_names;
	int ob_num_fields;
	struct state * state;
} DTSObject;

static smacq_result python_init(struct smacq_init * context) {
	char * cmd, * filename;
	PyObject * builtins, * tmpargs = NULL;
	struct state * state = context->state = g_new0(struct state, 1);

	state->env = context->env;

	filename = context->argv[1];
	assert(filename);

	//assert(context->argv[2]);
	//snprintf(cmd, 1024, "%s\n", context->argv[2]);
	//assert(cmd);

	{
		FILE * fh;
		struct stat stats;
		int res;

		fh = fopen(filename, "r");
		assert(fh);
		fstat(fileno(fh), &stats);
		cmd = g_malloc(stats.st_size+1);
		res = fread(cmd, stats.st_size, 1, fh);
		assert(res == 1);
		fclose(fh);

		cmd[stats.st_size] = '\0';
		//fprintf(stderr, "command is:\n%s\n", cmd);
	}

	Py_Initialize();
	state->globals = PyDict_New();

	builtins = PyEval_GetBuiltins();
	assert(builtins);

	tmpargs = Py_CompileString(cmd, filename, Py_file_input);
	assert(tmpargs);

	// Tupelize this object
	state->pyArgs = Py_BuildValue("(O,O)", tmpargs, state->globals);
	assert(state->pyArgs);

	// Save pointer to eval
	state->pyObject = PyDict_GetItemString(builtins, "eval");
	assert(state->pyObject);

	return SMACQ_PASS;
}

static smacq_result python_consume(struct state * state,
                                const dts_object * datum, int * outchan) {
	PyObject * result, * temp;

//	printf("Datum is at %d\n", datum);

	temp = (PyObject *)PyObject_New(DTSObject, &DTSType);
	((DTSObject *)(temp))->ob_datum = (dts_object *)datum;
	((DTSObject *)(temp))->state = state;
	((DTSObject *)(temp))->ob_name = strdup("data");
	((DTSObject *)(temp))->ob_num_fields = 0;

	PyDict_SetItem(state->globals, PyString_FromString("datum"), temp);

	state->pyArgs = Py_BuildValue("(O,O)",
		PyTuple_GetItem(state->pyArgs, 0), state->globals);

	result = PyEval_CallObject(state->pyObject, state->pyArgs);

	if (!result) {
		PyErr_Print(); // Print traceback
		return SMACQ_END|SMACQ_ERROR;
	}

	return SMACQ_PASS;
}

static smacq_result python_produce(struct state * state, const dts_object ** datum,
                                int * outchan) {
	return SMACQ_END;
}

static smacq_result python_shutdown(struct state * state) {
	Py_Finalize();
	return SMACQ_END;
}

struct smacq_functions smacq_python_table = {
	produce: &python_produce, 
	consume: &python_consume,
	init: &python_init,
	shutdown: &python_shutdown
};



/* Python object glue */

static PyObject * DTS_getattr(DTSObject * self, char * name) {
	PyObject * pyAttr;
	dts_object * field;

//	printf("# Getting %s attr for a DTSObject!\n", name);

	field = malloc(sizeof(dts_object));

	pyAttr = (PyObject *)PyObject_New(DTSObject, &DTSType);
	((DTSObject *)(pyAttr))->state = self->state;
	((DTSObject *)(pyAttr))->ob_name = strdup(name);

	if (flow_getfield(self->state->env, self->ob_datum,
		flow_requirefield(self->state->env, name), field) == 0) {
//		printf("# %s field not found\n", name);
		((DTSObject *)(pyAttr))->ob_datum = NULL;
	} else {
		((DTSObject *)(pyAttr))->ob_datum = field;
	}

	return pyAttr;
}

static int DTS_setattr(DTSObject * self, char * name, PyObject * value) {
	PyErr_SetString(PyExc_AttributeError, "read-only attributes");
	return -1;
}

static PyObject * DTS_repr(DTSObject * self) {
	char * str;
	int len;

//	printf("# Building repr for a DTSObject!\n");

//	strcpy(type,
//		dts_typename_bynum(self->state->env, self->ob_datum->type));

//	printf("# %d, %d\n", NULL, self->ob_datum);
	if (self->ob_datum == NULL) {
		str = strdup("datum doesn't exist");
	} else {
		flow_presentdata(self->state->env, self->ob_datum, 
			flow_transform(self->state->env, "string"),
			(void *)&str, &len);
	}

//	printf("# %s\n", str);

	return PyString_FromString(str);
}

static PyObject * DTS_str(DTSObject * self) {
	return DTS_repr(self);
}
	
static int DTS_print (DTSObject * self, FILE * fp, int flags) {
	PyObject_Print(DTS_repr(self), fp, 0);

	return 0;
}

static void DTS_dealloc(DTSObject * self) {
	free(self->ob_datum);
	PyObject_Del(self);
}

/* TypeObject def for Python2.2 */
statichere PyTypeObject DTSType = {
	PyObject_HEAD_INIT(NULL)
	0,					/* ob_size		*/
	"DTS",					/* tp_name		*/
	sizeof(DTSObject),			/* tp_basicsize		*/
	0,					/* tp_itemsize		*/
	(destructor) DTS_dealloc,		/* tp_dealloc		*/
	(printfunc) DTS_print,			/* tp_print		*/
	(getattrfunc) DTS_getattr,		/* tp_getattr		*/
	(setattrfunc) DTS_setattr,		/* tp_setattr		*/
	0,					/* tp_compare		*/
	(reprfunc) DTS_repr,			/* tp_repr		*/
	0,					/* tp_as_number		*/
	0,					/* tp_as_sequence	*/
	0,					/* tp_as_mapping	*/
	0,					/* tp_hash		*/
	0,					/* tp_call		*/
	(reprfunc) DTS_str			/* tp_str		*/
};
