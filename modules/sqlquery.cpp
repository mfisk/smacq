#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq-gda.h>
#include <string>
#include <SmacqModule.h>

SMACQ_MODULE(sqlquery,
  PROTO_CTOR(sqlquery);
  PROTO_DTOR(sqlquery);
  PROTO_PRODUCE();

  GdaConnection * gda_connection;
  GdaDataModel * results, * schema;

  std::string querystr, where;

  std::vector<DtsField> columns;
  int num_rows;
  int num_columns;
  int row_number;

  dts_typeid string_type, empty_type, double_type, int_type, uint32_type, int64_type, uint64_type;

  void processInvariants(int, SmacqGraphNode_ptr);
  void endClause();
  void startClause();
  void addConstant(int column, const char * constant);

);

static struct smacq_options options[] = {
  {"s", {string_t:NULL}, "Datasource (DSN) to connect to (use instead of -d)", SMACQ_OPT_TYPE_STRING},
  {"d", {string_t:NULL}, "Database to connect to", SMACQ_OPT_TYPE_STRING},
  {"dp", {string_t:"SQLite"}, "Database provider name (use with -d)", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
};

inline void sqlqueryModule::startClause() {
	if (where.length()) where += " AND ";
	where += "(";
}

inline void sqlqueryModule::endClause() {
	where += ")";
}

inline void sqlqueryModule::addConstant(int column, const char * constant) {
	// Don't put quotes around numbers
 	const GValue * val = gda_data_model_get_value_at(schema, column, 0, NULL);
	if (gda_value_is_number(val)) {
		where += constant;
	} else {
		where += "'";
		where += constant;
		where += "'";
	}
}

void sqlqueryModule::processInvariants(int column, SmacqGraphNode_ptr g) {
	if (!g) return;

	int const argc = g->getArgc();
	const char ** argv = g->getArgv();

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

	std::vector<ThreadSafeMultiSet<SmacqGraphNode_ptr> > children = g->getChildren();
	if (children[0].size()) 
		processInvariants(column, children[0][0].get());
}

smacq_result sqlqueryModule::produce(DtsObject & datum, int & outchan) {
 if (!results) return SMACQ_END;
 if (row_number >= num_rows) {
	return SMACQ_END;
 }

 //fprintf(stderr, "getting row %d of %d\n", row_number, num_rows);

 datum = dts->construct(empty_type, NULL);

 for (int i = 0; i < num_columns; i++) {
 	const GValue * val = gda_data_model_get_value_at(results, i, row_number, NULL);
   	DtsObject f;

 	//fprintf(stderr, "sqlquery: row %d, column %d is type %d\n", row_number, i, gda_value_get_type(val));

#define GET(smacqtype, ctype, gdatype) { ctype c = g_value_get_##gdatype(val); f = dts->construct(smacqtype, &c); break; }

	switch (G_VALUE_TYPE(val)) {
		case G_TYPE_NONE:	continue;
		case G_TYPE_UCHAR: 	GET(int_type, int, uchar);
#if defined(GLIB_VERSION_2_32)
		case G_TYPE_CHAR: 	GET(int_type, int, schar);
#else
		case G_TYPE_CHAR: 	GET(int_type, int, char);
#endif
		case G_TYPE_UINT: 	GET(uint32_type, uint32_t, uint);
		case G_TYPE_INT: 	GET(int_type, int, int);
		case G_TYPE_UINT64: 	GET(uint64_type, uint64_t, uint64);
		case G_TYPE_INT64: 	GET(int64_type, int64_t, int64);
		case G_TYPE_STRING:	f=dts->construct_fromstring(string_type, g_value_get_string(val)); 
fprintf(stderr, "tringify %s\n",g_value_get_string(val));
					break;

		default:
			char * valstr = gda_value_stringify(val);
  			f = dts->construct_fromstring(string_type, valstr);
			g_free(valstr);
			break;
	}
	datum->attach_field(columns[i], f);
 }
 row_number++;

 return SMACQ_PASS|SMACQ_PRODUCE;
}

sqlqueryModule::sqlqueryModule(struct SmacqModule::smacq_init * context)
  : SmacqModule(context)
{
  smacq_opt cnc, dsn, provider;
  const char ** argv; int argc;

  {
    struct smacq_optval optvals[] = {
      {"s", &dsn},
      {"d", &cnc},
      {"dp", &provider},
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    assert(argc);
  }

#ifdef HAS_GDA_NON_SELECT
  gda_init();
#else
  gda_init("SMACQ-GDA-sqlquery", "0.1", 0, NULL);
#endif

  GdaSqlParser *parser;
  parser = gda_connection_create_parser (gda_connection);

  if (!parser) /* @cnc doe snot provide its own parser => use default one */
     parser = gda_sql_parser_new ();
  /* attach the parser object to the connection */
  g_object_set_data_full (G_OBJECT (gda_connection), "parser", parser, g_object_unref);

  if (dsn.string_t) {
  	gda_connection = 
    		gda_connection_open_from_dsn(dsn.string_t, NULL, GDA_CONNECTION_OPTIONS_READ_ONLY, NULL);
  } else if (cnc.string_t) {
  	assert(provider.string_t);

  	gda_connection = 
    		gda_connection_open_from_string(provider.string_t, cnc.string_t, NULL, GDA_CONNECTION_OPTIONS_READ_ONLY, NULL);
  } else {
	fprintf(stderr, "sqlquery: must specify connection or source option\n");
  }

  assert(gda_connection);

  querystr = "SELECT * FROM ";
  std::string schemastr = querystr;

  // 
  // Query the table for 1 row to get types
  //
  schemastr += argv[0];
  schemastr += " limit 1";
  GError * gerr = NULL;
  GdaStatement *stmt;

  stmt = gda_sql_parser_parse_string (parser, schemastr.c_str(), NULL, NULL);

  schema =  gda_connection_statement_execute_select(gda_connection, stmt, NULL, &gerr);
  if (gerr) {
    	fprintf(stderr, "Error executing SQL command: %s\n\t%s\n", schemastr.c_str(), gerr->message);
	g_error_free(gerr);
  }
  num_rows = gda_data_model_get_n_rows(schema);
  if (num_rows > 0) {
  	num_columns = gda_data_model_get_n_columns(schema);
  	columns.resize(num_columns);

  	for (int i = 0; i < num_columns; i++) {
		columns[i] = dts->requirefield((gchar*)gda_data_model_get_column_title(schema, i));

		SmacqGraphNode_ptr invars = context->self->getChildInvariants(dts, context->scheduler, columns[i]);
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
  
  stmt = gda_sql_parser_parse_string (parser, querystr.c_str(), NULL, NULL);
  gerr = NULL;
  results = gda_connection_statement_execute_select(gda_connection, stmt, NULL, &gerr);
  if (gerr) {
    	fprintf(stderr, "Error executing SQL command: %s\n\t%s\n", querystr.c_str(), gerr->message);
	g_error_free(gerr);
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
  uint64_type = dts->requiretype("uint64");
  int64_type = dts->requiretype("int64");
}

sqlqueryModule::~sqlqueryModule() {
  if (results) g_object_unref(results);
  if (schema) g_object_unref(schema);
}

