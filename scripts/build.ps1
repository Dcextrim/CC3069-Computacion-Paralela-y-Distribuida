param(
    [string]$Compilador = "g++"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$raiz = Split-Path -Parent $PSScriptRoot
$directorioBuild = Join-Path $raiz "build"
New-Item -ItemType Directory -Force -Path $directorioBuild | Out-Null

$opcionesComunes = @(
    "-std=c++20",
    "-O3",
    "-march=native",
    "-Wall",
    "-Wextra",
    "-Wpedantic"
)

Write-Host "Compilando version secuencial..."
& $Compilador @opcionesComunes `
    (Join-Path $raiz "secuencial/main.cpp") `
    "-o" (Join-Path $directorioBuild "secuencial.exe")
if ($LASTEXITCODE -ne 0) {
    throw "Fallo la compilacion de la version secuencial."
}

Write-Host "Compilando version paralela con OpenMP..."
& $Compilador @opcionesComunes "-fopenmp" `
    (Join-Path $raiz "paralelo/main.cpp") `
    "-o" (Join-Path $directorioBuild "paralelo.exe")
if ($LASTEXITCODE -ne 0) {
    throw "Fallo la compilacion de la version paralela."
}

Write-Host "Ejecutables creados en $directorioBuild"
