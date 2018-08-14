#!/bin/bash

files=$(ls src/*.{c,h})

declare -a patterns=(
    '[a-z] ?\* [a-z]'
    '[^ ]   ?[^ ]'
    'malloc[^.]|.{81,}'
    '.{81,}'
    '[a-z]\(.*\){'
    '\){'
    $'\t'
    '^( {4})* {1,3}[^ ]'
    ',[^ ]'
    ' ,'
    '\S\s+$'
    '\s(if|switch|for|while)\('
    'do\('
)

{
    for (( i=0; i<${#patterns[@]}; i++ )); do
        egrep --color=always -rni "${patterns[$i]}" $files;
    done
} | sort -t':' -k 2,2n | cat

cppcheck --enable=all $files 2>&1 | grep '^\['
