// Includes {{{
#include <smacq.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>
#include <dts.h>
#include <DtsObject.h>
#include <iostream>
#include <vector>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <ThreadSafe.h>
// }}}

int simple_query(char * query) {  // {{{
    
    SmacqScheduler s;
    DTS dts;
    int retval;

    SmacqGraph graph;

    graph.addQuery(&dts, &s, (std::string)query);
    graph.print(stderr, 8);

    graph.init(&dts, &s);

    std::cout << "before \n";
    s.seed_produce(&graph);
    std::cout << "after \n";
    s.start_threads(0);

    retval = (! s.busy_loop());

    return retval;
} // }}}

bool is_dtsobj_null(DtsObject d) { // {{{
// Since python can't really tell the difference between a NULL and not null object, the check
// needs to be done in C++ land.
    if (d == NULL) 
      return 1;
    else 
      return 0;
} // }}}

// Avoiding collision amongst overloaded operators.   {{{
void    (SmacqGraph::*add_graph_fptr)(SmacqGraph *, bool)    = &SmacqGraph::add_graph;
void    (SmacqGraph::*join_fptr)(SmacqGraph *, bool)    = &SmacqGraph::join;
void    (SmacqScheduler::*seed_produce_fptr)(SmacqGraph *)   = &SmacqScheduler::seed_produce;
DtsObject (DTS::*newObject_fptr)(dts_typeid) = &DTS::newObject;
DtsObject (DtsObject_::*getfield_fptr_Fo)(DtsField &, bool) = &DtsObject_::getfield;
DtsObject (DtsObject_::*getfield_fptr_s)(char *, bool) = &DtsObject_::getfield;
// }}}

// Exposing smacq methods and functions to python {{{
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SG_join_overloads, join, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SG_clone_overloads, clone, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SG_add_graph_overloads, add_graph, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SG_init_overloads, init, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dtsO_getfield_s_overloads, getfield, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dtsO_getfield_Fo_overloads, getfield, 1, 2)

BOOST_PYTHON_MODULE(libpysmacq)
{
    def("query", simple_query);
    def("is_dtsobj_null", is_dtsobj_null);

    class_<std::vector<int> >("Ivec")
        .def(vector_indexing_suite<std::vector<int> >())
    ;

    class_<std::vector<DtsObject> >("DtsObject_vec") 
        .def(vector_indexing_suite<std::vector<DtsObject> >())
    ;

    class_<SmacqGraph>("SmacqGraph", init<>())
        .def("init", &SmacqGraph::init, SG_init_overloads())
        .def("add_graph", add_graph_fptr, SG_add_graph_overloads())
        .def("addQuery", &SmacqGraph::addQuery)
        .def("join", join_fptr, SG_join_overloads())
        .def("clone", &SmacqGraph::clone, 
            return_value_policy<manage_new_object>(),
            SG_clone_overloads())
        .def("print_query", &SmacqGraph::print_query,
            return_value_policy<return_by_value>())
    ;

    class_<DTS>("DTS", init<>())
        .def("newObject", newObject_fptr)
        .def("requirefield", &DTS::requirefield)
        .def("field_getname", &DTS::pyfield_getname)
//            return_value_policy<manage_new_object>())
    ;

    class_<DtsField>("DtsField", init<dts_field_element>())
    ;

    class_<DtsObject>("DtsObject") //, init<int, dts_typeid>())
        .def("get", &DtsObject::get,
            return_internal_reference<>())
    ;

    class_<DtsObject_>("DtsObject_", init<DTS *, int, int>())
        .def("getfield", getfield_fptr_s, dtsO_getfield_s_overloads())
        .def("getfield", getfield_fptr_Fo, dtsO_getfield_Fo_overloads())
        .def("getdata", &DtsObject_::pygetdata,
            return_value_policy<return_by_value>())
        .def("prime_all_fields", &DtsObject_::prime_all_fields)
        .def("fieldcache", &DtsObject_::fieldcache)
    ;

    class_<SmacqScheduler>("SmacqScheduler",init<>())
        .def("seed_produce", seed_produce_fptr)
        .def("start_threads", &SmacqScheduler::start_threads)
        .def("busy_loop", &SmacqScheduler::busy_loop)
        .def("get", &SmacqScheduler::get)
        .def("element", &SmacqScheduler::pyelement)
        .def("done", &SmacqScheduler::done)
    ;
    
/*    class_<SmacqGraphNode>("SmacqGraphNode", init<std::string>())
//        .def("init", &SmacqGraph::init)
    ;
*/  
} // }}} 

