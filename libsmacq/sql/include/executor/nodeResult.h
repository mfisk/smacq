/*-------------------------------------------------------------------------
 *
 * nodeResult.h
 *
 *
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $Id: nodeResult.h,v 1.1 2002/10/30 19:40:35 mfisk Exp $
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODERESULT_H
#define NODERESULT_H

#include "nodes/plannodes.h"

extern TupleTableSlot *ExecResult(Result *node);
extern bool ExecInitResult(Result *node, EState *estate, Plan *parent);
extern int	ExecCountSlotsResult(Result *node);
extern void ExecEndResult(Result *node);
extern void ExecReScanResult(Result *node, ExprContext *exprCtxt, Plan *parent);

#endif   /* NODERESULT_H */
