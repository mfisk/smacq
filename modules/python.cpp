//#define DUMP_ENABLE
#include <Python.h>
#include <stdint.h>
#include <limits.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "DtsObject.h"
#include "smacq.h"
#include "dump.h"
#include "dts.h"

static int init_count = 0;

SMACQ_MODULE(python,
	     PROTO_CTOR(python);
	     PROTO_DTOR(python);
	     PROTO_CONSUME();

             public:
             void enqueue_mofo(DtsObject &dts, int &outchan);

	     private:
	     PyObject *pConsume;
);

static PyObject *pydts_create(DtsObject datum, DTS *dts);


/*
 *
 *
 *
 * DTS object
 *
 *
 *
 */

static char PyDts_doc[] =
("A DTS object.\n"
 "\n"
 "This object contains data, perhaps hierarchically so.\n"
 "It can be accessed like a dictionary, and supports methods\n"
 "to return the data it represents.\n");

typedef struct {
  PyObject_HEAD
  DtsObject d;
  DTS       *dts;
} PyDtsObject;


static void PyDts_dealloc(PyObject *p)
{
  PyDtsObject *self = (PyDtsObject *)p;

  /*
   * d was constructed with placement new, so we have to call dtor
   * manually.  Do NOT call delete.
   */
  self->d.~DtsObject();

  PyObject_Del(p);
}

static int PyDts_length(PyObject *p)
{
  return 0;
}

static PyMethodDef PyDts_methods[] = {
  {NULL, NULL},
};


/** Retrieve a DTS field by name
 *
 * Pretty standard stuff here.
 */
static PyObject *PyDts_subscript(PyObject *p, PyObject *pName)
{
  PyDtsObject *self   = (PyDtsObject *)p;
  char        *name;
  PyObject    *pDts   = NULL;
  DtsObject    d;
  int          passed = 0;

  do {
    name = PyString_AsString(pName);
    if (! name) {
      break;
    }

    d = self->d->getfield(name);
    if (! d) {
      PyErr_Format(PyExc_KeyError, "No such field: %s", name);
      break;
    }

    pDts = pydts_create(d, self->dts);
    if (! pDts) {
      break;
    }

    passed = 1;
  } while (0);

  if (! passed) {
    Py_XDECREF(pDts);
    return NULL;
  }

  return (PyObject *)pDts;
}

static PyMappingMethods PyDts_as_mapping = {
  PyDts_length,                 /* mp_length            */
  PyDts_subscript,              /* mp_subscript         */
  NULL,                         /* mp_ass_subscript     */
};

enum datatype {
  STRING,
  UINT32,
};

/** Return type name
 */
static PyObject *PyDts_gettype(PyObject *p, void *closure)
{
  PyDtsObject *self = (PyDtsObject *)p;
  char        *dtsname;
  dts_typeid   dtstype;

  dtstype = self->d->gettype();
  dtsname = self->dts->typename_bynum(dtstype);
  return Py_BuildValue("s", dtsname);
}

/** Return raw data
 */
static PyObject *PyDts_getraw(PyObject *p, void *closure)
{
  PyDtsObject *self = (PyDtsObject *)p;

  return Py_BuildValue("s#",
                       self->d->getdata(),
                       self->d->getsize());
}

/** Return data as an appropriate Python type
 */
static PyObject *PyDts_getvalue(PyObject *p, void *closure)
{
  PyDtsObject *self = (PyDtsObject *)p;
  dts_typeid   dtstype;
  char        *dtsname;
  void        *data;
  PyObject    *pRet;

  dtstype = self->d->gettype();
  data    = self->d->getdata();
  dtsname = self->dts->typename_bynum(dtstype);

  if (strcmp(dtsname, "ubyte") == 0) {
    pRet = PyInt_FromLong((long)*((unsigned char *)data));
  } else if (strcmp(dtsname, "ushort") == 0) {
    pRet = PyInt_FromLong((long)*((unsigned short *)data));
  } else if (strcmp(dtsname, "nushort") == 0) {
    pRet = PyInt_FromLong((long)ntohs(*((unsigned short *)data)));
  } else if (strcmp(dtsname, "uint32") == 0) {
    pRet = PyLong_FromLongLong((long long)*((unsigned long *)data));
  } else if (strcmp(dtsname, "nuint32") == 0) {
    pRet = PyLong_FromLongLong((long long)ntohl(*((unsigned long *)data)));
  } else if (strcmp(dtsname, "ip") == 0) {
    /* We presume that "ip" means "ipv4" */
    char str[INET_ADDRSTRLEN];

    if (NULL == inet_ntop(AF_INET, data, str, sizeof(str))) {
      pRet = PyString_FromStringAndSize((const char *)data,
                                        self->d->getsize());
    } else {
      pRet = PyString_FromString((const char *)str);
    }
  } else {
    pRet = PyString_FromStringAndSize((const char *)data,
                                      self->d->getsize());
  }

  return pRet;
}

