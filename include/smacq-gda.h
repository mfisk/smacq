#ifndef SMACQ_GDA
#define SMACQ_GDA
#include <libgda/libgda.h>

#ifdef HAS_GDA_NON_SELECT
#include <sql-parser/gda-sql-parser.h>
#endif

static inline gint smacq_gda_execute_non_select(GdaConnection *cnc, const char *qstr, GdaSet *params, GError **error) { 
#if defined(HAS_GDA_NON_SELECT)
  // Use libgda-1.4 style GdaStatement
  GdaStatement * stmt; 

/* create an SQL parser */
  GdaSqlParser *parser;
  parser = gda_connection_create_parser (cnc);

  if (!parser) /* @cnc doe snot provide its own parser => use default one */
     parser = gda_sql_parser_new ();
  /* attach the parser object to the connection */
  g_object_set_data_full (G_OBJECT (cnc), "parser", parser, g_object_unref);

  stmt = gda_sql_parser_parse_string (parser, qstr, NULL, NULL);
  gint res = gda_connection_statement_execute_non_select(cnc, stmt, params, NULL, error); 
#else
  // Wrap a libgda-1.3 style GdaCommand
  GdaCommand * gda_cmd; 
  gda_command_set_text(gda_cmd, qstr); 
  gint res = gda_connection_execute_non_query(cnc, gda_cmd, params, error); 
  gda_command_free(gda_cmd);
#endif
  return res;
}

#ifndef HAS_GDA_NON_SELECT
// Provide 1.4-style wrappers for older versions of libgda

static inline GdaConnection * gda_connection_open_from_string(const gchar *provider_name,
                                                         const gchar *cnc_string,
                                                         const gchar *auth_string,
                                                         GdaConnectionOptions options,
                                                         GError **error) {
    return gda_client_open_connection_from_string(provider_name, cnc_string, NULL, NULL, error);
}

static inline GdaConnection * gda_connection_open_from_string(const gchar *provider_name, const gchar *cnc_string, const gchar *auth_string, GdaConnectionOptions options, GError **error) {
  gda_client = gda_client_new();
  gint r = gda_client_open_connection_from_string(gda_client, provider_name, cnc_sring, options, error);
  return r
}
#endif

#ifdef __gda_error_h__
// Create a libgda-1.3 style compatibility wrapper around the libgda-1.2 API that had its own error type

static inline void smacq_gda_set_error(GdaConnection * conn, GError ** err) {
    GList * errorlist = (GList*) gda_connection_get_errors(conn);
    if (errorlist) {
      errorlist = g_list_first(errorlist);
      assert(errorlist);
      GdaError * error = (GdaError*) errorlist->data;
      assert(error);
      g_set_error(err, 0, gda_error_get_number(error), "%s", gda_error_get_description(error));
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
