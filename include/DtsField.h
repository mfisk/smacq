#ifndef DTSFIELD_H
#define DTSFIELD_H

#include <vector>

typedef unsigned short dts_field_element;

//typedef DtsField::iterator DtsField;

class DtsField : public std::vector<dts_field_element> {
  public:
	operator bool() const { return size()!=0; }
	bool operator  ! () const { return !size(); }
};

#endif
