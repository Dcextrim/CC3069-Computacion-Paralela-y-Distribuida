#!/usr/bin/env python3
"""Genera una gráfica SVG de speedup y eficiencia sin dependencias externas."""

from __future__ import annotations

import argparse
import csv
import html
from pathlib import Path


ANCHO = 1000
ALTO = 500
COLOR_DATOS = "#2563eb"
COLOR_IDEAL = "#64748b"
COLOR_REJILLA = "#dbe3ee"
COLOR_TEXTO = "#172033"


def leer_resumen(ruta: Path) -> list[dict[str, float | str]]:
    with ruta.open(encoding="utf-8-sig", newline="") as archivo:
        filas = list(csv.DictReader(archivo))
    if not filas:
        raise ValueError(f"El archivo no contiene mediciones: {ruta}")

    requeridas = {"integrante", "hilos", "speedup", "eficiencia_porcentaje"}
    faltantes = requeridas.difference(filas[0])
    if faltantes:
        raise ValueError(f"Faltan columnas en el CSV: {', '.join(sorted(faltantes))}")

    mediciones: list[dict[str, float | str]] = []
    for fila in filas:
        mediciones.append(
            {
                "integrante": fila["integrante"],
                "hilos": float(fila["hilos"]),
                "speedup": float(fila["speedup"]),
                "eficiencia": float(fila["eficiencia_porcentaje"]),
            }
        )
    return sorted(mediciones, key=lambda fila: float(fila["hilos"]))


def coordenada_x(valor: float, maximo: float, x: float, ancho: float) -> float:
    return x + valor / maximo * ancho


def coordenada_y(valor: float, maximo: float, y: float, alto: float) -> float:
    return y + alto - valor / maximo * alto


def dibujar_panel(
    elementos: list[str],
    *,
    x: float,
    y: float,
    ancho: float,
    alto: float,
    titulo: str,
    etiqueta_y: str,
    hilos: list[float],
    valores: list[float],
    maximo_y: float,
    ideal: str,
) -> None:
    maximo_x = max(hilos)
    elementos.append(
        f'<text x="{x + ancho / 2:.1f}" y="{y - 22:.1f}" '
        f'class="titulo-panel">{html.escape(titulo)}</text>'
    )

    for paso in range(6):
        valor_y = maximo_y * paso / 5
        posicion_y = coordenada_y(valor_y, maximo_y, y, alto)
        elementos.append(
            f'<line x1="{x:.1f}" y1="{posicion_y:.1f}" x2="{x + ancho:.1f}" '
            f'y2="{posicion_y:.1f}" class="rejilla"/>'
        )
        elementos.append(
            f'<text x="{x - 10:.1f}" y="{posicion_y + 5:.1f}" '
            f'class="marca-y">{valor_y:.1f}</text>'
        )

    elementos.append(
        f'<line x1="{x:.1f}" y1="{y + alto:.1f}" x2="{x + ancho:.1f}" '
        f'y2="{y + alto:.1f}" class="eje"/>'
    )
    elementos.append(
        f'<line x1="{x:.1f}" y1="{y:.1f}" x2="{x:.1f}" '
        f'y2="{y + alto:.1f}" class="eje"/>'
    )

    for cantidad in hilos:
        posicion_x = coordenada_x(cantidad, maximo_x, x, ancho)
        elementos.append(
            f'<text x="{posicion_x:.1f}" y="{y + alto + 24:.1f}" '
            f'class="marca-x">{cantidad:g}</text>'
        )

    if ideal == "speedup":
        ideal_puntos = [
            (coordenada_x(0.0, maximo_x, x, ancho), coordenada_y(0.0, maximo_y, y, alto)),
            (
                coordenada_x(maximo_x, maximo_x, x, ancho),
                coordenada_y(min(maximo_x, maximo_y), maximo_y, y, alto),
            ),
        ]
    else:
        ideal_puntos = [
            (x, coordenada_y(100.0, maximo_y, y, alto)),
            (x + ancho, coordenada_y(100.0, maximo_y, y, alto)),
        ]
    ideal_svg = " ".join(f"{px:.1f},{py:.1f}" for px, py in ideal_puntos)
    elementos.append(f'<polyline points="{ideal_svg}" class="ideal"/>')

    puntos = [
        (
            coordenada_x(cantidad, maximo_x, x, ancho),
            coordenada_y(valor, maximo_y, y, alto),
        )
        for cantidad, valor in zip(hilos, valores, strict=True)
    ]
    puntos_svg = " ".join(f"{px:.1f},{py:.1f}" for px, py in puntos)
    elementos.append(f'<polyline points="{puntos_svg}" class="datos"/>')
    for (px, py), valor in zip(puntos, valores, strict=True):
        elementos.append(f'<circle cx="{px:.1f}" cy="{py:.1f}" r="5" class="punto"/>')
        elementos.append(
            f'<text x="{px:.1f}" y="{py - 10:.1f}" class="valor">{valor:.2f}</text>'
        )

    elementos.append(
        f'<text x="{x + ancho / 2:.1f}" y="{y + alto + 50:.1f}" '
        f'class="etiqueta">Hilos</text>'
    )
    elementos.append(
        f'<text x="{x - 54:.1f}" y="{y + alto / 2:.1f}" '
        f'class="etiqueta etiqueta-y" transform="rotate(-90 {x - 54:.1f} '
        f'{y + alto / 2:.1f})">{html.escape(etiqueta_y)}</text>'
    )


