#ifndef DTS_H
#define DTS_H

#ifndef __cplusplus
#warning "C++ is required when including dts.h"
#endif 

#include <smacq.h>
#include <gmodule.h>
#include <assert.h>
#include <dts-types.h>
#include <dts-filter.h>
#include <dts-module.h>
#include <DtsField.h>
#include <stack>
#include <map>
#include <DynamicArray.h>

static inline char * dts_fieldname_append(const char * old, const char * newf) {
  char * ret = (char*)malloc(strlen(old) + strlen(newf) + 2);
  sprintf(ret, "%s.%s", old, newf);
  return ret;
}

typedef int smacqtype_lt_fn(void *, int, void *, int);
typedef int smacqtype_fromstring_fn(char *, DtsObject);
typedef int smacqtype_getfield_fn(DtsObject, DtsObject, dts_field_element);

struct dts_type_info {
  int size;
  smacqtype_fromstring_fn * fromstring;
  smacqtype_lt_fn * lt;
  smacqtype_getfield_fn * getfield;
};

class dts_type {
 public:
  DynamicArray<struct dts_field_info *> fields;
  char * name;
  int num;
  GModule * module;
  struct dts_field_spec * description;
  struct dts_type_info info;
};

typedef int dts_typeid;

struct dts_field_info {
 //public:
  dts_typeid type;
  int offset;
  dts_field_spec desc;
  dts_field_element elementid;
};

typedef struct _dts_msg {
  DtsObject field_data;
  dts_comparison * criteria;
  struct _dts_msg * next;
} dts_message;

struct ltstr 
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

/// DTS is a Dynamic Type System run-time environment.
/// You probably only want one instance of the DTS
/// for your entire program.
/// Factory methods are used to construct DtsObjects,
/// which are typed using the DTS.
class DTS {
 public:
  	/// Construct a new DTS.  You probably only want your program to use 
  	/// pointers to a single instance for the whole  program.
  DTS();

  ///@name Factory Methods 
  ///@{ 

  	/// Construct a new object with a copy of the given data.
  	/// The amount of data copied is determined by the requested typeid. 
  DtsObject construct(dts_typeid, void * data);

  	/// Construct a new object with data parsed from the given string.
  	/// The input string should be format accordingly for the given typeid.
  DtsObject construct_fromstring(dts_typeid type, char * data);

  	/// Return a new object of the given type
  DtsObject newObject(dts_typeid);

  	/// Return a new object of the given type and size
  DtsObject newObject(dts_typeid, int size);

  DtsObject readObject(struct pickle * pickle, int fd);
  ///@}

  int fromstring(dts_typeid, char * datastr, DtsObject data);
  
  int dts_lt(int type, void * p1, int len1, void * p2, int len2);
  ///@name Field IDs 
  /// DtsObjects expose 0 or more fields (attributes) that can be accessed.
  /// Each field is assigned a numeric identifier, a DtsField,  
  /// specific to this runtime environment.
  /// Fields names can be nested (e.g. "foo.bar.baz") which translates to
  /// nested numeric IDs (e.g. "1.3.2").
  /// It is recommended
  /// for performance that modules convert type names to IDs sparingly and cache 
  /// results. 
  ///@{

  	/// Convert the given field name into a numeric identifier.
  DtsField requirefield(char * name);

	/// Find the location, size, and type of a field of the specified object
  //int getfieldoffset(DtsObject datum, dts_field_element fnum, dts_typeid * dtype, int * offset, int * len);

  	/// Return the name of the specified field.
  char * field_getname(DtsField &f);

  ///@}

  ///@name Type IDs 
  /// Types are dynamically loaded classes.  Each type is assigned
  /// a numeric identifier specific to this runtime environment.
  /// All DtsObjects are typed with these values.
  /// It is recommended
  /// for performance that modules convert type names to IDs sparingly and cache 
  /// results. 
  ///@{

  	/// Load the specified type module
  	/// (if it is not already loaded) and return the dynamically assigned numeric
  	/// identifier for that type.
  dts_typeid requiretype(const char * name);

  	/// If the specified type module is already loaded, this
  	/// result is the same as requiretype().  Unlike
  	/// requiretype(), if the type is not loaded, -1 is returned.
  dts_typeid typenum_byname(const char * name);

  	/// Return the name of the given type 
  char * typename_bynum(const dts_typeid);
 
  	/// Return the type structure for the given type
  struct dts_type * type_bynum(const dts_typeid id) { return types[id]; }
 
  	/// Return the size (in bytes) of the specified type.
  	/// -1 if size is variable, -2 if type doesn't exist.
  int type_size(const dts_typeid type) {
    struct dts_type * t = this->type_bynum(type);
    if (!t) return -2;
    return t->info.size;
  }

  ///@}
  
  ///@name Interface to data testing system 
  ///@{

  int parsetest(dts_comparison * comp, char * test);
  int match(DtsObject datum, dts_comparison * comps);
  
  dts_comparison * parse_tests(int argc, char ** argv);
  struct dts_operand * parse_expr(int argc, char ** argv);
  ///@}
 
  	/// This freelist should only be used by the DtsObject implementation.
        /// When an object is freed, it can be put on the freelist instead of
	/// being destroyed.  newObject() will use objects on the freelist
	/// before constructing new objects.
  std::stack<DtsObject> freelist;

  int maxfield() { return max_field; }
 
  ///@name Messaging 
  ///@{ 
  	/// Send the given object to the given field of all objects that satisfy the comparison
  void send_message(DtsObject, dts_field_element, dts_comparison *);

  	/// Check for a message for this obejct and field
  DtsObject msg_check(DtsObject o, dts_field_element field);
  ///@}

  ///@name Warnings
  ///@{
	/// Don't warn 
	void set_no_warnings() { warnings = false; }

    /// Get current setting
    bool warn_missing_fields() { return warnings; }
  ///@}

 private:
  int max_type; 
  int max_field; 
  bool warnings;

  std::map<const char *, struct dts_type *, ltstr> types_byname;
  std::map<const char *, int, ltstr> fields_byname;
  DynamicArray<dts_message*> messages_byfield;
  DynamicArray<struct dts_type *> types; 
  DynamicArray<char*> fields_bynum; 
  
  //  	DtsField double_field;
  //  	int double_type;
  
  dts_field_element requirefield_single(char * name);
  
  int pickle_getnewtype(int fd, struct sockdatum * hdr, struct pickle * pickle);
  int pickle_addnewtype(char * name, unsigned int extnum, struct pickle * pickle, int fd);
  void print_comp(dts_comparison * c);

  struct dts_operand * comp_arith(enum dts_arith_operand_type op, struct dts_operand * op1, struct dts_operand * op2);
  void make_fields_doubles(struct dts_operand * operand);

  friend int yysmacql_parse();
  friend int yyfilterparse();
  friend int yyexprparse();
};

inline char * DTS::typename_bynum(const dts_typeid num) {
  assert(type_bynum(num));
  return type_bynum(num)->name;
}

inline int DTS::typenum_byname(const char * name) {
  std::map<const char *, struct dts_type*, ltstr>::iterator i;
  i = types_byname.find(name);
  if (i == types_byname.end()) return -1;
  return (*i).second->num;
}


BEGIN_C_DECLS
 
int dts_comparefields(DtsField &a, DtsField &b);

//void dts_decref(DtsObject d);
	
END_C_DECLS

#include <DtsObject.h>

#endif

