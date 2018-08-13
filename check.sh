#!/bin/bash

files=$(ls src/*.{c,h})

{
    egrep --color=always -rni '[a-z] ?\* [a-z]' $files;
    egrep --color=always -rni '[^ ]   ?[^ ]' $files;
    egrep --color=always -rni 'malloc[^.]|.{81,}' $files;
    egrep --color=always -rni '.{81,}' $files;
    egrep --color=always -rni '[a-z]\(.*\){' $files;
    egrep --color=always -rni '\){' $files;
    egrep --color=always -rni $'\t' $files;
    egrep --color=always -rni '^( {4})* {1,3}[^ ]' $files;
    egrep --color=always -rni ',[^ ]' $files;
    egrep --color=always -rni ' ,' $files;
} | sort | cat
cppcheck --enable=all $files 2>&1 | grep '^\['
