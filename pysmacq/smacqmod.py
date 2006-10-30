import libpysmacq

SMACQ_NONE = libpysmacq.smacq_result(0)
SMACQ_FREE = libpysmacq.smacq_result(1)
SMACQ_PASS = libpysmacq.smacq_result(2)
SMACQ_ERROR = libpysmacq.smacq_result(4)
SMACQ_END = libpysmacq.smacq_result(8)
SMACQ_CANPRODUCE = libpysmacq.smacq_result(256)
SMACQ_PRODUCE = libpysmacq.smacq_result(512)

class SmacqModule:
    """This is the base class used to create modules for smacq in python.
    To create a smacq module, you need to do the following:
    1. Create a class that inherits from this one.  Overloaded methods should take the same
        arguments as the version in this class.
    2. Enter it into the smacq list of available python modules by creating an instance of 
        libpysmacq.SmacqPyModule(module_name, class_object)
    Remember, these modules can only be used from python directly, and cannot be used via smacqq
    command line tool or the smacq api directly.  These modules should not be expected to be fast,
    they are primarily for experimentation and prototyping."""

    def __init__(self, scheduler, node):
        
        self.scheduler = scheduler
        self.node = node

    def consume(self, datum, outchan):
        """The consume() method is called when there is new data for a module to process.  It is 
    passed a data (dts) object and an output channel descriptor.  It should return a smacq_result
    object.  The result should be SMACQ_PASS if the object is not filtered out and SMACQ_FREE if 
    it is.  In addition, the return code can be OR'd with the following flags: SMACQ_ERROR specifies 
    that there was a fatal error in the module.  SMACQ_END signifies that the module wishes to never 
    be called again."""
        print "In smacqmod.py: Consuming"

        return SMACQ_FREE

    def produce(self, outchan):
        """The produce() method is called when SMACQ expects an object to produce new data.  It
    should be passed a channel descriptor.  It should return a tuple where the 0th element is a
    DtsObject and the 1st element is a smacq_result object."""
        print "In smacqmod.py: Producing"
    
        return (None, SMACQ_NONE)

    def enqueue(self, datum, outchan):
        """Enqueue an object for output to the specified output channel.
This shouldn't be overloaded."""
        self.scheduler.enqueue(self.node, datum, outchan)
        
