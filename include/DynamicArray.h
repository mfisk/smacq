#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <vector>

/// This is a wrapper around vector which grows the array
/// as necessary to satisfy [] operations.
template <class T>
class DynamicArray : public std::vector<T> {
   public:
        T& operator[](const unsigned int x) {
                if (x >= size())
                        resize(x+1);
                return (std::vector<T>::operator [](x));
        }
};

template<>
class DynamicArray<bool> : public std::vector<bool> {
   public:
        reference operator[](const unsigned int x) {
                if (x >= size())
                        resize(x+1);
                return (std::vector<bool>::operator [](x));
        }
};


#endif

