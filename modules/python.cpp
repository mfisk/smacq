#include <Python.h>
#define DUMP_ENABLE
#include "dump.h"
#undef DUMP_ENABLE
#include <netinet/in.h>
#include "smacq.h"
#include "dts.h"

static int init_count = 0;

SMACQ_MODULE(python,
	     PROTO_CTOR(python);
	     PROTO_DTOR(python);
	     PROTO_CONSUME();

	     private:
	     PyObject *pConsume;
);

static PyObject *pydts_create(DtsObject datum, DTS *dts);

/** Initialize the python module
 *
 * Load up a module named by argv
 */
pythonModule::pythonModule(struct smacq_init *context)
  : SmacqModule(context)
{
  PyObject *pModule = NULL;
  PyObject *pInit   = NULL;
  PyObject *pArgs   = NULL;
  int       passed  = 0;

  DUMP();
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
    pArgs = PyTuple_New(context->argc - 1);
    if (! pArgs) {
      PyErr_Print();
      break;
    }

    /* Populate it */
    for (i = 1; i < context->argc; i += 1) {
      PyObject *pObj;
      int       ret;

      pObj = PyString_FromString(context->argv[i]);
      if (! pObj) {
        break;
      }

      ret = PyTuple_SetItem(pArgs, i-1, pObj);
      if (ret) {
        Py_XDECREF(pObj);
        break;
      }
    }
    if (i < context->argc) {
      PyErr_Print();
      break;
    }

    /* Call the init function */
    this->pConsume = PyObject_CallObject(pInit, pArgs);
    if (! this->pConsume) {
      PyErr_Print();
      break;
    }

    if (! PyCallable_Check(this->pConsume)) {
      fprintf(stderr, "Return from init() is not callable\n");
      break;
    }

    passed = 1;
  } while(0);


  /* Clean up */
  Py_XDECREF(pModule);
  Py_XDECREF(pArgs);
  Py_XDECREF(pInit);

  if (! passed) {
    Py_XDECREF(this->pConsume);
    assert(0);
  }
}

smacq_result pythonModule::consume(DtsObject datum, int &outchan)
{
  PyObject *pDts   = NULL;
  PyObject *pRet   = NULL;
  int       passed = 0;

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

    /* For now we ignore pRet */

    passed = 1;
  } while (0);

  /* Clean up */

  Py_XDECREF(pRet);
  Py_XDECREF(pDts);

  if (! passed) {
    return SMACQ_ERROR;
  }

  return SMACQ_PASS;
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
 "I'm not sure what this does just yet.  More later.\n");

typedef struct {
  PyObject_HEAD
  /* Normally we'd keep d on the stack, not as a pointer.  But we can't
   * put it on the stack, since Python wants to use its internal memory
   * allocator.  Therefore, we keep a pointer and use new and delete. */
  DtsObject *d;
  DTS       *dts;
} PyDtsObject;


static void PyDts_dealloc(PyObject *p)
{
  PyDtsObject *self = (PyDtsObject *)p;

  delete self->d;
  PyObject_Del(p);
}

static int PyDts_length(PyObject *p)
{
  return 0;
}

