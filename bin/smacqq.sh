#!/bin/sh

RUNDIR=`echo $0 | sed 's/\/[^/]*$//'`
LD_LIBRARY_PATH=${RUNDIR}/:${LD_LIBRARY_PATH} ${RUNDIR}/bin/smacqq "$@"
