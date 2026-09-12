# Informe técnico — Integración numérica con OpenMP

## Datos del equipo

- **Consultora:** Integralmente Paralelos
- **Grupo:** 6
- **Integrante 1:** [Nombre completo]
- **Integrante 2:** [Nombre completo]

## 1. Contexto y datos

### Problema elegido

Se debe aproximar el área bajo una curva compleja `f(x)` en el intervalo
`[a, b]` mediante una suma de Riemann de `n = 10^9` rectángulos. El problema es
apropiado para paralelismo de memoria compartida porque evaluar cada rectángulo
es independiente; la única dependencia aparece al acumular el área total.

La función usada es:

```text
f(x) = exp(-x/10) · (sin²(x) + cos²(2x) + sqrt(x+1) + ln(x+1))
       + 1/(1+x²)
```

Se utiliza el intervalo `[0, 10]`. En él, la función es continua, está definida y
es positiva, por lo que el resultado representa un área geométrica. Combina
funciones trigonométricas, exponencial, raíz y logaritmo; así, cada iteración
tiene suficiente trabajo computacional para que el costo de crear y coordinar
hilos no domine cuando `n` es grande.

### Representación y datos de prueba

No es necesario almacenar mil millones de valores. Para el rectángulo `i` se
calcula bajo demanda el punto medio:

```text
h   = (b - a) / n
x_i = a + (i + 0.5) · h
área ≈ h · Σ f(x_i), para i = 0, ..., n-1
```

Se eligió la regla del punto medio porque normalmente tiene menor error que usar
el extremo izquierdo o derecho con la misma cantidad de rectángulos. Los índices
se representan con enteros de 64 bits, necesarios porque `10^9` debe ser válido
en todas las plataformas. Los cálculos se realizan con `double`. La memoria
adicional de la versión secuencial es `O(1)` y la implementación no depende de
archivos ni de datos aleatorios, de modo que todas las corridas son reproducibles.

Para desarrollo se usan `2 × 10^5` rectángulos en las pruebas rápidas. La
medición final usa exactamente `10^9`, como indica el enunciado.

### Propuesta secuencial

El algoritmo base recorre todos los índices, evalúa `f(x_i)`, acumula la suma y
al final multiplica por el ancho `h`. Su complejidad temporal es `O(n)` y su
memoria adicional es `O(1)`. El cronómetro abarca únicamente el ciclo de
integración, no el análisis de argumentos ni la impresión del resultado.

## 2. Estrategia de paralelización

La versión paralela utiliza estas directivas y decisiones:

- `#pragma omp parallel`: crea el equipo de hilos una sola vez.
- `#pragma omp for schedule(static)`: reparte bloques de iteraciones de forma
  determinista. Todas las evaluaciones cuestan prácticamente lo mismo, por lo
  que un reparto estático ofrece buen balance y menos *overhead* que `dynamic`.
- `reduction(+:suma)`: OpenMP proporciona a cada hilo una suma privada y combina
  las sumas parciales al terminar. Esto elimina la condición de carrera que
  ocurriría con varios hilos ejecutando `suma += ...` sobre la misma variable.
- `default(none)`: obliga a declarar de manera explícita cómo se comparte cada
  variable y ayuda a detectar errores de alcance durante la compilación.
- `omp_set_dynamic(0)`: impide que el *runtime* cambie silenciosamente la
  cantidad solicitada de hilos, algo importante para experimentos repetibles.
- `omp_get_wtime()`: mide el tiempo de pared de la región paralela.

No se usa `critical` ni una operación `atomic` dentro del ciclo: cualquiera de
ellas serializaría mil millones de actualizaciones y destruiría la escalabilidad.
La reducción solo sincroniza la combinación final. Para `p` hilos, el trabajo
ideal por hilo es aproximadamente `n/p`; el tiempo computacional esperado es
`O(n/p)` más el costo de crear el equipo y reducir `p` sumas parciales.

La suma de números en punto flotante no es asociativa. Por ello, el último bit
puede cambiar entre cantidades de hilos aunque ambos algoritmos sean correctos.
La prueba automática y el benchmark reportan el error relativo frente a la
versión secuencial en vez de exigir igualdad bit a bit.

## 3. Metodología experimental

Cada integrante debe medir en su propia computadora:

1. Compilar ambas versiones una sola vez con `-O3 -march=native`; solo la versión
   paralela lleva `-fopenmp`.
2. Conectar el equipo a corriente, usar el mismo perfil de energía y cerrar
   aplicaciones que produzcan carga considerable.
3. Ejecutar `10^9` rectángulos con 1, 2, 4, ... hilos, sin superar los hilos
   lógicos disponibles.
4. Realizar al menos cinco repeticiones. Se usa la mediana, que es menos sensible
   a interrupciones ocasionales del sistema operativo.
5. Comparar cada tiempo paralelo contra la versión secuencial de **esa misma
   computadora**.

Comando recomendado:

```powershell
.\scripts\benchmark.ps1 `
  -Integrante "nombre_apellido" `
  -Rectangulos 1000000000 `
  -Repeticiones 5 `
  -Hilos 1,2,4,8
```

