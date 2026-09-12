# Integralmente Paralelos — Consultora HPC

Solución del **Grupo 6** para el Parcial 1 de CC3069 — Computación Paralela y
Distribuida. Se implementa el **Problema 2: integración numérica mediante una
suma de Riemann**, tanto de forma secuencial como paralela con OpenMP.

## Integrantes

- **Daniel Chet - 231177**
- **Dulce Ambrosio - 231143**

## Solución

Se aproxima el área de una función positiva y computacionalmente exigente en el
intervalo `[0, 10]` usando por defecto `10^9` rectángulos y la regla del punto
medio. La versión paralela distribuye iteraciones independientes con
`schedule(static)` y combina las sumas parciales mediante `reduction(+:suma)`,
sin compartir una variable modificable entre hilos.

```text
.
├── include/       Código común y función a integrar
├── secuencial/    Implementación base
├── paralelo/      Implementación con OpenMP
├── scripts/       Compilación, pruebas y mediciones
└── docs/          Informe, resultados y evidencias
```

## Requisitos

- Windows PowerShell 5.1 o PowerShell 7
- GCC/G++ con soporte para C++20 y OpenMP

En MSYS2, el compilador adecuado es el paquete UCRT64 de GCC. Puede comprobarse
el soporte instalado con:

```powershell
g++ --version
```

## Compilación

Desde la raíz del repositorio:

```powershell
.\scripts\build.ps1
```

Si Windows bloquea la ejecución de scripts por su política local, puede usarse
sin modificarla permanentemente:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
```

Los ejecutables se generan en `build/secuencial.exe` y
`build/paralelo.exe`. La compilación utiliza optimización `-O3` en ambos casos;
la única diferencia relevante es `-fopenmp` para la versión paralela.

## Ejecución

La cantidad solicitada por el problema es de mil millones de rectángulos:

```powershell
.\build\secuencial.exe --rectangles 1000000000
.\build\paralelo.exe --rectangles 1000000000 --threads 8
```

Para una prueba corta:

```powershell
.\build\secuencial.exe --rectangles 1000000
.\build\paralelo.exe --rectangles 1000000 --threads 4
```

Todos los parámetros disponibles pueden consultarse con `--help`. El intervalo
también es configurable mediante `--a` y `--b` (se exige `a >= 0` por el dominio
de la función elegida).

## Verificación y benchmark

La prueba automatizada compara la respuesta secuencial con ejecuciones paralelas
de 1, 2 y 4 hilos:

```powershell
.\scripts\test.ps1
```

Cada integrante debe ejecutar sus propias mediciones en su computadora. Por
ejemplo, si el equipo tiene 8 hilos lógicos:

```powershell
.\scripts\benchmark.ps1 `
  -Integrante "nombre_apellido" `
  -Rectangulos 1000000000 `
  -Repeticiones 5 `
  -Hilos 1,2,4,8
```

Con la política restrictiva, el comando equivalente es:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File .\scripts\benchmark.ps1 `
  -Integrante "nombre_apellido" `
  -Rectangulos 1000000000 `
  -Repeticiones 5 `
  -Hilos "1,2,4,8"
```

El script guarda las corridas crudas y el resumen de *speedup* y eficiencia en
`docs/resultados/`. También imprime el resumen en pantalla para que cada
integrante tome la captura solicitada. No deben comparar tiempos obtenidos en
computadoras diferentes.

Opcionalmente, cada resumen se convierte en una gráfica SVG sin instalar
paquetes de Python adicionales:

```powershell
py .\scripts\graficar_resultados.py `
  .\docs\resultados\nombre_apellido_resumen.csv `
  --output .\docs\graficas\nombre_apellido.svg
```

El diseño, la metodología experimental y las tablas que deben completar están
en [docs/informe.md](docs/informe.md).
