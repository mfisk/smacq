#ifndef FIELD_VEC_H
#define FIELD_VEC_H
#include <vector>
#include <dts.h>
#include <IoVec.h>

class FieldVecElement {
 public:
  char * name;
  dts_field num;
  DtsObject field_obj;

  FieldVecElement();
  ~FieldVecElement();
};

class FieldVec : public std::vector<FieldVecElement*> {
 public:
  /// Initialize fields from this DtsObject
  FieldVec & getfields(DtsObject o);

  /// Return true iff one of the vector elements is undefined for this
  /// DtsObject.  Recomputed when getfields() is called.
  bool has_undefined() { return has_undefined_element; }

  /// Initialize field vector from an argument vector.  Deletes any previous contents.
  void init(DTS *, int argc, char ** argv);
 
  /// Construct an empty vector.  Use init() to initialize later
  FieldVec() : dts(NULL) {};

  /// Construct and initialize field vector from an argument vector
  FieldVec(DTS * dts, int argc, char** argv) { init(dts, argc, argv); }

  /// Can evaluate to IoVec
  operator IoVec () { return ioVec(); }

  /// Hash into value in [0..range]
  int hash() { return this->ioVec().hash(); }

  /*
  FieldVec& operator= (const FieldVec& a) {
    if (this == a) return this;
    this->dts = a->dts;
    this->lasttype = a->lasttype;
    this->has_undefined_element = a->has_undefined_element;
    
  }
  */

 private:
  DTS * dts;
  int lasttype;
  bool has_undefined_element;

  /// Construct and return an equivalent IoVec object
  IoVec ioVec();

};

inline FieldVecElement::FieldVecElement() : num(NULL), field_obj(NULL)
{}

inline FieldVecElement::~FieldVecElement() {
  if (num)
    dts_field_free(num);
}

inline void FieldVec::init(DTS * dts, int argc, char ** argv) {
  this->dts = dts;
  clear(); // Empty all elements

  while (argc-- > 0) {
    if (argv[0][0] == '-') {
      fprintf(stderr, "Unknown argument: %s\n", argv[0]);
      argv++;
      continue;
    } else {
      FieldVecElement * el = new FieldVecElement;
      this->push_back(el);
      el->name = argv[0];
      el->num = dts->requirefield(argv[0]);
      argv++;
    }
  }
}

inline FieldVec & FieldVec::getfields(DtsObject o) {
  FieldVecElement * el;
  FieldVec::iterator i;
  has_undefined_element = false;

  for (i = begin(); i != end(); i++) {
    el = *i;

    el->field_obj = o->getfield(el->num);

    if (! el->field_obj)
      has_undefined_element = true;
  }

  return *this;
}

inline IoVec FieldVec::ioVec() {
  FieldVec::iterator i;
  IoVec v(this->size());
  int j;

  for (i = begin(), j=0; 
       i != end(); 
       i++, j++) 
    {
      FieldVecElement * el = *i;
      
      if (el->field_obj) {
    	v[j].iov_len = el->field_obj->getsize();
    	v[j].iov_base = (unsigned char*)el->field_obj->getdata();
      } else {
	v[j].iov_len = 0;
      }
    }
  
  return v;
}

#endif
