#!/bin/bash

OUTPUT="salon"
FILES="fryzjer.c funkcje.c klient.c main.c kierownik.c"
LIBS="-lpthread"

gcc -o $OUTPUT $FILES $LIBS

if [ $? -eq 0 ]; then
    echo "$OUTPUT"
else
    echo "blad"
    exit 1
fi