static PyObject * PyDts_set(PyObject *p,
                            PyObject *args)
{
  PyDtsObject *self   = (PyDtsObject *)p;
  PyObject    *pVal;
  int          size;
  int          passed = 0;

  if (! PyArg_ParseTuple(args, "O", &pVal)) {
    return NULL;
  }

  DUMP();
  do {
    size = self->d->get()->getsize();

    if (PyInt_Check(pVal)) {
      long l;

      l = PyInt_AsLong(pVal);
      switch (self->d->get()->getsize()) {
        case sizeof(char):
          if 
          DUMP();
          break;
        case sizeof(short):
          DUMP();
          break;
        case sizeof(int):
          DUMP();
          break;
#ifdef HAVE_LONG_LONG
#ifdef NEALE_FIGURED_OUT_HOW_TO_CHECK_IN_CPP_WHETHER_INT_IS_THE_SAME_AS_LONG_INT
        case sizeof(long):
          DUMP();
          break;
#endif
        case sizeof(long long):
          DUMP();
          break;
#endif
    default:
      PyErr_Format(PyExc_ValueError,
                   "No integer type for data of length %d",
                   self->d->get()->getsize());
      break;
      }
    } else {
      DUMP_s("It's not a trap!");
    }

    passed = 1;
  } while (0);

  if (! passed) {
    return NULL;
  }

  Py_XINCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyDts_methods[] = {
  {"set", PyDts_set, METH_VARARGS, NULL},
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

    /* We can't use the overloaded -> operator since self->d is a pointer. */
    d = self->d->get()->getfield(self->dts->requirefield(name));
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

/** Return DtsObject data as a string
 */
static PyObject *PyDts_getdata(PyObject *p, void *closure)
{
  PyDtsObject *self = (PyDtsObject *)p;

  return Py_BuildValue("s#",
                       self->d->get()->getdata(),
                       self->d->get()->getsize());
}

/** Set DtsObject data from a string
 */
static int PyDts_setdata(PyObject *p, PyObject *pVal, void *closure)
{
  PyDtsObject *self = (PyDtsObject *)p;
  char        *buf;
  int          buflen;

  if (PyString_AsStringAndSize(pVal, &buf, &buflen)) {
    return -1;
  }

  if (self->d->get()->getsize() != buflen) {
    PyErr_Format(PyExc_ValueError,
                 "This field's length is %d, cannot set to string of length %d.",
                 buflen,
                 self->d->get()->getsize());
    return -1;
  }

  self->d->get()->setcopy(buf);
  return 0;
}

/** Return DtsObject data as a signed int
 *
 * This does some special butt-magic by checking the length of the data
 * against known integer types, and doing the right typecast.  If the
 * length is something weird (like 6), you get an AttributeError.
 */
static PyObject *PyDts_getint(PyObject *p, void *closure)
{
  PyDtsObject   *self = (PyDtsObject *)p;
  PyObject      *pRet = NULL;
  unsigned char *val;

  val = self->d->get()->getdata();
  switch (self->d->get()->getsize()) {
    case sizeof(char):
      pRet = PyInt_FromLong((long)*(char *)val);
      break;
    case sizeof(short):
      pRet = PyInt_FromLong((long)*(short *)val);
      break;
    case sizeof(int):
      pRet = PyInt_FromLong((long)*(int *)val);
      break;
#ifdef HAVE_LONG_LONG
#ifdef NEALE_FIGURED_OUT_HOW_TO_CHECK_IN_CPP_WHETHER_INT_IS_THE_SAME_AS_LONG_INT
    case sizeof(long):
      pRet = PyLong_FromLong(*(long *)val);
      break;
#endif
    case sizeof(long long):
      pRet = PyLong_FromLongLong(*(long long *)val);
      break;
#endif
    default:
      PyErr_Format(PyExc_ValueError,
                   "No integer type for data of length %d",
                   self->d->get()->getsize());
      break;
  }
  return pRet;
}

/** Set DtsObject data as a signed int
 */
static int PyDts_setint(PyObject *p, PyObject *pVal, void *closure)
{
  PyDtsObject *self = (PyDtsObject *)p;
  long long    l;

  l = PyLong_AsLongLong(l);
  switch (self->d->get()->getsize()) {
    case sizeof(char):
      l = PyInt_AsLongFromLong((long)*(char *)val);
      break;
    case sizeof(short):
      pRet = PyInt_FromLong((long)*(short *)val);
      break;
    case sizeof(int):
      pRet = PyInt_FromLong((long)*(int *)val);
      break;
#ifdef HAVE_LONG_LONG
#ifdef NEALE_FIGURED_OUT_HOW_TO_CHECK_IN_CPP_WHETHER_INT_IS_THE_SAME_AS_LONG_INT
    case sizeof(long):
      pRet = PyLong_FromLong(*(long *)val);
      break;
#endif
    case sizeof(long long):
      pRet = PyLong_FromLongLong(*(long long *)val);
      break;
#endif
    default:
      PyErr_Format(PyExc_ValueError,
                   "No integer type for data of length %d",
                   self->d->get()->getsize());
      break;
  }

  return 0;
}

/** Return DtsObject data as an unsigned int
 *
 * Same thing as PyDts_getint, but with unsigned ints this time.
 */
static PyObject *PyDts_getuint(PyObject *p, void *closure)
{
  PyDtsObject   *self = (PyDtsObject *)p;
  PyObject      *pRet = NULL;
  unsigned char *val;

  val = self->d->get()->getdata();
  switch (self->d->get()->getsize()) {
    case sizeof(unsigned char):
      pRet = PyInt_FromLong((long)*(unsigned char *)val);
      break;
    case sizeof(unsigned short):
      pRet = PyInt_FromLong((long)*(unsigned short *)val);
      break;
    case sizeof(unsigned int):
      pRet = PyLong_FromUnsignedLong((unsigned long)*(unsigned int *)val);
      break;
#ifdef HAVE_LONG_LONG
#ifdef NEALE_FIGURED_OUT_HOW_TO_CHECK_IN_CPP_WHETHER_INT_IS_THE_SAME_AS_LONG_INT
    case sizeof(unsigned long):
      pRet = PyLong_FromUnsignedLong(*(unsigned long *)val);
      break;
#endif
    case sizeof(unsigned long long):
      pRet = PyLong_FromUnsignedLongLong(*(unsigned long long *)val);
      break;
#endif
    default:
      PyErr_Format(PyExc_ValueError,
                   "No unsigned integer type for data of length %d",
                   self->d->get()->getsize());
      break;
  }
  return pRet;
}

static PyGetSetDef PyDts_getset[] = {
  {"data", PyDts_getdata, NULL, "data as a string"},
  {"int",  PyDts_getint,  NULL, "data as a signed int"},
  {"uint", PyDts_getuint, NULL, "data as an unsigned int"},
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
     * programmer.  Unfortunately, we can't use it, since our object
     * isn't on the stack.  So for us, it has to look ugly.  Don't blame
     * boost; blame C++. */
    pObj->d = new DtsObject;
    *(pObj->d) = datum;

    pObj->dts = dts;

    passed = 1;
  } while (0);

  if (! passed) {
    Py_XDECREF(pObj);
    return NULL;
  }

  return (PyObject *)pObj;
}
