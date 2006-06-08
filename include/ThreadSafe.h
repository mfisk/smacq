#ifndef CXX_PTHREAD_MUTEX_H
#define CXX_PTHREAD_MUTEX_H

#include <boost/lambda/algorithm.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <pthread.h>

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

#include <map>

template <typename KEY, typename VALUE, typename LT = std::less<KEY> >
class ThreadSafeMap : protected PthreadMutex, private std::map<KEY, VALUE, LT> {
  public:
    /// Return true iff key already exists.  
    bool get(KEY k, VALUE & v) {
	typename std::map<KEY, VALUE, LT>::iterator i;
	RecursiveLock l(this);

 	i = std::map<KEY, VALUE, LT>::find(k);
  	if (i != std::map<KEY, VALUE, LT>::end()) {
		v = i->second;
		return true;
	} else {
		return false;
	}
    }

    VALUE&  operator[](KEY k) {
	RecursiveLock l(this);
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

template <typename T, typename CONTAINER> 
class ThreadSafeContainer : protected PthreadMutex, protected CONTAINER {
  public:
    typedef typename CONTAINER::size_type sizetype;
    ThreadSafeContainer() {}
    ThreadSafeContainer(int x) : CONTAINER(x) {}

    void clear() {
	RecursiveLock l(this);
	CONTAINER::clear();
    }
    typename CONTAINER::size_type size() {
	RecursiveLock l(this);
	return CONTAINER::size();
    }
    bool empty() {
	RecursiveLock l(this);
	return CONTAINER::empty();
    }
	
    CONTAINER snapshot() {
	RecursiveLock l(this);
	return *this;
    }

    void resize(const typename CONTAINER::size_type x) {
	RecursiveLock l(this);
	return CONTAINER::resize(x);
    }
 
    template<class U> 
    void foreach(U cb) {
    	using namespace boost::lambda;

	RecursiveLock l(this);
	if (!empty())
        	for_each(CONTAINER::begin(), CONTAINER::end(), cb);
    };

    template<typename U> 
    typename CONTAINER::iterator findIf(U cb) {
	RecursiveLock l(this);
        return find_if(CONTAINER::begin(), CONTAINER::end(), cb);
    };

    template<class U> 
    bool has_if(U cb) {
	RecursiveLock l(this);
        typename CONTAINER::iterator f = findIf(cb);
	if (f == CONTAINER::end()) {
		return false;
	} else {
		return true;
	}
    };

    template<typename U> 
    bool erase_first_match(U cb) {
	RecursiveLock l(this);
        typename U::iterator f = findIf(CONTAINER::begin(), CONTAINER::end(), cb);
	if (f == CONTAINER::end()) {
		return false;
	} else {
		f.erase();
		return true;
        }
    }
};

template <typename T, typename CONTAINER>
class ThreadSafeRandomAccessContainer : public ThreadSafeContainer<T, CONTAINER> {
   public:
    ThreadSafeRandomAccessContainer() {}
    ThreadSafeRandomAccessContainer(int x) : ThreadSafeContainer<T, CONTAINER >(x) {}

    T& operator[](size_t i) {
	RecursiveLock l(this);
	return CONTAINER::operator[](i);
    }
    void push_back(T x) {
	RecursiveLock l(this);
	return CONTAINER::push_back(x);
    }

};

template <typename T>
class ThreadSafeVector : public ThreadSafeRandomAccessContainer<T, std::vector<T> > {
  public:
    ThreadSafeVector() {}
    ThreadSafeVector(int x) : ThreadSafeRandomAccessContainer<T, std::vector<T> >(x) {}
};

#include <DynamicArray.h>

template <typename T>
class ThreadSafeDynamicArray : public ThreadSafeRandomAccessContainer<T, DynamicArray<T> > {};

#include <stack>
template <class T>
class ThreadSafeStack : private PthreadMutex, private std::stack<T> {
  public:
    bool pop(T & ret) {
	RecursiveLock l(this);
	if (std::stack<T>::empty()) {
		return false;
	}

	ret = std::stack<T>::top();
	std::stack<T>::pop();
	
	return true;
    }

    void push(T x) {
	RecursiveLock l(this);
	std::stack<T>::push(x);
    }

    int size() {
	return std::stack<T>::size();
    }
	
};

template <class T>
class ThreadSafeMultiSet : public ThreadSafeRandomAccessContainer<T, std::vector<T> > {
  public:
    void insert(const T & x) {
	RecursiveLock l(this);
	return std::vector<T>::push_back(x);
    }

    void erase(unsigned int i) {
           RecursiveLock l(this);
           
           // Swap, drop, and roll...
           unsigned int last = std::vector<T>::size() - 1;
           if (i < last) {
                (*this)[i] = (*this)[last];
           }    
           (*this)[last] = NULL;
           std::vector<T>::pop_back();
    }  
    bool erase(T x) {
	RecursiveLock l(this);
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

class ThreadSafeCounter {
  public: 
    ThreadSafeCounter() : _val(0) {}
 
    gint increment() {
	return g_atomic_int_exchange_and_add(&_val, 1) + 1;
    }

    gint decrement() {
	return g_atomic_int_exchange_and_add(&_val, -1) - 1;
    }
    gint get() {
	return g_atomic_int_get(&_val);
    }

  private:
    gint _val;

};

class ThreadSafeBoolean {
  public:
    ThreadSafeBoolean() : val(0) {}

    /// Return the current status.
    bool get() {
	return g_atomic_int_get(&val);
    }

    /// Atempt to set the boolean to true.  Return false iff already set.
    bool set() {
	bool r = g_atomic_int_compare_and_exchange(&val, 0, 1);
	assert (g_atomic_int_get(&val));
	return r;

	//return g_atomic_int_compare_and_exchange(&val, 0, 1);
    }

  private:
    int val;
};

#endif
