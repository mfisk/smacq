#include <python2.2/Python.h>
#include <smacq.h>
#include <glib.h>

/// Export DtsObject to Python.

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
	 * whose __getattr__() method does smacq_getfield() calls.
	 *
	 */

SMACQ_MODULE(python,
	     PROTO_CTOR(python);
	     PROTO_DTOR(python);
	     PROTO_CONSUME();
	     private:
	     PyObject * globals; 
	     PyObject * pyObject;
	     PyObject * pyArgs;
);

staticforward PyTypeObject PyDTSType;
typedef struct {
	PyObject_HEAD
	DtsObject ob_datum;
	char * ob_name;
	char ** ob_field_names;
	int ob_num_fields;
} PyDtsObject;

pythonModule::pythonModule(struct smacq_init * context) {
	char * cmd, * filename;
	PyObject * builtins, * tmpargs = NULL;

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
	globals = PyDict_New();

	builtins = PyEval_GetBuiltins();
	assert(builtins);

	tmpargs = Py_CompileString(cmd, filename, Py_file_input);
	assert(tmpargs);

	// Tupelize this object
	pyArgs = Py_BuildValue("(O,O)", tmpargs, globals);
	assert(pyArgs);

	// Save pointer to eval
	pyObject = PyDict_GetItemString(builtins, "eval");
	assert(pyObject);

	return SMACQ_PASS;
}

smacq_result pythonModule::consume(DtsObject &datum, int * outchan) {
	PyDtsObject * result, * temp;

//	printf("Datum is at %d\n", datum);

	temp = PyObject_New(PyDtsObject, &PyDTSType);
	temp->ob_datum = datum;
	temp->ob_name = strdup("data");
	temp->ob_num_fields = 0;

	PyDict_SetItem(globals, PyString_FromString("datum"), temp);

	pyArgs = Py_BuildValue("(O,O)",
		PyTuple_GetItem(pyArgs, 0), globals);

	result = PyEval_CallObject(pyObject, pyArgs);

	if (!result) {
		PyErr_Print(); // Print traceback
		return SMACQ_END|SMACQ_ERROR;
	}

	return SMACQ_PASS;
}

pythonModule::~pythonModule(struct state * state) {
	Py_Finalize();
	return SMACQ_END;
}




/* Python object glue */

static PyObject * DTS_getattr(PyDtsObject & self, char * name) {
	PyDtsObject * pyAttr;

//	printf("# Getting %s attr for a PyDtsObject!\n", name);

	field = malloc(sizeof(DtsObject));
	dts_module_init(field);

	pyAttr = PyObject_New(PyDtsObject, &PyDTSType);
	pyAttr->ob_name = strdup(name);
	pyAttr->ob_datum = self->ob_datum->getfield(dts->requirefield(name));

	return pyAttr;
}

static int DTS_setattr(PyDtsObject * self, char * name, PyObject * value) {
	PyErr_SetString(PyExc_AttributeError, "read-only attributes");
	return -1;
}

static PyObject * DTS_repr(PyDtsObject * self) {
	char * str;
	int len;

//	printf("# Building repr for a PyDtsObject!\n");

//	strcpy(type,
//		dts_typename_bynum(self->env, self->ob_datum->type));

//	printf("# %d, %d\n", NULL, self->ob_datum);
	if (self->ob_datum) {
		str = strdup("datum doesn't exist");
	} else {
		smacq_presentdata(self->env, self->ob_datum, 
			smacq_transform(self->env, "string"),
			(void *)&str, &len);
	}

//	printf("# %s\n", str);

	return PyString_FromString(str);
}

static PyObject * DTS_str(PyDtsObject * self) {
	return DTS_repr(self);
}
	
static int DTS_print (PyDtsObject * self, FILE * fp, int flags) {
	PyObject_Print(DTS_repr(self), fp, 0);

	return 0;
}

static void DTS_dealloc(PyDtsObject * self) {
        self->ob_datum.reset();
	PyObject_Del(self);
}

/* TypeObject def for Python2.2 */
statichere PyTypeObject PyDTSType = {
	PyObject_HEAD_INIT(NULL)
	0,					/* ob_size		*/
	"DTS",					/* tp_name		*/
	sizeof(PyDtsObject),			/* tp_basicsize		*/
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
