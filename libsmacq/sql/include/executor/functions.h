/*-------------------------------------------------------------------------
 *
 * functions.h
 *		Declarations for execution of SQL-language functions.
 *
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $Id: functions.h,v 1.1 2002/10/30 19:40:35 mfisk Exp $
 *
 *-------------------------------------------------------------------------
 */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "fmgr.h"

extern Datum fmgr_sql(PG_FUNCTION_ARGS);

#endif   /* FUNCTIONS_H */
