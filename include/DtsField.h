#ifndef DTSFIELD_H
#define DTSFIELD_H

#include <vector>

typedef unsigned short dts_field_element;

//typedef DtsField::iterator DtsField;

/// DtsField is a vector class used to describe a field
/// specification such as foo.bar.baz translated into
/// numeric identifiers for fast lookup.
class DtsField : public std::vector<dts_field_element> {
  public:
	DtsField(dts_field_element fe) {
		std::vector<dts_field_element>::push_back(fe);
	}
	DtsField() {}

	operator bool() const { return size()!=0; }
	bool operator  ! () const { return !size(); }
};

#endif
