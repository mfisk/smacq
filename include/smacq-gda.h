#ifndef SMACQ_GDA
#define SMACQ_GDA
#include <libgda/libgda.h>

#ifdef __gda_error_h__
// Create a libgda-1.3 style compatibility wrapper around the libgda-1.2 API that had its own error type

static inline void smacq_gda_set_error(GdaConnection * conn, GError ** err) {
    GList * errorlist = (GList*) gda_connection_get_errors(conn);
    if (errorlist) {
      errorlist = g_list_first(errorlist);
      assert(errorlist);
      GdaError * error = (GdaError*) errorlist->data;
      assert(error);
      g_set_error(err, 0, gda_error_get_number(error), gda_error_get_description(error));
      if (g_list_next(errorlist)) {
	fprintf(stderr, "Warning: additional GDA errors not displayed\n");
      }
    } else {
	fprintf(stderr, "Unknown GDA error\n");
    }
}

static inline GdaConnection* gda_client_open_connection(GdaClient* client , const gchar* dsn, const gchar* username, const gchar* password, GdaConnectionOptions options, GError ** err) {
  GdaConnection * gdares = gda_client_open_connection(client, dsn, username, password, options);
  return gdares;
}
static inline GdaConnection* gda_client_open_connection_from_string(GdaClient *client, const gchar *provider_id, const gchar *cnc_string, GdaConnectionOptions options, GError** err) {
  GdaConnection * gdares =  gda_client_open_connection_from_string(client, provider_id, cnc_string, options);
  return gdares;
}

static inline gint gda_connection_execute_non_query(GdaConnection* conn, GdaCommand* cmd, GdaParameterList* params, GError ** err) {
  gint gdares = gda_connection_execute_non_query(conn, cmd, params);
  if (gdares == -1) {
	smacq_gda_set_error(conn, err);
	return false;
  } else {
	return true;
  }
}

static inline GdaDataModel * gda_connection_execute_single_command(GdaConnection* conn, GdaCommand* cmd, GdaParameterList* params, GError ** err) {
  GdaDataModel * gdares = gda_connection_execute_single_command(conn, cmd, params);
  if (! gdares) {
	smacq_gda_set_error(conn, err);
  }
  return gdares;
}

#endif

#endif
