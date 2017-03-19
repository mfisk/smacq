#ifndef CXX_PTHREAD_MUTEX_H
#define CXX_PTHREAD_MUTEX_H

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/algorithm.hpp>
#include <boost/lambda/if.hpp>
#include <pthread.h>

#define RECURSIVE_LOCK(o) RECURSIVE_LOCK_NAMED(_lock, o);

#ifdef SMACQ_CONFIG_THREAD_SAFE 
#define ATOMIC_GET(x) g_atomic_int_get(& x)
#define RECURSIVE_LOCK_NAMED(n, o) RecursiveLock n(o);
#define ATOMIC_INT_COMPARE_AND_SET(x, y, z) g_atomic_int_compare_and_exchange(&x, y, z);
#define protected PthreadMutex, protected PthreadMutex, 

/// This is a Mutex that can be acquired multiple times, recursively, by the same thread
/// without causing deadlock.
class PthreadMutex {
  private:
   pthread_mutex_t mutex;

  public:
    PthreadMutex() {
	pthread_mutexattr_t attr;
	if (0 != pthread_mutexattr_init(&attr)) {
		perror("pthread_mutexattr_init");
		assert(0);
	}
	if (0 != pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) {
		perror("pthread_mutexattr_settype");
		assert(0);
	}
        if (0 != pthread_mutex_init(&mutex, &attr)) {
		perror("pthread_mutex_init");
		assert(0);
	}
	pthread_mutexattr_destroy(&attr);
    }
    ~PthreadMutex() {
        pthread_mutex_destroy(&mutex);
    }

  protected:
    void lock() {
        pthread_mutex_lock(&mutex);
    }
    bool try_lock() {
        return (0 == pthread_mutex_trylock(&mutex));
    }
    void unlock() {
        pthread_mutex_unlock(&mutex);
    }

  friend class RecursiveLock;
};

/// On instantiation, this class will lock a PthreadMutex and unlock it automatically upon destruction.
class RecursiveLock {
  public:
    RecursiveLock(PthreadMutex * _m) : m(_m) {
	m->lock();
    }
    RecursiveLock(PthreadMutex & _m) : m(&_m) {
	m->lock();
    }
    ~RecursiveLock() {
	m->unlock();
    }
  private:
    PthreadMutex * m;
};
#else // Thread-safe (not)
#define ATOMIC_GET(x) x
#define RECURSIVE_LOCK_NAMED(n, o)
static inline gboolean int_compare_and_set(int & x, int y, int z) {
	if (x==y) { 
		x = z;
		return TRUE; 
	} else { 
		return FALSE; 
	}
}
#define ATOMIC_INT_COMPARE_AND_SET(x, y, z) int_compare_and_set(x, y, z);
class PthreadMutex {
    public:
	bool try_lock() { return true; }
	bool unlock() { return true; }
};
#endif


#include <map>

/// A Thread-safe map based on an STL map.
template <typename KEY, typename VALUE, typename LT = std::less<KEY> >
class ThreadSafeMap : protected PthreadMutex, private std::map<KEY, VALUE, LT> {
  public:
    /// Return true iff key already exists.  
    bool get(KEY k, VALUE & v) {
	typename std::map<KEY, VALUE, LT>::iterator i;
        RECURSIVE_LOCK(this);

 	i = std::map<KEY, VALUE, LT>::find(k);
  	if (i != std::map<KEY, VALUE, LT>::end()) {
		v = i->second;
		return true;
	} else {
		return false;
	}
    }

    VALUE&  operator[](KEY k) {
        RECURSIVE_LOCK(this);
	return std::map<KEY, VALUE, LT>::operator[](k);
    }
};

#include <vector>
#include <algorithm>

template <typename Return, typename Arg, typename Class, typename Callback>
class MethodFunctor {
  public:
    MethodFunctor(Class *_self, Callback _cb) : self(_self), cb(_cb) {}

    Return operator()(Arg x) {
	return (self->*cb)(x);
    }

  private:
    Class * self;
    Callback * cb;
};

/// A base class for thread-safe containers.
template <typename T, typename CONTAINER> 
class ThreadSafeContainer : protected PthreadMutex, protected CONTAINER {
  public:
    typedef typename CONTAINER::size_type sizetype;
    ThreadSafeContainer() {}
    ThreadSafeContainer(int x) : CONTAINER(x) {}

    void clear() {
	RECURSIVE_LOCK(this);
	CONTAINER::clear();
    }
    typename CONTAINER::size_type size() {
	RECURSIVE_LOCK(this);
	return CONTAINER::size();
    }
    bool empty() {
	RECURSIVE_LOCK(this);
	return CONTAINER::empty();
    }
	
    CONTAINER snapshot() {
	RECURSIVE_LOCK(this);
	return *this;
    }

    void resize(const typename CONTAINER::size_type x) {
	RECURSIVE_LOCK(this);
	return CONTAINER::resize(x);
    }
 
