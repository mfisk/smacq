#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "smacq.h"
#include <libgda/libgda.h>

#define BUFSIZE 8192

SMACQ_MODULE(sqlinsert,
  PROTO_CTOR(sqlinsert);
  PROTO_DTOR(sqlinsert);
  PROTO_CONSUME();

  GdaCommand * gda_cmd;  
  GdaClient * gda_client;
  GdaConnection * gda_connection;
  char insert_format[BUFSIZE];

  char ** argv;
  int argc;
  std::vector<DtsField> fields;
  DtsField string_transform;
);

static struct smacq_options options[] = {
  {"d", {string_t:NULL}, "Database to connect to", SMACQ_OPT_TYPE_STRING},
  {"t", {string_t:NULL}, "Table to insert into", SMACQ_OPT_TYPE_STRING},
  {"p", {string_t:"SQLite"}, "Provider name", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

static void print_gda_errors(GdaConnection * conn) {
  GList *list;
  GList *node;
  GdaError *error;
  
  list = (GList *) gda_connection_get_errors (conn);
  
  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
    {
      error = (GdaError *) node->data;
      fprintf (stderr, "Error no.: %ld\t", gda_error_get_number (error));
      fprintf (stderr, "desc: %s\t", gda_error_get_description (error));
      fprintf (stderr, "source: %s\t", gda_error_get_source (error));
      fprintf (stderr, "sqlstate: %s\n", gda_error_get_sqlstate (error));
    }
}


smacq_result sqlinsertModule::consume(DtsObject datum, int & outchan) {
  char values [BUFSIZE] = "";
  char query [BUFSIZE] = "";
  int i, gdares;
  DtsObject field;
  assert(datum);

  for (i = 0; i < argc; i++) {
    if (i)
      strncat(values, ",", BUFSIZE);

    field = datum->getfield(fields[i]);
    if (field) {
      strncat(values, "'", BUFSIZE);
      strncat(values, (char *)field->getdata(), BUFSIZE);
      strncat(values, "'", BUFSIZE);
    } else
      strncat(values,  "NULL", BUFSIZE);
  }
  
  snprintf(query, BUFSIZE, insert_format, values);

  gda_command_set_text(gda_cmd, query);
  gdares = gda_connection_execute_non_query(gda_connection, gda_cmd, NULL);
  
  if (gdares == -1) {
    fprintf(stderr, "Error executing SQL command: %s\n\t", gda_command_get_text(gda_cmd));
    print_gda_errors(gda_connection);
    return SMACQ_ERROR|SMACQ_END;
  }  else {
    return SMACQ_PASS;
  }
}

sqlinsertModule::sqlinsertModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context)
{
  char qbuf[BUFSIZE];
  smacq_opt table_name, database_name, provider_name;
  int i;

  {
    struct smacq_optval optvals[] = {
      {"t", &table_name},
      {"d", &database_name},
      {"p", &provider_name},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    assert(argv);
  }

  assert(table_name.string_t);
  assert(database_name.string_t);
  assert(provider_name.string_t);

  fields.resize(argc);
  string_transform = dts->requirefield("string");

  gda_init("SMACQ-GDA-sqlinsert", "0.1", 0, NULL);

  gda_client = gda_client_new();

  gda_connection = 
    gda_client_open_connection_from_string(gda_client, provider_name.string_t, database_name.string_t, (GdaConnectionOptions)(0));

  assert(gda_connection);

  gda_cmd = gda_command_new("", GDA_COMMAND_TYPE_SQL, 
				   GDA_COMMAND_OPTION_STOP_ON_ERRORS);

  snprintf(insert_format, BUFSIZE, "INSERT INTO %s(", table_name.string_t);
  /* Would use "IF NOT EXISTS", but sqlite doesn't take it */
  snprintf(qbuf, BUFSIZE, "create table %s (", table_name.string_t);
  
  for (i = 0; i < argc; i++) {
    fields[i] = dts->requirefield(dts_fieldname_append(argv[i],"string")); 

    if (i) { /* Not first column */
	strncat(insert_format, ",", BUFSIZE);
    	strncat(qbuf, ",", BUFSIZE);
    }
    strncat(insert_format, argv[i], BUFSIZE);
    strncat(qbuf, argv[i], BUFSIZE);
    strncat(qbuf, " varchar(255) ", BUFSIZE);
  }

  strncat(insert_format, ") VALUES (%s)", BUFSIZE);
  strncat(qbuf, ");", BUFSIZE);

  gda_command_set_text(gda_cmd, qbuf);
  if (-1 == gda_connection_execute_non_query(gda_connection, gda_cmd, NULL)) {
    	fprintf(stderr, "Error executing SQL command: %s\n\t", gda_command_get_text(gda_cmd));
    	print_gda_errors(gda_connection);
	/* return(SMACQ_ERROR|SMACQ_END); */
  }
}

sqlinsertModule::~sqlinsertModule() {
  gda_command_free(gda_cmd);
  gda_client_close_all_connections(gda_client);
  g_object_unref(G_OBJECT(gda_client));
}