static PyGetSetDef PyDts_getset[] = {
  {"type",  PyDts_gettype,  NULL, "named type of data"},
  {"raw",   PyDts_getraw,   NULL, "raw data as a string"},
  {"value", PyDts_getvalue, NULL, "data as an appropriate Python type"},
  {NULL}                        /* Sentinel */
};

static PyTypeObject PyDtsType = {
  PyObject_HEAD_INIT(NULL)
  0,                            /* ob_size		*/
  "PyDts",                      /* tp_name		*/
  sizeof(PyDtsObject),          /* tp_basicsize		*/
  0,                            /* tp_itemsize		*/
  PyDts_dealloc,                /* tp_dealloc		*/
  0,                            /* tp_print		*/
  0,                            /* tp_getattr		*/
  0,                            /* tp_setattr		*/
  0,                            /* tp_compare		*/
  0,                            /* tp_repr		*/
  0,                            /* tp_as_number		*/
  0,                            /* tp_as_sequence	*/
  &PyDts_as_mapping,            /* tp_as_mapping	*/
  0,                            /* tp_hash		*/
  0,                            /* tp_call		*/
  0,                            /* tp_str		*/
  PyObject_GenericGetAttr,      /* tp_getattro          */
  0,                            /* tp_setattro          */
  0,                            /* tp_as_buffer         */
  Py_TPFLAGS_DEFAULT,           /* tp_flags             */
  PyDts_doc,                    /* tp_doc               */
  0,                            /* tp_traverse */
  0,                            /* tp_clear */
  0,                            /* tp_richcompare */
  0,                            /* tp_weaklistoffset */
  0,                            /* tp_iter */
  0,                            /* tp_iternext */
  PyDts_methods,                /* tp_methods */
  0,                            /* tp_members */
  PyDts_getset,                 /* tp_getset */
  0,                            /* tp_base */
  0,                            /* tp_dict */
  0,                            /* tp_descr_get */
  0,                            /* tp_descr_set */
};

/** Create a new DtsObject from C
 */
static PyObject *pydts_create(DtsObject datum, DTS *dts)
{
  PyDtsObject *pObj   = NULL;
  int          passed = 0;

  do {
    /* Allocate the new object */
    pObj = PyObject_New(PyDtsObject, &PyDtsType);
    if (! pObj) {
      break;
    }

    /* Boost does some fancy footwork to make things feel nice for the
     * programmer.  That would let us just say "pObj->d = datum".
     * Unfortunately, since Python uses plain C allocators instead of
     * "new", we have to use placement new to manually initialize d.
     */
    new(&pObj->d) DtsObject(datum);

    pObj->dts = dts;

    passed = 1;
  } while (0);

  if (! passed) {
    Py_XDECREF(pObj);
    return NULL;
  }

  return (PyObject *)pObj;
}


/*
 *
 *
 *
 * SMACQ object
 *
 *
 *
 */
static char PySmacq_doc[] =
("A SMACQ object.\n"
 "\n"
 "This object represents the internal module state.  It is the main\n"
 "entry point for Python modules into the SMACQ system.\n");

/*
 * In reality, this just stores a pointer to the "current" pythonModule
 * object.  This allows Python's callback-based API to still access
 * self, for things like enqueue().
 */

typedef struct {
  PyObject_HEAD
  pythonModule *module;
} PySmacqObject;

static void PySmacq_dealloc(PyObject *p)
{
  /* Nothing to do here */
}

