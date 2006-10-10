#ifndef SMACQ_GDA
#define SMACQ_GDA
#include <libgda/libgda.h>


#ifdef __gda_error_h__
// Create a libgda-1.3 style compatibility wrapper around the libgda-1.2 API that had its own error type
static inline gint gda_connection_execute_non_query(GdaConnection* conn, GdaCommand* cmd, GdaParameterList* params, GError ** err) {
  gint gdares = gda_connection_execute_non_query(conn, cmd, params);
  if (gdares == -1) {
    GList * errorlist = gda_connection_get_errors(conn);
    assert(errorlist);
    errorlist = g_list_first(errorlist);
    assert(errorlist);
    GdaError * error = errorlist->data;
    assert(error);
    g_set_error(err, 0, gda_error_get_number(error), gda_error_get_description(error));
    if (g_list_next(errorlist)) {
	fprintf(stderr, "Warning: additional GDA errors not displayed\n");
    }
    return false;
  } else {
    return true;
  }
}
#endif

#endif
