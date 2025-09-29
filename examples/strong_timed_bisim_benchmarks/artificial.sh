#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

usage() {
    echo "Usage: $0 <width> <depth (max 26)> <no_of_clocks> [-c]"
    echo "  -c    mutate the last transition"
}

# Need 3 or 4 arguments
if [[ $# -lt 3 || $# -gt 4 ]]; then
    usage
    exit 1
fi

width=$1
depth=$2
clk=$3
mutate=false

if [[ $# -eq 4 ]]; then
    if [[ $4 == "-c" ]]; then
        mutate=true
    else
        echo "Unknown option: $4"
        usage
        exit 1
    fi
fi

if [ $depth -ge 27 ]; then
    usage
    exit 1
fi
# Model

echo "system:artificial_system_${width}_${depth}_${clk}
"

# Events

char="a"

for ((i=0; i < depth-1; i++)); do
    for((j=0; j < width**i; j++)); do
#        for((k=0; k < width; k++)); do
#          echo "event:${char}_${j}_${k}"
#        done
    echo "event:${char}_${j}"
    done

    # increment char
    ord=$(printf "%d" "'$char")
    ((ord++))
    char=$(printf \\$(printf "%03o" "$ord"))
done

echo ""

# Process and clocks

echo "process:artificial_process_${width}_${depth}_${clk}

clock:${clk}:x
"

# locations

echo "location:artificial_process_${width}_${depth}_${clk}:loc_A_0{initial:}"
char="B"

for ((i=1; i < depth; i++)); do
    for((j=0; j < width**(i); j++)); do
        echo "location:artificial_process_${width}_${depth}_${clk}:loc_${char}_${j}"
    done

    # increment char
    ord=$(printf "%d" "'$char")
    ((ord++))
    char=$(printf \\$(printf "%03o" "$ord"))
done

echo ""

char=A

# edges
for ((i=1; i < depth; i++)); do
    # increment char
    ord=$(printf "%d" "'$char")
    ((ord++))
    new_char=$(printf \\$(printf "%03o" "$ord"))
    for ((j=0; j < width**(i-1); j++)); do
        random=$(( RANDOM % clk ))
        target=$(( ${j} * ${width} ))
        low=${char,,}
        for ((k=0; k < width; k++)); do          
            if (( depth - 1 == i && (width**(i-1)) - 1 == j && width-1 == k )) && [[ true == $mutate ]]; then
                iplusone=$(( i+1 ))
                echo "edge:artificial_process_${width}_${depth}_${clk}:loc_${char}_${j}:loc_${new_char}_${target}:${low}_${j}{provided:x[${random}]<=${iplusone}}"
            else
                echo "edge:artificial_process_${width}_${depth}_${clk}:loc_${char}_${j}:loc_${new_char}_${target}:${low}_${j}{provided:x[${random}]<=${i}}"
                target=$(( target + 1 ))
            fi
        done  
    done
    char=${new_char}
done