static PyObject *PySmacq_enqueue(PyObject *p, PyObject *args)
{
  PySmacqObject *self    = (PySmacqObject *)p;
  PyDtsObject   *pDts;
  int            outchan = 0;

  if (! PyArg_ParseTuple(args, "O!|i", &PyDtsType, &pDts, &outchan)) {
    return NULL;
  }

  self->module->enqueue_mofo(pDts->d, outchan);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PySmacq_methods[] = {
  {"enqueue", PySmacq_enqueue, METH_VARARGS},
  {NULL, NULL}                  // sentry
};

static PyTypeObject PySmacqType = {
  PyObject_HEAD_INIT(NULL)
  0,                            /* ob_size		*/
  "PySmacq",                    /* tp_name		*/
  sizeof(PySmacqObject),        /* tp_basicsize		*/
  0,                            /* tp_itemsize		*/
  PySmacq_dealloc,              /* tp_dealloc		*/
  0,                            /* tp_print		*/
  0,                            /* tp_getattr		*/
  0,                            /* tp_setattr		*/
  0,                            /* tp_compare		*/
  0,                            /* tp_repr		*/
  0,                            /* tp_as_number		*/
  0,                            /* tp_as_sequence	*/
  0,                            /* tp_as_mapping	*/
  0,                            /* tp_hash		*/
  0,                            /* tp_call		*/
  0,                            /* tp_str		*/
  PyObject_GenericGetAttr,      /* tp_getattro          */
  0,                            /* tp_setattro          */
  0,                            /* tp_as_buffer         */
  Py_TPFLAGS_DEFAULT,           /* tp_flags             */
  PySmacq_doc,                  /* tp_doc               */
  0,                            /* tp_traverse */
  0,                            /* tp_clear */
  0,                            /* tp_richcompare */
  0,                            /* tp_weaklistoffset */
  0,                            /* tp_iter */
  0,                            /* tp_iternext */
  PySmacq_methods,              /* tp_methods */
  0,                            /* tp_members */
  0,                            /* tp_getset */
  0,                            /* tp_base */
  0,                            /* tp_dict */
  0,                            /* tp_descr_get */
  0,                            /* tp_descr_set */
};

/** Create a new SmacqObject from C
 */
static PyObject *pysmacq_create(pythonModule *module)
{
  PySmacqObject *pObj   = NULL;
  int            passed = 0;

  do {
    /* Allocate the new object */
    pObj = PyObject_New(PySmacqObject, &PySmacqType);
    if (! pObj) {
      break;
    }

    pObj->module = module;

    passed = 1;
  } while (0);

  if (! passed) {
    Py_XDECREF(pObj);
    return NULL;
  }

  return (PyObject *)pObj;
}


/*
 *
 *
 * SMACQ module code
 *
 *
 */

/** Initialize the python module
 *
 * Load up a module named by argv
 */
pythonModule::pythonModule(struct smacq_init *context)
  : SmacqModule(context)
{
  PyObject *pSmacq  = NULL;
  PyObject *pModule = NULL;
  PyObject *pInit   = NULL;
  PyObject *pArgs   = NULL;
  PyObject *pObj    = NULL;
  int       passed  = 0;

  //DUMP();
  if (context->argc < 2) {
    fprintf(stderr, "No module name provided");
    /* XXX: there's got to be a better way to raise an error... */
    assert(0);
  }

  /* First check to see if Python is already running.  If so, don't
   * bother initializing, and make sure we never try to uninitialize. */
  if (-1 == init_count) {
    /* Don't do anything, someone else did the initialization. */
  } else if (0 == init_count) {
    if (Py_IsInitialized()) {
      /* We haven't initialized yet but Python's already running, so
       * hands off. */
      init_count = -1;
    } else {
      Py_Initialize();
      init_count = 1;
    }
  } else {
    init_count += 1;
  }

  /* XXX: if the module name is empty, compile argv[1] and save it as
   * the consume function.  But how do you pass in arguments?  Maybe a
   * lambda?  Mike thinks the object should just be called "o" in the
   * namespace.  I'm not sure I like that, it doesn't seem very
   * pythony. */

  this->pConsume = NULL;

  do {
    int i;

    /* Load up the module */
    pModule = PyImport_ImportModule(context->argv[1]);
    if (! pModule) {
      PyErr_Print();
      break;
    }

    /* Find the init function */
    pInit = PyObject_GetAttrString(pModule, "init");
    if (! pInit) {
      PyErr_Print();
      break;
    }

    /* Create the args tuple for the init function */
    pArgs = PyTuple_New(context->argc);
    if (! pArgs) {
      PyErr_Print();
      break;
    }

    /* Create the smacq object */
    pSmacq = pysmacq_create(this);
    if (! pSmacq) {
      PyErr_Print();
      break;
    }

    /* Stick it in pArgs */
    i = PyTuple_SetItem(pArgs, 0, pSmacq);
    if (i) {
      PyErr_Print();
      break;
    }

    /* Populate pArgs with the strings */
    for (i = 1; i < context->argc; i += 1) {
      PyObject *pObj;
      int       ret;

      pObj = PyString_FromString(context->argv[i]);
      if (! pObj) {
        PyErr_Print();
        break;
      }

      /* Leave the first one for the smacq object */
      ret = PyTuple_SetItem(pArgs, i, pObj);
      if (ret) {
        PyErr_Print();
        Py_XDECREF(pObj);
        break;
      }
    }
    if (i < context->argc) {
      PyErr_Print();
      break;
    }

    /* Call the init function */
    pObj = PyObject_CallObject(pInit, pArgs);
    if (! pObj) {
      PyErr_Print();
      break;
    }

    /* Make sure the return value conforms */
    this->pConsume = PyObject_GetAttrString(pObj, "consume");
    if (! this->pConsume) {
      PyErr_Print();
      break;
    }

    if (! PyCallable_Check(this->pConsume)) {
      fprintf(stderr, "consume is not callable\n");
      break;
    }

    passed = 1;
  } while(0);


  /* Clean up */
  Py_XDECREF(pObj);
  Py_XDECREF(pSmacq);
  Py_XDECREF(pModule);
  Py_XDECREF(pArgs);
  Py_XDECREF(pInit);

  if (! passed) {
    Py_XDECREF(this->pConsume);
    assert(0);
  }
}

/*
 * It's crap like this that drives people to Python.
 */
void pythonModule::enqueue_mofo(DtsObject &dts, int &outchan)
{
  enqueue(dts, outchan);
}

smacq_result pythonModule::consume(DtsObject datum, int &outchan)
{
  PyObject     *pDts   = NULL;
  PyObject     *pRet   = NULL;
  int           passed = 0;
  smacq_result  ret    = SMACQ_ERROR;

  do {
    /* Create a new DTS object */
    pDts = pydts_create(datum, this->dts);
    if (! pDts) {
      PyErr_Print();
      break;
    }

    /* Send it to the consume function */
    pRet = PyObject_CallFunction(this->pConsume, "O", pDts);
    if (! pRet) {
      PyErr_Print();
      break;
    }

    if (Py_None == pRet) {
      /* If they returned None, the buck stops here. */
      ret = SMACQ_FREE | canproduce();
    } else {
      /* Otherwise, it's gotta be a PyDtsObject */
      if (! PyObject_IsInstance(pRet, (PyObject *)&PyDtsType)) {
        fprintf(stderr, "Invalid return value: must be data object\n");
        break;
      }

      /* If they returned datum, we just pass it through.  Otherwise, we
       * need to enqueue it.
       */
      if (((PyDtsObject *)pRet)->d == datum) {
      DUMP();
        ret = SMACQ_PASS | canproduce();
      } else {
      DUMP();
        ret = SMACQ_FREE | canproduce();
        enqueue(((PyDtsObject *)pRet)->d, 0);
      }
    }

    passed = 1;
  } while (0);

  /* Clean up */

  Py_XDECREF(pRet);
  Py_XDECREF(pDts);

  if (! passed) {
    return SMACQ_ERROR;
  }

  return ret;
}

pythonModule::~pythonModule()
{
  if (-1 == init_count) {
    /* Don't touch it */
  } else {
    init_count -= 1;

    if (0 == init_count) {
      Py_Finalize();
    }
  }
}

