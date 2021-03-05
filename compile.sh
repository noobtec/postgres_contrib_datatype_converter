#!/bin/bash

echo "" > main.h;
echo "" > entry.h;

for d in datatype/* ; do
    echo "#include \"$d/main.h\"" >> main.h;
done

for d in datatype/* ; do
    echo "#include \"$d/entry.h\"" >> entry.h;
done

gcc -fwhole-program -fPIC -shared libpostgres_contrib_datatype_converter.c -o libpostgres_contrib_datatype_converter.so

#gcc -fwhole-program libpostgres_contrib_datatype_converter.c -o libpostgres_contrib_datatype_converter && ./libpostgres_contrib_datatype_converter
