# Contador de frecuencia de palabras

El proyecto incluye dos implementaciones que aplican exactamente las mismas
reglas al texto: los signos `.,;:!?\"'()[]{}-_` se sustituyen por espacios y
las palabras conservan sus mayusculas y minusculas.

- `contador_secuencial.cpp`: realiza el conteo con un solo flujo de ejecucion.
- `contador_paralelo.cpp`: divide la lista en bloques contiguos y usa, como
  minimo, dos hilos. Cada hilo mantiene un diccionario local y el hilo principal
  combina los resultados despues de esperar a todos con `join()`.

## Compilar

Con un compilador compatible con C++17:

```powershell
$mingwBin = Split-Path (Get-Command g++).Source
$env:Path = "$mingwBin;$env:Path"
g++ -std=c++17 -O2 contador_secuencial.cpp -o contador_secuencial.exe
g++ -std=c++17 -O2 contador_paralelo.cpp -o contador_paralelo.exe
```

Las primeras dos lineas aseguran que Windows use las bibliotecas del mismo
MinGW con el que se compilaron los programas.

## Ejecutar

Sin argumentos, ambos programas leen `texto.txt`:

```powershell
.\contador_secuencial.exe
.\contador_paralelo.exe
```

Tambien se puede indicar el archivo y, en la version paralela, la cantidad de
hilos (debe ser al menos 2):

```powershell
.\contador_secuencial.exe texto.txt
.\contador_paralelo.exe texto.txt 4
```

## Comprobar que los resultados coinciden

```powershell
.\contador_secuencial.exe texto.txt > salida_secuencial.txt
.\contador_paralelo.exe texto.txt 4 > salida_paralela.txt
Compare-Object (Get-Content salida_secuencial.txt) (Get-Content salida_paralela.txt)
```

Si `Compare-Object` no imprime nada, ambas versiones produjeron exactamente las
mismas palabras y frecuencias.

## Video

[Corto2](https://canva.link/corto-2)
