#!/usr/bin/env bash

if ! test -n "${TCK_COMPARE}";
then
    echo 1>&2 "missing variable TCK_COMPARE"
    exit 1
fi

if ! test -n "${DOT_MAX_SIZE}";
then
    DOT_MAX_SIZE=1000
fi

COMMAND="${TCK_COMPARE}"
while test $# != 0;
do
    COMMAND="${COMMAND} $1"
    shift
done

eval ${COMMAND} | sed -e 's/\(^MEMORY_MAX_RSS \).*$/\1 xxxx/g' -e 's/\(^RUNNING_TIME_SECONDS \).*$/\1 xxxx/g'

exit 0
