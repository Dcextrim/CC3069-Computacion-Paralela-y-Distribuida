# Examen Parcial 1: Consultoría HPC usando OpenMP

Cualquier programador puede escribir un ciclo *for*. Sin embargo, el verdadero valor de un ingeniero en High Performance Computing (HPC) viene en saber cuándo utilizar la paralelización y cuándo no. Muchos incluso aún no conocen sobre este tipo de implementaciones a pesar de existir ya por mucho tiempo. Ya saben que el objetivo de la paralelización no es simplemente "usar más threads", sino comprender el hardware para obtener una mejora real en los tiempos de procesamiento. Algunos algoritmos se pueden dividir de forma natural y sencilla, mientras que otros sufren tanto overhead (gasto de recursos creando y destruyendo threads) que la versión paralela termina siendo más lenta que la secuencial.

Esta actividad se realizará en grupos de 2 personas. Ustedes conforman una firma de consultoría de optimización de software. La primera tarea de su consultora es tomar decisiones estratégicas respecto a algunos problemas a resolver:

## Problema 1: El Histograma Masivo

Tienen un arreglo unidimensional con miles de millones de mediciones de temperatura en punto flotante. Necesitan clasificar estos números en 100 rangos distintos (cubetas) para contar cuántas mediciones caen en cada rango. ¿Cómo reparten el trabajo entre N trabajadores para que el conteo final sea exacto y lo más rápido posible?

## Problema 2: Integración Numérica (Suma de Riemann)

Necesitamos calcular el área bajo una curva muy compleja, f(x), en el intervalo de a hasta b. Para tener precisión absoluta, calcularemos el área de 10^9 rectángulos minúsculos y los sumaremos todos. ¿Cómo distribuyen el cálculo de estos rectángulos y, más importante, cómo gestionan la suma total del área sin estorbarse?

## Problema 3: Multiplicación de Matrices Densas (C = A X B)

Tienen dos matrices cuadradas gigantes, A y B, de un millón por un millón de elementos. Deben calcular la matriz resultante C. Recuerden que para calcular una sola celda de C, necesitan leer toda una fila de A y toda una columna de B. ¿Cómo asignan el trabajo de llenar la matriz C de manera que minimicen la cantidad de veces que los trabajadores leen los mismos datos de las matrices originales?

## Problema 4: Búsqueda de Ruta Mínima (Grafos)

Tienen el mapa de una red social con millones de usuarios (nodos) y sus amistades (aristas). Algunos usuarios tienen 2 amigos, otros tienen 10,000. Quieren encontrar el camino más corto de conexiones entre el Usuario X y el Usuario Y explorando los vecinos paso a paso usando una Cola (Queue) de tareas pendientes. ¿Cómo reparten la exploración de esta red sin que un trabajador termine en un segundo y otro se quede procesando a las 10,000 amistades él solo?

## Problema 5: Filtro de Desenfoque de Imagen (Blur)

Tienen una imagen satelital de 8K representada como una matriz de píxeles. Quieren aplicar un filtro de desenfoque. Para calcular el nuevo color de un píxel, necesitan promediar su valor actual con el de sus 8 vecinos inmediatos. ¿Cómo cortan y reparten esta imagen entre los trabajadores asegurándose de que puedan calcular correctamente los píxeles que quedan exactamente en los bordes de sus recortes?

---

Su primer paso es establecer la infraestructura de trabajo colaborativo. En equipo, discutan brevemente las soluciones para el algoritmo secuencial y consideraciones para convertirlos en paralelo. Basados en esa discusión, deberán elegir exactamente 1 **problema** para implementar en paralelo:

- Grupos 1 al 3 realizarán el problema 1.
- Grupos 4 al 6 realizarán el problema 2.
- Grupos 7 al 9 realizarán el problema 3.
- Grupos 10 al 12 realizarán el problema 4.
- Grupos 13 al 15 realizarán el problema 5.

Creen un único repositorio en GitHub (o lo que usen de preferencia) por grupo.

1. Creen un archivo README.md en la raíz del repositorio. Este archivo debe contener el nombre creativo de su "Consultora HPC" y los nombres completos de los dos integrantes.

2. Cada integrante del equipo debe clonar el repositorio en su máquina y realizar al menos un commit. Deben crear la siguiente estructura de carpetas básica:
   - `/secuencial` (Para el algoritmo base).
   - `/paralelo` (Para la solución optimizada con OpenMP).
   - `/docs` (Para reportes, gráficas y análisis de datos).

La documentación debe contener, como mínimo, los siguientes elementos:

1. **Contexto y Datos:** Descripción del problema elegido junto con la propuesta secuencial y la justificación de los datos de prueba utilizados (tamaño de la muestra, origen, estructuras de datos en memoria).

2. **Estrategia de Paralelización:** Luego de implementar su solución en paralelo ¿Qué directivas de OpenMP utilizaron y por qué? ¿Cómo evitaron las condiciones de carrera (race conditions) o el desbalance de carga (con el scheduling)?

3. **Resultados y Métricas (Requisito Individual):** Esta es la sección más importante. Argumentar cómo sus decisiones realmente mejoraron su algoritmo secuencial. Para garantizar la participación equitativa, el informe debe contener las mediciones de speedup y eficiencia respectivas por cada integrante con pruebas de corridas (screenshot o video de sus ejecuciones).
