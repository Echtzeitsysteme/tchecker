#!/usr/bin/env bash

if ! test -n "${TCK_SIMULATE}";
then
    echo 1>&2 "missing variable TCK_SIMULATE"
    exit 1
fi

COMMAND="${TCK_SIMULATE}"
while test $# != 0;
do
    COMMAND="${COMMAND} $1"
    shift
done

eval ${COMMAND}

exit 0
