#ifndef DTS_TYPES_H
#define DTS_TYPES_H

typedef unsigned short dts_field_element;
typedef dts_field_element * dts_field;

typedef int field_getfunc_fn(DtsObject*, DtsObject*);

typedef int smacqtype_lt_fn(void *, int, void *, int);
typedef int smacqtype_fromstring_fn(char *, DtsObject *);
typedef int smacqtype_getfield_fn(DtsObject *, DtsObject *, dts_field_element);

#endif
