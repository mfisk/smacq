#ifndef INTRUSIVE_PTR_HPP_INCLUDED
#define INTRUSIVE_PTR_HPP_INCLUDED
#define REFINFO

// Keep optional debugging info 
#ifdef REFINFO

// We just need an int that initializes to 0
class Counter {
   public:
        Counter() : count(0) {};

        short int operator++() { return ++count; }
        short int operator--() { return --count; }

        short int count;
};
#include <map>
typedef std::map<void*,std::map<void*, Counter> > RefMap;
extern RefMap REFS;

#endif

//
//  intrusive_ptr.hpp
//
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  See http://www.boost.org/libs/smart_ptr/intrusive_ptr.html for documentation.
//

#include <functional>           // for std::less
#include <iosfwd>               // for std::basic_ostream

namespace boost
{

//
//  intrusive_ptr
//
//  A smart pointer that uses intrusive reference counting.
//
//  Relies on unqualified calls to
//  
//      void intrusive_ptr_add_ref(T * p);
//      void intrusive_ptr_release(T * p);
//
//          (p != 0)
//
//  The object is responsible for destroying itself.
//
template<class T> class intrusive_ptr
{
private:

    typedef intrusive_ptr this_type;

    void incref() {
        if(p_ != 0) {
		intrusive_ptr_add_ref(p_);
#ifdef REFINFO
		++REFS[this][p_];
#endif
	}
    }

    void decref() {
        if(p_ != 0) {
		intrusive_ptr_release(p_);
#ifdef REFINFO
		--REFS[this][p_];
#endif
	}
    }

public:

    typedef T element_type;

    intrusive_ptr(): p_(0)
    {
    }

    intrusive_ptr(T * p, bool add_ref = true): p_(p)
    {
	if (add_ref) incref();
    }

#if !defined(BOOST_NO_MEMBER_TEMPLATES) || defined(BOOST_MSVC6_MEMBER_TEMPLATES)

    template<class U> intrusive_ptr(intrusive_ptr<U> const & rhs): p_(rhs.get())
    {
	incref();
    }

#endif

    intrusive_ptr(intrusive_ptr const & rhs): p_(rhs.p_)
    {
        incref();
    }

    ~intrusive_ptr()
    {
	decref();
    }

#if !defined(BOOST_NO_MEMBER_TEMPLATES) || defined(BOOST_MSVC6_MEMBER_TEMPLATES)

    template<class U> intrusive_ptr & operator=(intrusive_ptr<U> const & rhs)
    {
#ifdef REFINFO
	// This is not exception safe
	decref();
	p_ = rhs.p_;
	incref();
#else
        this_type(rhs).swap(*this);
#endif
        return *this;
    }

#endif

    intrusive_ptr & operator=(intrusive_ptr const & rhs)
    {
#ifdef REFINFO
	// This is not exception safe
	decref();
	p_ = rhs.p_;
	incref();
#else
        this_type(rhs).swap(*this);
#endif
        return *this;
    }

    intrusive_ptr & operator=(T * rhs)
    {
#ifdef REFINFO
	// This is not exception safe
	decref();
	p_ = rhs;
	incref();
#else
        this_type(rhs).swap(*this);
#endif
        return *this;
    }

    T * get() const
    {
        return p_;
    }

    T & operator*() const
    {
        return *p_;
    }

    T * operator->() const
    {
        return p_;
    }

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type () const
    {
        return p_ == 0? 0: &this_type::p_;
    }

    void swap(intrusive_ptr & rhs)
    {
        T * tmp = p_;
        p_ = rhs.p_;
        rhs.p_ = tmp;
    }

private:

    T * p_;
};

template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
    return a.get() != b.get();
}

template<class T> inline bool operator==(intrusive_ptr<T> const & a, T * b)
{
    return a.get() == b;
}

template<class T> inline bool operator!=(intrusive_ptr<T> const & a, T * b)
{
    return a.get() != b;
}

template<class T> inline bool operator==(T * a, intrusive_ptr<T> const & b)
{
    return a == b.get();
}

template<class T> inline bool operator!=(T * a, intrusive_ptr<T> const & b)
{
    return a != b.get();
}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
{
    return a.get() != b.get();
}

#endif

template<class T> inline bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
{
    return std::less<T *>()(a.get(), b.get());
}

template<class T> void swap(intrusive_ptr<T> & lhs, intrusive_ptr<T> & rhs)
{
    lhs.swap(rhs);
}

// mem_fn support

template<class T> T * get_pointer(intrusive_ptr<T> const & p)
{
    return p.get();
}

template<class T, class U> intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const & p)
{
    return static_cast<T *>(p.get());
}

template<class T, class U> intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const & p)
{
    return const_cast<T *>(p.get());
}

template<class T, class U> intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const & p)
{
    return dynamic_cast<T *>(p.get());
}

// operator<<

#if defined(__GNUC__) &&  (__GNUC__ < 3)

template<class Y> std::ostream & operator<< (std::ostream & os, intrusive_ptr<Y> const & p)
{
    os << p.get();
    return os;
}

#else

template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, intrusive_ptr<Y> const & p)
{
    os << p.get();
    return os;
}

#endif

} // namespace boost

#endif  // #ifndef INTRUSIVE_PTR_HPP_INCLUDED
