#ifndef DTS_TYPES_H
#define DTS_TYPES_H

#ifndef __cplusplus
typedef void * DtsObject;
typedef void DTS;
#else


class DTS;
class DtsObject_;

extern void intrusive_ptr_add_ref(DtsObject_ *);
extern void intrusive_ptr_release(DtsObject_ *);

#include <intrusive_ptr.hpp>

#ifdef DOXYGEN_KLUDGE
typedef DtsObject_* DtsObject;
#else
typedef boost::intrusive_ptr<DtsObject_> DtsObject;
#endif

#endif

typedef unsigned short dts_field_element;
typedef dts_field_element * dts_field;

typedef int smacqtype_lt_fn(void *, int, void *, int);
typedef int smacqtype_fromstring_fn(char *, DtsObject);
typedef int smacqtype_getfield_fn(DtsObject, DtsObject, dts_field_element);

#endif
