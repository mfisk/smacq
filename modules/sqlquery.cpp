#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <libgda/libgda.h>
#include <string>
#include <SmacqModule.h>

SMACQ_MODULE(sqlquery,
  PROTO_CTOR(sqlquery);
  PROTO_DTOR(sqlquery);
  PROTO_PRODUCE();

  GdaCommand * gda_cmd;  
  GdaClient * gda_client;
  GdaConnection * gda_connection;
  GdaDataModel * results, * schema;

  std::string querystr, where;

  std::vector<DtsField> columns;
  int num_rows;
  int num_columns;
  int row_number;

  dts_typeid string_type, empty_type, double_type, int_type, uint32_type;

  void processInvariants(int, SmacqGraph_ptr);
  void endClause();
  void startClause();
  void addConstant(int column, char * constant);

);

static struct smacq_options options[] = {
  {"s", {string_t:NULL}, "Datasource to connect to (use instead of -d)", SMACQ_OPT_TYPE_STRING},
  {"d", {string_t:NULL}, "Database to connect to", SMACQ_OPT_TYPE_STRING},
  {"dp", {string_t:"SQLite"}, "Database provider name (use with -d)", SMACQ_OPT_TYPE_STRING},
  {"u", {string_t:NULL}, "Username", SMACQ_OPT_TYPE_STRING},
  {"p", {string_t:NULL}, "Password", SMACQ_OPT_TYPE_STRING},
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

inline void sqlqueryModule::startClause() {
	if (where.length()) where += " AND ";
	where += "(";
}

inline void sqlqueryModule::endClause() {
	where += ")";
}

inline void sqlqueryModule::addConstant(int column, char * constant) {
	// Don't put quotes around numbers
 	GdaValue * val = (GdaValue*)gda_data_model_get_value_at(schema, column, 0);
	if (gda_value_is_number(val)) {
		where += constant;
	} else {
		where += "'";
		where += constant;
		where += "'";
	}
}

void sqlqueryModule::processInvariants(int column, SmacqGraph_ptr g) {
	if (!g) return;

	int const argc = g->getArgc();
	char ** argv = g->getArgv();

	if (!strcmp(argv[0], "equals") && argc == 3) {
		startClause();
		where += argv[1];
		where += "=";
		addConstant(column, argv[2]);
		endClause();
	} else if (!strcmp(argv[0], "filter") && argc == 2) {
		startClause();
		where += argv[1];
		endClause();
	} else {
		fprintf(stderr, "Cannot eagerly perform %s (%d args)\n", argv[0], argc);
	}

	if (g->getChildren()[0].size()) 
		processInvariants(column, g->getChildren()[0][0].get());
}

smacq_result sqlqueryModule::produce(DtsObject & datum, int & outchan) {
 if (!results) return SMACQ_END;
 if (row_number >= num_rows) {
	return SMACQ_END;
 }

 //fprintf(stderr, "getting row %d of %d\n", row_number, num_rows);
 GdaRow * row = (GdaRow*)gda_data_model_get_row(results, row_number++);
 assert(row);
 assert(gda_row_get_length(row) == num_columns);

 datum = dts->construct(empty_type, NULL);

 for (int i = 0; i < num_columns; i++) {
 	GdaValue * val = (GdaValue*)gda_row_get_value(row, i);
   	DtsObject f;

 	//fprintf(stderr, "sqlquery: row %d, column %d is type %d\n", row_number, i, gda_value_get_type(val));

#define GET(smacqtype, ctype, gdatype) { ctype c = gda_value_get_##gdatype(val); f = dts->construct(smacqtype, &c); break; }

	switch (gda_value_get_type(val)) {
		case GDA_VALUE_TYPE_NULL:	continue;
		case GDA_VALUE_TYPE_TINYUINT: 	GET(int_type, int, tinyuint);
		case GDA_VALUE_TYPE_TINYINT: 	GET(int_type, int, tinyint);
		case GDA_VALUE_TYPE_SMALLUINT: 	GET(int_type, int, smalluint);
		case GDA_VALUE_TYPE_SMALLINT: 	GET(int_type, int, smallint);
		case GDA_VALUE_TYPE_UINTEGER: 	GET(uint32_type, uint32_t, uinteger);
		case GDA_VALUE_TYPE_INTEGER: 	GET(int_type, int, integer);
		case GDA_VALUE_TYPE_STRING:	f=dts->construct_fromstring(string_type, gda_value_get_string(val)); break;

		default:
			char * valstr = gda_value_stringify(gda_row_get_value(row, i));
  			f = dts->construct_fromstring(string_type, valstr);
			g_free(valstr);
			break;
	}
	datum->attach_field(columns[i], f);
 }

 return SMACQ_PASS|SMACQ_PRODUCE;
}

sqlqueryModule::sqlqueryModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context)
{
  smacq_opt cnc, source, username, password, provider;
  char ** argv; int argc;

  {
    struct smacq_optval optvals[] = {
      {"u", &username},
      {"p", &password},
      {"s", &source},
      {"d", &cnc},
      {"dp", &provider},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    assert(argc);
  }


  gda_init("SMACQ-GDA-sqlquery", "0.1", 0, NULL);
  gda_client = gda_client_new();
  if (source.string_t) {
  	gda_connection = 
    		gda_client_open_connection(gda_client, source.string_t, username.string_t, password.string_t, GDA_CONNECTION_OPTIONS_READ_ONLY);
  } else if (cnc.string_t) {
  	assert(provider.string_t);
  	gda_connection = 
    		gda_client_open_connection_from_string(gda_client, provider.string_t, cnc.string_t, GDA_CONNECTION_OPTIONS_READ_ONLY);
  } else {
	fprintf(stderr, "sqlquery: must specify connection or source option\n");
  }

  assert(gda_connection);
  gda_cmd = gda_command_new("", GDA_COMMAND_TYPE_SQL, 
				   GDA_COMMAND_OPTION_STOP_ON_ERRORS);

  querystr = "SELECT * FROM ";
  std::string schemastr = querystr;

  // 
  // Query the table for 1 row to get types
  //
  schemastr += argv[0];
  schemastr += " limit 1";
  gda_command_set_text(gda_cmd, schemastr.c_str());
  schema = gda_connection_execute_single_command(gda_connection, gda_cmd, NULL);
  if (schema == NULL) {
    	fprintf(stderr, "Error executing SQL command: %s\n\t", gda_command_get_text(gda_cmd));
    	print_gda_errors(gda_connection);
  }
  num_rows = gda_data_model_get_n_rows(schema);
  if (num_rows > 0) {
  	num_columns = gda_data_model_get_n_columns(schema);
  	columns.resize(num_columns);

  	for (int i = 0; i < num_columns; i++) {
		columns[i] = dts->requirefield((gchar*)gda_data_model_get_column_title(schema, i));

		SmacqGraph_ptr invars = context->self->getChildInvariants(dts, context->scheduler, columns[i]);
		processInvariants(i, invars);
	
		/*	
		fprintf(stderr, "column %d is %s\n", i, gda_data_model_get_column_title(schema, i));
	 	if (invars) {
			fprintf(stderr, "\tthere are filters\n");
		} else {
			fprintf(stderr, "\tno filters\n");
		}
		*/
  	}
  }

  // 
  // Now prepare the real query
  //
  for (int i = 0; i < argc; i++) {
	querystr += argv[i];
	querystr += " ";
  }

  if (where.length()) {
	querystr += " WHERE " + where;
  }
  fprintf(stderr, "sqlquery: %s\n", querystr.c_str());
  
  gda_command_set_text(gda_cmd, querystr.c_str());
  results = gda_connection_execute_single_command(gda_connection, gda_cmd, NULL);
  if (results == NULL) {
    	fprintf(stderr, "Error executing SQL command: %s\n\t", gda_command_get_text(gda_cmd));
    	print_gda_errors(gda_connection);
  }
  num_rows = gda_data_model_get_n_rows(results);

  // 
  // Setup some types we'll need
  //
  empty_type = dts->requiretype("empty");
  string_type = dts->requiretype("string");
  double_type = dts->requiretype("double");
  int_type = dts->requiretype("int");
  uint32_type = dts->requiretype("uint32");
}

sqlqueryModule::~sqlqueryModule() {
  gda_command_free(gda_cmd);
  gda_client_close_all_connections(gda_client);
  if (results) g_object_unref(results);
  if (schema) g_object_unref(schema);
  if (gda_client) g_object_unref(G_OBJECT(gda_client));
}

