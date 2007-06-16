#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <smacq-gda.h>
#include <stdexcept>
#include <SmacqModule.h>
#include <smacq-gda.h>
#define BUFSIZE 8192

SMACQ_MODULE(sqlinsert,
  PROTO_CTOR(sqlinsert);
  PROTO_DTOR(sqlinsert);
  PROTO_CONSUME();

  GdaCommand * gda_cmd;  
  GdaClient * gda_client;
  GdaConnection * gda_connection;
  std::string insert_format;

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

smacq_result sqlinsertModule::consume(DtsObject datum, int & outchan) {
  std::string values;
  char query[BUFSIZE];
  int i; 
  DtsObject field;
  assert(datum);

  for (i = 0; i < argc; i++) {
    if (i)
  	values += ",";

    field = datum->getfield(fields[i]);
    if (field) {
      values += "'";
      values += (char *)field->getdata();
      values += "'";
    } else
      values += "NULL";
  }
  
  snprintf(query, BUFSIZE-1, insert_format.c_str(), values.c_str());

  gda_command_set_text(gda_cmd, query);
 
  GError *gerr = NULL;
  gda_connection_execute_non_query(gda_connection, gda_cmd, NULL, &gerr);
  if (gerr) {
    fprintf(stderr, "Error executing SQL command: %s\n\t%s", gda_command_get_text(gda_cmd), gerr->message);
    g_error_free(gerr);
    return SMACQ_ERROR|SMACQ_END;
  }  else {
    return SMACQ_PASS;
  }
}

sqlinsertModule::sqlinsertModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context)
{
  std::string qbuf;
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

  GError *gerr = NULL;
  gda_connection = 
    gda_client_open_connection_from_string(gda_client, provider_name.string_t, database_name.string_t, (GdaConnectionOptions)(0), &gerr);

  if (gerr) {
    std::string s("Error connecting to database ");
    s += provider_name.string_t;
    s += ", ";
    s += database_name.string_t;
    s += ": ";
    s += gerr->message;
    g_error_free(gerr);
    throw std::runtime_error(s);
  }

  assert(gda_connection);

  gda_cmd = gda_command_new("", GDA_COMMAND_TYPE_SQL, 
				   GDA_COMMAND_OPTION_STOP_ON_ERRORS);

  insert_format = "INSERT INTO ";
  insert_format += table_name.string_t;
  insert_format += "(";

  /* Would use "IF NOT EXISTS", but sqlite doesn't take it */
  qbuf = "CREATE TABLE ";
  qbuf += table_name.string_t;
  qbuf += "(";

  for (i = 0; i < argc; i++) {
    fields[i] = dts->requirefield(dts_fieldname_append(argv[i],"string")); 

    if (i) { /* Not first column */
	insert_format += ",";
	qbuf += ",";
    }
    insert_format += argv[i];
    qbuf += argv[i];
    qbuf += " varchar(255) ";
  }

  insert_format += ") VALUES (%s)";
  qbuf += ");";

  gda_command_set_text(gda_cmd, qbuf.c_str());
  gda_connection_execute_non_query(gda_connection, gda_cmd, NULL, NULL);
}

sqlinsertModule::~sqlinsertModule() {
  gda_command_free(gda_cmd);
  gda_client_close_all_connections(gda_client);
  g_object_unref(G_OBJECT(gda_client));
}

