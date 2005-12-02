/**
 * GASNet.h
 * by Bill Weiss, Mike Fisk
 *
 * A C++ wrapper to GASNet, to make it less hard to use
 *
 *
 */
#ifndef SMACQ_GASNET_H
#define SMACQ_GASNET_H

#include <gasnet.h>
// Gasnet defines these and that causes problems with libgnu:
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

/// Avoid unused reference for gasneti_threadkey_init
inline void gasnetbug() {
	gasneti_threadkey_init(0);
}

#include <StackException.h>
#include <cstdarg>
#include <iostream>
#include <map>

/** \class GASNet
 * \brief GASNet wrapper class singleton
 *
 * At program init, create an instance of GASNet with pointers to argc and
 * argv.
 * 
 * To use this, make a new instance like this:
 * GASNet * foo = GASNet::Instance();
 * This will ensure that there is only one of these at any point.
 *
 * for now, all GASNet errors are fatal.  This may change to exceptions or
 * something more useful.
 *
 * This class is not threadsafe until attach() is called.
 */
class GASNet
{
	typedef void (*handler_t)();
	typedef std::map<handler_t, int> handler_map;

	protected:
	/** \var map<const void *, int> pre_handlers
	 *  \brief map where handlers are held before attach
	 */
		handler_map pre_handlers;

	/** \var gasnet_handlerentry_t * handlers
	 *  \brief table of handlers for gasnet_attach
	 */
		gasnet_handlerentry_t * handlers;

		/// Exception class
		class Exception : public StackException {
			public:
			Exception(char * msg, int retVal) : StackException(msg) {
				this->message += ": gasnet error ";
				this->message += gasnet_ErrorName(retVal);
				this->message += ": ";
				this->message += gasnet_ErrorDesc(retVal);
			}
		};

	public:
		void registerHandler(handler_t, const int ref = 0);
		void attach(int *, char ***);
		void RequestMedium(gasnet_node_t, gasnet_handler_t, void * source_addr, size_t nbytes, int nargs, ...);
		void RequestShort(gasnet_node_t, gasnet_handler_t, int nargs, ...);
		void ReplyShort(gasnet_token_t, gasnet_handler_t, int nargs, ...);

		void testException(char * msg, int retval) {
			if (retval != GASNET_OK) {
				throw Exception(msg, retval);
			}
		}
			
};

/** \brief add a handler to GASNet's internal list
 *
 * @param handler function pointer
 * @param ref integer which will be used to refer to this function
 */
inline void GASNet::registerHandler(handler_t handler, const int ref)
{
	pre_handlers[handler] = ref;
}

/** \brief One-time initialization and attach to cluster
 *
 * @param argc pointer to main's argc.  May modify argc!
 * @param argv pointer to main's argv.  May modify argv!
 *
 * Call gasnet_attach()
 *
 * Hopefully, we will be thread-safe after this
 */
inline void GASNet::attach(int * argc, char *** argv)
{
	testException("gasnet_init", gasnet_init(argc, argv));

	//		First, we need an array of handlers for gasnet_attach()

	//	allocate some space
	this->handlers = (gasnet_handlerentry_t *) calloc(sizeof(gasnet_handlerentry_t), pre_handlers.size());
	
	//	fill in array entries
	int i = 0;
	for(
			handler_map::iterator cur = pre_handlers.begin();
			cur != pre_handlers.end();
			++cur, ++i
	   ) 
        {
		this->handlers[i].fnptr = cur->first;
		this->handlers[i].index = cur->second;
 	}

	testException("gasnet_attach", gasnet_attach(this->handlers, pre_handlers.size(), 0, 0));
}

inline void GASNet::RequestMedium(gasnet_node_t dest, gasnet_handler_t handler, void * source_addr, size_t nbytes, int nargs, ...) {
        va_list args;	
	va_start(args, nargs);

	testException("gasnet_AMRequestMediumM", gasnetc_AMRequestMediumM(dest, handler, source_addr, nbytes, nargs, args));
}

inline void GASNet::ReplyShort(gasnet_token_t token, gasnet_handler_t handler, int nargs, ...) {
	va_list args;
	va_start(args, nargs);

	testException("gasnet_AMRequestShortM", gasnetc_AMReplyShortM(token, handler, nargs, args));
}

inline void GASNet::RequestShort(gasnet_node_t dest, gasnet_handler_t handler, int nargs, ...) {
	va_list args;
	va_start(args, nargs);

	testException("gasnet_AMRequestShortM", gasnetc_AMRequestShortM(dest, handler, nargs, args));
}
#endif
