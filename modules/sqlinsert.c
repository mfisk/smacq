#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "smacq.h"
#include <libgda/libgda.h>

#define BUFSIZE 8192

struct state {
  GdaCommand * gda_cmd;  
  GdaClient * gda_client;
  GdaConnection * gda_connection;
  char insert_format[BUFSIZE];

  smacq_environment * env;
  char ** argv;
  int argc;
  dts_field * fields;
  dts_field string_transform;
};

static struct smacq_options options[] = {
  {"d", {string_t:NULL}, "Database to connect to", SMACQ_OPT_TYPE_STRING},
  {"t", {string_t:NULL}, "Table to insert into", SMACQ_OPT_TYPE_STRING},
  {"p", {string_t:"SQLite"}, "Provider name", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
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


static smacq_result sqlinsert_consume(struct state * state, const dts_object * datum, int * outchan) {
  char values [BUFSIZE] = "";
  char query [BUFSIZE] = "";
  int i, gdares;
  const dts_object *field;
  assert(datum);

  for (i = 0; i < state->argc; i++) {
    if (i)
      strncat(values, ",", BUFSIZE);

    field = smacq_getfield(state->env, datum, state->fields[i], NULL);
    if (field) {
      strncat(values, "'", BUFSIZE);
      strncat(values, (char *)dts_getdata(field), BUFSIZE);
      strncat(values, "'", BUFSIZE);
    } else
      strncat(values,  "NULL", BUFSIZE);
  }
  
  snprintf(query, BUFSIZE, state->insert_format, values);

  gda_command_set_text(state->gda_cmd, query);
  gdares = gda_connection_execute_non_query(state->gda_connection, state->gda_cmd, NULL);
  
  if (gdares == -1) {
    fprintf(stderr, "Error executing SQL command: %s\n", gda_command_get_text(state->gda_cmd));
    print_gda_errors(state->gda_connection);
    return SMACQ_ERROR|SMACQ_END;
  }  else {
    return SMACQ_PASS;
  }
}

static smacq_result sqlinsert_init(struct smacq_init * context) {
  struct state * state;
  smacq_opt table_name, database_name, provider_name;
  int i;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      {"t", &table_name},
      {"d", &database_name},
      {"p", &provider_name},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
    assert(state->argv);
  }

  assert(table_name.string_t);
  assert(database_name.string_t);
  assert(provider_name.string_t);

  state->fields = malloc(state->argc * sizeof(dts_field));
  state->string_transform = smacq_requirefield(state->env, "string");

  gda_init("SMACQ-GDA-sqlinsert", "0.1", 0, NULL);

  state->gda_client = gda_client_new();

  state->gda_connection = 
    gda_client_open_connection_from_string(state->gda_client, provider_name.string_t, database_name.string_t, 0);

  assert(state->gda_connection);

  state->gda_cmd = gda_command_new("", GDA_COMMAND_TYPE_SQL, 
				   GDA_COMMAND_OPTION_STOP_ON_ERRORS);

  snprintf(state->insert_format, BUFSIZE, "INSERT INTO %s(", table_name.string_t);

  for (i = 0; i < state->argc; i++) {
    state->fields[i] = smacq_requirefield(state->env, dts_fieldname_append(state->argv[i],"string")); 
    if (i) strncat(state->insert_format, ",", BUFSIZE);
    strncat(state->insert_format, state->argv[i], BUFSIZE);
  }

  strncat(state->insert_format, ") VALUES (%s)", BUFSIZE);

  return 0;
}

static smacq_result sqlinsert_shutdown(struct state * state) {
  int i;

  gda_command_free(state->gda_cmd);
  gda_client_close_all_connections(state->gda_client);
  g_object_unref(G_OBJECT(state->gda_client));

  for (i = 0; i < state->argc; i++) 
	  dts_field_free(state->fields[i]);

  free(state->fields);
  free(state);
  return 0;
}

struct smacq_functions smacq_sqlinsert_table = {
  consume: &sqlinsert_consume,
  init: &sqlinsert_init,
  shutdown: &sqlinsert_shutdown,
};

