#!/bin/sh

RUNDIR=`echo $0 | sed 's/\/[^/]*$//'`
CMD=`basename $0`
LD_LIBRARY_PATH=${RUNDIR}/.lib:${LD_LIBRARY_PATH} exec -a "$0" ${RUNDIR}/.bin/${CMD} "$@"
#LD_LIBRARY_PATH=${RUNDIR}/.lib:${LD_LIBRARY_PATH} ${RUNDIR}/.bin/${CMD} "$@"
