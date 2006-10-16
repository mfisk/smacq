"""System for Modular Analysis and Continuous Queries.

See http://smacq.sourceforge.net/
"""

import libpysmacq
import time, sys

# TODO:
# Change all instances of raise Exception to raise More_Appropriate_Exception

class SmacqQuery:  # {{{
    """Executes one or more queries in the SMACQ (System for Modular Analysis and Continous Queries) API."""
    
    graph = None
    dts = None
    scheduler = None
    running = False

    def __init__(self, query_str = None, run_now = False):  
        self.__running = False
        self.scheduler = libpysmacq.SmacqScheduler()
        self.dts = libpysmacq.DTS()
        self.graph = libpysmacq.SmacqGraph()
	if query_str:
            self.graph.addQuery(self.dts, self.scheduler, query_str)

        if run_now:
            self.run()

    def run(self, ignoreDups = False):  
        """Adds this query to the main SmacqGraph and runs it.  If the scheduler hasn't already been
started, then it is started."""

        if self.__running:
	    if not ignoreDups:
            	print "This query is already running."
        else:
                self.graph.init(self.dts, self.scheduler)
                self.scheduler.seed_produce(self.graph)
                self.scheduler.start_threads(0)
                self.__running = True
        
        return  

    def is_running(self): 
        return self.__running

# Fetching Methods {{{  
    def fetchone(self):
	"""Fetch the next result object and return it, or None when no more data is available"""
	return self.fetchmay(1)

    def fetchmany(self, num_results = 1): # {{{
        """Returns num_results DtsObject objects in a list.  This will wait for results if it
    needs to.  If the number of results returned is less than requested, then the 
    query has been completed."""

        self.run(True)
        query_results = []

        for i in range(num_results):
            result = self.scheduler.get()
            if result:
                query_results.append( result )
            else:
                break
        
        return query_results

    def fetch_nb(self, num_results = 1): # {{{
        """Performs a non-blocking fetch of num_results data items.
    To test if the query is done, check the value of done().  If done is True, then a fetchall
    performed afterwards should return the remaining results without blocking."""

        self.run(True)
        query_results = []
    
        for i in range(num_results):
            result = self.scheduler.element()
         
            if result:
                query_results.append( result ) 
            else:
                break
    
        return query_results
 
    def fetchall(self, result_limit = None, time_limit = 0): # {{{
        """Fetches all results produced by the current query.  Note that querying an unbounded
    data source will cause the function to never return or run out of memory.  Returns a tuple
    containing the list of results and the reason it returned. 
    Two limiting parameters are provided: result_limit and time_limit (in seconds).  
    If either limit is reached, the query will return it's results immediately. If either limit 
    is set to None (default) or zero, it has no effect. """

        self.run(True)

        if result_limit == 0:
            result_limit = None

        time_done = time.time() + time_limit
        num_results = 0
        results = []

        while (True):
            if result_limit is not None:
                if num_results >= result_limit:
                    stop_reason = "max_results"
                    break
                else:
                    num_results += 1

            if time_limit != 0 and time.time >= time_done:
                stop_reason = "time"
                break

            result = self.scheduler.get()

            if not result:
                stop_reason = "done"
                break
            
            results.append( result )
            
        return (results, stop_reason)

    def busy_loop(self): 
        """Runs the query until it is done, but throws out any results"""
	self.run(True)
        self.scheduler.busy_loop()
        
    def __done(self): 
        # While this isn't currently needed, it will be if non-blocking fetches are implemented.
        """Returns True if the query is done processing, False otherwise"""
        return self.scheduler.done()

    #end Fetching methods }}}

    # Iterator methods {{{
    def __iter__(self): 
	"""Return self in compliane with iterator protocol."""
	self.run(True)
        return self

    def next(self): 
	"""Return the next DtsObject returned by the query.  Raise StopIteration when complete."""
	x = self.scheduler.get()
	if x:
		return x
	else:
		raise StopIteration
	
    # }}}

    # Join methods {{{
    def append(self, query):
        """Joins this query with the other_query.  
other_query can be either a string or a SmacqGraph object"""
        if type(query) == str:
            newg = libpysmacq.SmacqGraph()
	    newg.addQuery(self.dts, self.scheduler, query)
            self.graph.join(newg)

	elif type(query) == libpysmacq.SmacqGraph:
            self.graph.join(query)

	else:
	    print type(query)
	    raise TypeError

    def __iadd__(self, query): 
	"""This is the += operator."""

	self += query
	return self

    def add(self, query): 
        """Adds the query on the righthand side to the query on the left.
If the right hand side is a query string, it is used to create a new query object first."""
       
	if type(query) == str:
		self.graph.addQuery(self.dts, self.scheduler, query)
	else:
        	self.graph.addQuery(query)


    def __rshift__(self, query): 
        """This is the >>= operator.  Joins the query on the righthand side with the query on the left.
If the right hand side is a query string, it is used to create a new query object first."""
      
	self.append(query)
        return self
 
    def __add__(self, query):
        """Adds two queries together, and returns a new query as a result"""
        
        newQuery = self.clone()
        newQuery += (query)

        return newQuery
    # end join methods }}}

    def __str__(self): # {{{
        return self.graph.print_query() 
    # }}}

# end SmacqQuery }}}

def DtsObjectGetItem(self, index):
	x = self.get().getfield(index, True) 
	if not x.get():
		raise KeyError, "DtsObject instance does not contain field " + index
	return x
libpysmacq.DtsObject.__getitem__ =  DtsObjectGetItem
del DtsObjectGetItem

libpysmacq.DtsObject.has_key = lambda self, name: (self.get().getfield(name, True).get() != None)
libpysmacq.DtsObject.__getattr__ = lambda self, name: self.get().__getattribute__(name)
libpysmacq.DtsObject.__str__ = lambda self: self["string"].getdata()
libpysmacq.DtsObject.__repr__ = lambda self: self.get().getdata()
libpysmacq.DtsObject.__nonzero__ = lambda self: (self.get() != None)

def DtsObject_keys(self, field_refs = False): 
        """Returns a list of field names for this object.  
    If field_refs is True, DtsField objects are returned instead.  DtsField objects can be used instead
    of field name strings for DtsObject field lookups, and are signifigantly faster.  The DtsField
    objects will be returned in the same order as the field names."""

        self.prime_all_fields()
        fields = self.fieldcache() 

        field_names = [] 
        
        field_nums = []
        for i in range( len(fields) ):
            if fields[i].get() is not None:
                field_nums.append(i)

        if field_refs:
            # Make a list of DtsField objects
            for i in field_nums:
                field_names.append( libpysmacq.DtsField(i) )

        else:
            # Make a list of field names
            field_getname = dts.field_getname
    
            for i in field_nums:
                field_names.append( field_getname( libpysmacq.DtsField(i) ) )
        
        return field_names
libpysmacq.DtsObject_.keys = DtsObject_keys
del DtsObject_keys
	
