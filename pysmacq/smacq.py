import libpysmacq
import time

# TODO:
# Change all instances of raise Exception to raise More_Appropriate_Exception
# A fetch that doesn't "block" while waiting for results.

class SmacqQuery:  # {{{
    """Executes a query in the SMACQ "System for Modular Analysis and Continous Queries" API.  
All SmacqQuery objects share the same instantiation of SMACQ.  Redundancies in the query graph are
eliminated automatically.  
Currently: 
    1. Output from all queries are gathered from the same output queue.  This is a smacq 
    issue, not an issue with pysmacq"""
    
    graph = None
    dts = None
    scheduler = None
    running = False

    def __init__(self, query_str, run_now = False):  # {{{
        if (SmacqQuery.graph is None) and (SmacqQuery.dts is None) and (SmacqQuery.scheduler is None):
            SmacqQuery.scheduler = libpysmacq.SmacqScheduler()
            SmacqQuery.dts = libpysmacq.DTS()
            SmacqData.str_field = SmacqQuery.dts.requirefield('string')
            SmacqData.dts = SmacqQuery.dts
            SmacqQuery.graph = libpysmacq.SmacqGraph()

            self.__running = False

            if run_now:
                self.run()

        self.graph = libpysmacq.SmacqGraph()
        self.graph.addQuery(SmacqQuery.dts, SmacqQuery.scheduler, query_str)
    # end SmacqQuery.__init__() }}}

    def run(self):  #{{{
        """Adds this query to the main SmacqGraph and runs it.  If the scheduler hasn't already been
started, then it is started."""

        if self.__running:
            print "This query is already running."
        else:
            SmacqQuery.graph.add_graph(self.graph)            
            self.__running = True
            
            if not SmacqQuery.running: 
                SmacqQuery.graph.init(SmacqQuery.dts, SmacqQuery.scheduler)
                SmacqQuery.scheduler.seed_produce(SmacqQuery.graph)
                SmacqQuery.scheduler.start_threads(0)
                SmacqQuery.running = True
        
        return  
    # end SmacqQuery.run() }}}

    def is_running(self): # {{{
        return self.__running
    # }}}

# Fetching Methods {{{  
    def fetch(self, num_results = 1): # {{{
        """Returns num_results SmacqData objects in a list.  This will wait for results if it
    needs to.  If the number of results returned is less than requested, then the 
    query has been completed."""

        if not self.is_running():
            raise Exception, "You cannot fetch a query result if smacq is not running. Try the run() method."
        query_results = []

        for i in range(num_results):
            result = SmacqQuery.scheduler.get()
            if libpysmacq.is_dtsobj_null(result):
                break
            else:
                print result 
                query_results.append( SmacqData(result) )
        
        return query_results
# end SmacqQuery::fetch }}}

    def fetch_nb(self, num_results = 1): # {{{
        """Performs a non-blocking fetch of num_results data items.
    To test if the query is done, check the value of done().  If done is True, then a fetchall
    performed afterwards should return the remaining results without blocking."""

        if not self.is_running():
            raise Exception, "You cannot fetch a query result if smacq is not running. Try the run() method."
        
        query_results = []
    
        for i in range(num_results):
            result = SmacqQuery.scheduler.element()
         
            if libpysmacq.is_dtsobj_null(result):
                break
            else:
                query_results.append( SmacqData(result) ) 
    
        return query_results
    # end SmacqQuery.fetch_nb() }}}
 
    def fetchall(self, result_limit = None, time_limit = None): # {{{
        """Fetches all results produced by the current query.  Note that querying an unbounded
    data source will cause the function to never return or run out of memory.  Returns a tuple
    containing the list of results and the reason it returned. 
    Two limiting parameters are provided: result_limit and time_limit (in seconds).  
    If either limit is reached, the query will return it's results immediately. If either limit 
    is set to None (default) or zero, it has no effect. """

        if (not self.is_running()):
            raise Exception, "You cannot fetch query results if smacq is not running. Try the run()"

        if result_limit == 0:
            result_limit = None

        if time_limit is 0:
            time_limit = None

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

            if time_limit is not None and time.time >= time_done:
                stop_reason = "time"
                break

            result = SmacqQuery.scheduler.get()

            if libpysmacq.is_dtsobj_null(result):
                stop_reason = "done"
                break
            
            results.append( SmacqData(result) )
            
        return (results, stop_reason)
# end SmacqQuery.fetchall() }}} 

    def busy_loop(self): # {{{
        """Runs the query until it is done, but throws out any results"""
        if self.is_running:
            SmacqQuery.scheduler.busy_loop()
        else:
            raise Exception, "You cannot fetch query results if smacq is not running. Try the run()"
    # end SmacqQuery.busyloop() }}}

    def __done(self): # {{{
        # While this isn't currently needed, it will be if non-blocking fetches are implemented.
        """Returns True if the query is done processing, False otherwise"""
        return SmacqQuery.scheduler.pydone()
    # end SmacqQuery.done() }}}
#end Fetching methods }}}

    # Iterator methods {{{
    def __iter__(self): 
        return self

    def next(self): 
        return self.fetch(1)
    # }}}

    # Join methods {{{
    def join(self, other_query):
        """Joins this query with the other_query.  
other_query can be either a string or a SmacqQuery object"""
        if type(other_query) == str:
            query2 = SmacqQuery(other_query)

        self.graph.join(other_query.graph)

    def __iadd__(self, query2): 
        """Joins the query on the righthand side with the query on the left.
If the right hand side is a query string, it is used to create a new query object first."""
        
        self.join(query2)
        return self

    def __add__(self, query2):
        """Joins two queries together, and returns a new query as a result"""
        
        newQuery = self.clone()
        newQuery.join(query2)

        return newQuery
    # end join methods }}}

    def __str__(self): # {{{
        return self.graph.print_query() 
    # }}}

# end SmacqQuery }}}

class SmacqData: # {{{
    """Contains the data for a result returned by smacq."""
    
    str_field = None
    dts = None

    def __init__(self, DtsObject): #{{{
        self.__data = DtsObject.get()
        print self.__data
    # }}}

    def __getitem__(self, index): #{{{
        base_data = self.__data.getfield(index, False).get()
        if base_data is None:
            return None
        else:
            return base_data.getfield(SmacqData.str_field).get().getdata()
    #}}}

    def keys(self, field_refs = False): #{{{
        """Returns a list of field names for this object.  
    If field_refs is True, DtsField objects are returned instead.  DtsField objects can be used instead
    of field name strings for SmacqData field lookups, and are signifigantly faster.  The DtsField
    objects will be returned in the same order as the field names."""

        self.__data.prime_all_fields()
        fields = self.__data.fieldcache() 
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
            field_getname = SmacqData.dts.field_getname
    
            for i in field_nums:
                field_names.append( field_getname( libpysmacq.DtsField(i) ) )
        
        return field_names
    # end SmacqData.keys() }}}

# end SmacqData }}}