    template<class U> 
    void foreach(U cb) {
    	using namespace boost::lambda;

	RECURSIVE_LOCK(this);
	if (!empty())
        	std::for_each(CONTAINER::begin(), CONTAINER::end(), cb);
    };

    template<typename U> 
    typename CONTAINER::iterator findIf(U cb) {
	RECURSIVE_LOCK(this);
        return std::find_if(CONTAINER::begin(), CONTAINER::end(), cb);
    };

    template<class U> 
    bool has_if(U cb) {
 	RECURSIVE_LOCK(this);
        typename CONTAINER::iterator f = ThreadSafeContainer<T,CONTAINER>::findIf(cb);
	if (f == CONTAINER::end()) {
		return false;
	} else {
		return true;
	}
    };

    template<typename U> 
    bool erase_first_match(U cb) {
	RECURSIVE_LOCK(this);
        typename U::iterator f = findIf(CONTAINER::begin(), CONTAINER::end(), cb);
	if (f == CONTAINER::end()) {
		return false;
	} else {
		f.erase();
		return true;
        }
    }
};

/// A base class for random-access thread-safe containers.
template <typename T, typename CONTAINER>
class ThreadSafeRandomAccessContainer : public ThreadSafeContainer<T, CONTAINER> {
   public:
    ThreadSafeRandomAccessContainer() {}
    ThreadSafeRandomAccessContainer(int x) : ThreadSafeContainer<T, CONTAINER >(x) {}

    T& operator[](size_t i) {
	RECURSIVE_LOCK(this);
	return CONTAINER::operator[](i);
    }
    void push_back(T x) {
	RECURSIVE_LOCK(this);
	return CONTAINER::push_back(x);
    }

};

/// A thread-safe version of the STL vector class.
template <typename T>
class ThreadSafeVector : public ThreadSafeRandomAccessContainer<T, std::vector<T> > {
  public:
    ThreadSafeVector() {}
    ThreadSafeVector(int x) : ThreadSafeRandomAccessContainer<T, std::vector<T> >(x) {}
};

#include <DynamicArray.h>

/// A thread-safe dynamic array.
template <typename T>
class ThreadSafeDynamicArray : public ThreadSafeRandomAccessContainer<T, DynamicArray<T> > {};

#include <stack>
/// A thread-safe version of the STL stack class.
template <class T>
class ThreadSafeStack : protected PthreadMutex, private std::stack<T> {
  public:
    bool pop(T & ret) {
	RECURSIVE_LOCK(this);
	if (std::stack<T>::empty()) {
		return false;
	}

	ret = std::stack<T>::top();
	std::stack<T>::pop();
	
	return true;
    }

    void push(T x) {
	RECURSIVE_LOCK(this);
	std::stack<T>::push(x);
    }

    int size() {
	return std::stack<T>::size();
    }
	
};

/// A thread-safe multiset.
template <class T>
class ThreadSafeMultiSet : public ThreadSafeRandomAccessContainer<T, std::vector<T> > {
  public:
    void insert(const T & x) {
	RECURSIVE_LOCK(this);
	return std::vector<T>::push_back(x);
    }

    void erase(unsigned int i) {
           RECURSIVE_LOCK(this);
           
           // Swap, drop, and roll...
           unsigned int last = std::vector<T>::size() - 1;
           if (i < last) {
                (*this)[i] = (*this)[last];
           }    
           (*this)[last] = NULL;
           std::vector<T>::pop_back();
    }  
    bool erase(T x) {
	RECURSIVE_LOCK(this);
	unsigned int max = std::vector<T>::size();

	for (unsigned int i = 0; i < max; ++i) {
		if (std::vector<T>::operator[](i) == x) {
			erase(i);
			return true;
		}
	}
	assert(false);
	return false;
    }
};


#include <glib.h>

/// An atomic, thread-safe counter.
class ThreadSafeCounter {
  public: 
    ThreadSafeCounter() : _val(0) {}

    void increment() {
#ifdef SMACQ_CONFIG_THREAD_SAFE 
	g_atomic_int_inc(&_val);
#else
	++_val;
#endif
    }

    /// Atomic decrement; return false iff 0 after decrementing
    gboolean decrement() {
#ifdef SMACQ_CONFIG_THREAD_SAFE 
	return !g_atomic_int_dec_and_test(&_val);
#else
	return(--_val != 0);
#endif
    }
    gint get() {
	return ATOMIC_GET(_val);
    }

  private:
    gint _val;
};


/// A thread-safe boolean value.
class ThreadSafeBoolean {
  public:
    ThreadSafeBoolean() : val(0) {}

    /// Return the current status.
    bool get() {
	return ATOMIC_GET(val);
    }

    /// Atempt to set the boolean to true.  Return false iff already set.
    bool set() {
	bool r = ATOMIC_INT_COMPARE_AND_SET(val, 0, 1);
	assert (ATOMIC_GET(val));
	return r;
    }

    /// Atempt to set the boolean to false.  Return false iff already false.
    bool clear() {
	bool r = ATOMIC_INT_COMPARE_AND_SET(val, 1, 0);
	return r;
    }

  private:
    int val;
};

#endif