Las métricas son:

```text
speedup(p)    = T_secuencial / T_paralelo(p)
eficiencia(p) = speedup(p) / p · 100 %
```

Después de medir, se genera una gráfica de *speedup* y eficiencia con:

```powershell
py .\scripts\graficar_resultados.py `
  .\docs\resultados\nombre_apellido_resumen.csv `
  --output .\docs\graficas\nombre_apellido.svg
```

El *speedup* indica cuántas veces es más rápida la solución paralela. La
eficiencia indica qué fracción de la capacidad teórica de los `p` hilos se está
aprovechando.

### Entorno del integrante 1

- **Nombre:** [Completar]
- **CPU:** [Modelo]
- **Núcleos físicos / hilos lógicos:** [Completar]
- **RAM:** [Completar]
- **Sistema operativo:** [Completar]
- **Compilador y versión (`g++ --version`):** [Completar]

| Hilos | Mediana secuencial (s) | Mediana paralela (s) | Speedup | Eficiencia (%) | Error relativo máx. |
|------:|-----------------------:|---------------------:|--------:|---------------:|--------------------:|
| 1 | [CSV] | [CSV] | [CSV] | [CSV] | [CSV] |
| 2 | [CSV] | [CSV] | [CSV] | [CSV] | [CSV] |
| 4 | [CSV] | [CSV] | [CSV] | [CSV] | [CSV] |
| 8 | [si aplica] | [si aplica] | [si aplica] | [si aplica] | [si aplica] |

**Evidencia:** agregar la captura en `docs/evidencias/integrante_1.png` y
la gráfica en `docs/graficas/integrante_1.svg`; luego reemplazar estas rutas:

```markdown
![Corridas del integrante 1](evidencias/integrante_1.png)
![Speedup y eficiencia del integrante 1](graficas/integrante_1.svg)
```

### Entorno del integrante 2

- **Nombre:** [Completar]
- **CPU:** [Modelo]
- **Núcleos físicos / hilos lógicos:** [Completar]
- **RAM:** [Completar]
- **Sistema operativo:** [Completar]
- **Compilador y versión (`g++ --version`):** [Completar]

| Hilos | Mediana secuencial (s) | Mediana paralela (s) | Speedup | Eficiencia (%) | Error relativo máx. |
|------:|-----------------------:|---------------------:|--------:|---------------:|--------------------:|
| 1 | [CSV] | [CSV] | [CSV] | [CSV] | [CSV] |
| 2 | [CSV] | [CSV] | [CSV] | [CSV] | [CSV] |
| 4 | [CSV] | [CSV] | [CSV] | [CSV] | [CSV] |
| 8 | [si aplica] | [si aplica] | [si aplica] | [si aplica] | [si aplica] |

**Evidencia:** agregar la captura en `docs/evidencias/integrante_2.png` y
la gráfica en `docs/graficas/integrante_2.svg`; luego reemplazar estas rutas:

```markdown
![Corridas del integrante 2](evidencias/integrante_2.png)
![Speedup y eficiencia del integrante 2](graficas/integrante_2.svg)
```

## 4. Análisis de resultados

> Completar después de generar ambos CSV. No afirmar que duplicar hilos duplica
> el rendimiento: argumentar usando las mediciones reales.

La configuración con mejor tiempo para el integrante 1 fue **[p] hilos**, con un
*speedup* de **[S]** y eficiencia de **[E] %**. Para el integrante 2 fue **[p]
hilos**, con un *speedup* de **[S]** y eficiencia de **[E] %**.

La eficiencia normalmente disminuye al aumentar los hilos debido al costo fijo
de crear y coordinar el equipo, la reducción final, la competencia por recursos
del procesador y la frecuencia dinámica del CPU. Si se usan más hilos que núcleos
físicos, el *hardware multithreading* comparte unidades de ejecución y no ofrece
el mismo beneficio que un núcleo adicional. Deben contrastarse estas causas con
la forma observada en las tablas de cada computadora.

La versión paralela con un solo hilo permite cuantificar el *overhead* de OpenMP:
**[comparar `T_paralelo(1)` con `T_secuencial`]**. Para cargas pequeñas ese costo
puede hacer que la versión secuencial sea más rápida; con `10^9` rectángulos el
trabajo útil es suficientemente grande para amortizarlo.

## 5. Conclusiones

- La independencia entre rectángulos permite repartir el ciclo sin comunicación
  durante el cálculo; la reducción resuelve la única dependencia global.
- `schedule(static)` corresponde a una carga uniforme y evita el costo extra del
  reparto dinámico.
- **[Agregar una conclusión cuantitativa basada en el mejor speedup medido.]**
- **[Comparar las dos computadoras y explicar por qué sus resultados difieren.]**

## 6. Reproducibilidad y participación

Los archivos `*_corridas.csv` conservan todas las observaciones y los archivos
`*_resumen.csv` contienen las medianas y métricas calculadas. Ambos deben
versionarse junto con las capturas. Además, el historial de Git debe mostrar al
menos un commit de cada integrante, de acuerdo con el requisito de la actividad.
