from pysmacqq import *
import time

# TODO:
# Change all instances of raise Exception to raise More_Appropriate_Exception
# A fetch that doesn't "block" while waiting for results.

class SmacqQuery:  # {{{
    """Executes a query in the SMACQ "System for Modular Analysis and Continous Queries" API.  
All SmacqQuery objects share the same instantiation of SMACQ.  Redundancies in the query graph are
eliminated automatically.  
Currently: 
    1. Output from all queries are gathered from the same output queue.  This should change in later versions.
    2.  """
    
    graph = None
    dts = None
    scheduler = None
    running = False

    def __init__(self, query_str, run_now = True):  # {{{
        if (SmacqQuery.graph is None) and (SmacqQuery.dts is None) and (SmacqQuery.scheduler is None):
            SmacqQuery.scheduler = SmacqScheduler()
            SmacqQuery.dts = DTS()
            SmacqQuery.graph = SmacqGraph()
            SmacqQuery.graph.addQuery(SmacqQuery.dts, SmacqQuery.scheduler, query_str)
            SmacqQuery.graph.init(SmacqQuery.dts, SmacqQuery.scheduler)

            if run_now:
                self.run()
        else: 
            SmacqQuery.graph.addQuery(SmacqQuery.dts, SmacqQuery.scheduler, query_str)
    # end SmacqQuery.__init__() }}}

    def run(self):  #{{{
        """Starts the SmacqScheduler.  Must be executed before query can be processed"""
        if SmacqQuery.running:
            print "The Smacq Scheduler is already running.  This only needs to be executed once regardless of how many SmacqQuery objects you create."
        else:
            SmacqQuery.scheduler.seed_produce(SmacqQuery.graph)
            SmacqQuery.scheduler.start_threads(0)
            SmacqQuery.running = True
        
        return  
    # end SmacqQuery.run() }}}

    def is_running(self): # {{{
        return SmacqQuery.running
    # }}}
  
    def fetch(self, num_results = 1): # {{{
        """Returns num_results SmacqResult objects in a list.  This will wait for results if it
needs to.  
If the number of results returned is less than requested, then the query has been completed."""

        if not self.is_running():
            raise Exception, "You cannot fetch a query result if smacq is not running. Try the run() method."
        query_results = []
        for i in range(num_results):
            result = SmacqResult( SmacqQuery.scheduler.get())
            if result is None:
                break
            else:
                query_results.append( SmacqResult(result) )
        
        return query_results
# end SmacqQuery::fetch }}}

    def fetchall(self, result_limit = None, time_limit = None): # {{{
        """Fetches all results produced by the current query.  Note that querying an unbounded
data source will cause the function to never return or run out of memory.  Returns a tuple
containing the list of results and the reason it returned. 
Two limiting parameters are provided: result_limit and time_limit (in seconds).  If either limit is reached, the query will return it's results immediately. If either limit is set to None (default) or zero, it has
no effect. """

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

            if result is None:
                stop_reason = "done"
                break
            
            results.append( SmacqResult(result) )
            
        return (results, stop_reason)
# end SmacqQuery.fetchall() }}} 

    def busyloop(self): # {{{
        """Runs the query until it is done, but throws out any results"""
        SmacqQuery.scheduler.busyloop()
    # end SmacqQuery.busyloop() }}}

    def __done(self): # {{{
        # While this isn't currently needed, it will be if non-blocking fetches are implemented.
        """Returns True if the query is done processing, False otherwise"""
        return SmacqQuery.scheduler.pydone()
    # end SmacqQuery.done() }}}

    # Iterator methods {{{
    def __iter__(self): 
        return self

    def next(self): 
        return self.fetch(1)
    # }}}


# end SmacqQuery }}}

class SmacqResult:
    """Contains the data for a result returned by smacq."""
    def __init__(self, DtsObject):
        self.__data = DtsObject.get()

    def __getitem__(self, index):
        return self.getfield(index, False).get().getfield('string').get().getdata()


