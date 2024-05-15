#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

function usage() {
    echo "Usage: $0 no_t";
    echo "       t number of transitions";
}

if [ $# -eq 1 ]; then
    no_t=$1
else
    usage
    exit 1
fi

# Model

echo "system:parallel_${no_t}
"

# Events

echo "event:s
"

# Process

echo "# Process eval_non_determ_${no_t}
process:P_eval_non_determ_${no_t}
clock:1:x
clock:1:y
location:P_eval_non_determ_${no_t}:A{initial:}
location:P_eval_non_determ_${no_t}:B{}
location:P_eval_non_determ_${no_t}:C{}
edge:P_eval_non_determ_${no_t}:B:C:s{provided:x<=1}"

for trans in `seq 1 ${no_t}`; do
echo "edge:P_eval_non_determ_${no_t}:A:B:s{provided:y<=${trans}:do:x=0}"
done
