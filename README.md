# Hoja de Trabajo 1 - Conteo de palabras

Las dos versiones miden unicamente la fase de conteo: el texto ya esta leido y
tokenizado cuando inicia el cronometro, y la impresion queda fuera de la
medicion. En la version paralela el tiempo incluye crear y unir los hilos y
combinar los mapas locales, porque esas operaciones son overhead del enfoque.

## Compilacion (MinGW-w64)

```powershell
g++ -O2 -std=c++17 -static -static-libgcc -static-libstdc++ contador_secuencial.cpp -o contador_secuencial.exe
g++ -O2 -std=c++17 -pthread -static -static-libgcc -static-libstdc++ contador_paralelo.cpp -o contador_paralelo.exe
g++ -O2 -std=c++17 -static -static-libgcc -static-libstdc++ generar_texto_benchmark.cpp -o generar_texto_benchmark.exe
```

## Carga y mediciones reproducibles

El texto original es demasiado pequeno para medir de forma estable. El
generador crea un archivo grande repitiendo exactamente su contenido; las tres
configuraciones procesan ese mismo archivo.

```powershell
.\generar_texto_benchmark.exe texto.txt texto_benchmark.txt 500000
.\medir_rendimiento.ps1 -Archivo texto_benchmark.txt -Repeticiones 5
```

El script realiza una corrida de calentamiento no registrada, rota el orden de
las configuraciones y guarda los tiempos, speedup y efficiency en
`resultados_medicion.csv`.
