#!/bin/bash
source="main_INCREMENTAL.cpp"
executable="./temp/INCREMENTAL.out"

mkdir -p ./temp

echo "Compilando $source..."
g++ -O3 $source -o $executable

if [ $? -ne 0 ]; then
    echo "Error: La compilación falló."
    exit 1
else
    echo "Compilación exitosa."
fi

if [ -x "$executable" ]; then
    echo "Ejecutando $executable..."
    "$executable"
    echo "Ejecución de $executable finalizada."
else
    echo "Error: $executable no es ejecutable o no se encuentra."
    exit 1
fi