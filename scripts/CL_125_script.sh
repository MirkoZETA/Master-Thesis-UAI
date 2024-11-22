#!/bin/bash

# Archivo fuente y ejecutable
source="main_DYNAMIC_CL_125.cpp"
executable="./temp/CL_125.out"

# Número de veces a ejecutar el archivo
iterations=4

# Crear directorio temp si no existe
mkdir -p ./temp

# Compilar el archivo con optimización O3
echo "Compilando $source..."
g++ -O3 $source -o $executable

# Verificar si la compilación fue exitosa
if [ $? -ne 0 ]; then
    echo "Error: La compilación falló."
    exit 1
else
    echo "Compilación exitosa."
fi

# Ejecuta el archivo en secuencia
for ((i=1; i<=iterations; i++)); do
    if [ -x "$executable" ]; then
        echo "Ejecutando $executable - Iteración $i..."
        "$executable"
        echo "Iteración $i de $executable finalizada."
    else
        echo "Error: $executable no es ejecutable o no se encuentra."
        exit 1
    fi
done

echo "El archivo $executable ha sido ejecutado $iterations veces."