def crear_svg(mediciones: list[dict[str, float | str]], titulo: str) -> str:
    hilos = [float(fila["hilos"]) for fila in mediciones]
    speedups = [float(fila["speedup"]) for fila in mediciones]
    eficiencias = [float(fila["eficiencia"]) for fila in mediciones]
    maximo_speedup = max(max(hilos), max(speedups)) * 1.1
    maximo_eficiencia = max(110.0, max(eficiencias) * 1.05)

    elementos = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{ANCHO}" height="{ALTO}" '
        f'viewBox="0 0 {ANCHO} {ALTO}" role="img">',
        "<style>",
        f"text {{ fill: {COLOR_TEXTO}; font-family: Arial, sans-serif; }}",
        ".titulo { font-size: 22px; font-weight: 700; text-anchor: middle; }",
        ".titulo-panel { font-size: 17px; font-weight: 600; text-anchor: middle; }",
        ".eje { stroke: #334155; stroke-width: 1.5; }",
        f".rejilla {{ stroke: {COLOR_REJILLA}; stroke-width: 1; }}",
        ".marca-y { font-size: 11px; text-anchor: end; }",
        ".marca-x { font-size: 12px; text-anchor: middle; }",
        ".etiqueta { font-size: 13px; text-anchor: middle; }",
        f".ideal {{ fill: none; stroke: {COLOR_IDEAL}; stroke-width: 2; stroke-dasharray: 7 5; }}",
        f".datos {{ fill: none; stroke: {COLOR_DATOS}; stroke-width: 3; }}",
        f".punto {{ fill: {COLOR_DATOS}; stroke: white; stroke-width: 2; }}",
        ".valor { font-size: 11px; font-weight: 600; text-anchor: middle; }",
        ".leyenda { font-size: 12px; }",
        "</style>",
        f'<rect width="{ANCHO}" height="{ALTO}" fill="white"/>',
        f'<text x="{ANCHO / 2}" y="35" class="titulo">{html.escape(titulo)}</text>',
    ]

    dibujar_panel(
        elementos,
        x=80,
        y=100,
        ancho=370,
        alto=300,
        titulo="Speedup observado",
        etiqueta_y="Speedup",
        hilos=hilos,
        valores=speedups,
        maximo_y=maximo_speedup,
        ideal="speedup",
    )
    dibujar_panel(
        elementos,
        x=580,
        y=100,
        ancho=370,
        alto=300,
        titulo="Eficiencia paralela",
        etiqueta_y="Eficiencia (%)",
        hilos=hilos,
        valores=eficiencias,
        maximo_y=maximo_eficiencia,
        ideal="eficiencia",
    )

    elementos.extend(
        [
            f'<line x1="350" y1="470" x2="385" y2="470" class="datos"/>',
            '<text x="395" y="474" class="leyenda">Medición</text>',
            f'<line x1="510" y1="470" x2="545" y2="470" class="ideal"/>',
            '<text x="555" y="474" class="leyenda">Ideal</text>',
            "</svg>",
        ]
    )
    return "\n".join(elementos)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("resumen", type=Path, help="Archivo *_resumen.csv")
    parser.add_argument("--output", "-o", type=Path, help="Ruta del SVG de salida")
    args = parser.parse_args()

    mediciones = leer_resumen(args.resumen)
    salida = args.output or args.resumen.with_suffix(".svg")
    salida.parent.mkdir(parents=True, exist_ok=True)
    integrante = str(mediciones[0]["integrante"])
    salida.write_text(
        crear_svg(mediciones, f"Resultados de OpenMP — {integrante}"),
        encoding="utf-8",
    )
    print(f"Gráfica creada en {salida}")


if __name__ == "__main__":
    main()
