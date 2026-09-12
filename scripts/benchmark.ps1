param(
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [string]$Integrante,

    [ValidateRange(1, [long]::MaxValue)]
    [long]$Rectangulos = 1000000000,

    [ValidateRange(1, 30)]
    [int]$Repeticiones = 5,

    [ValidateNotNullOrEmpty()]
    [string[]]$Hilos = @("1", "2", "4")
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$cantidadesHilos = @(
    $Hilos |
        ForEach-Object { $_ -split ',' } |
        ForEach-Object {
            $cantidad = 0
            if (-not [int]::TryParse($_.Trim(), [ref]$cantidad) -or $cantidad -le 0) {
                throw "Cantidad de hilos invalida: $_"
            }
            $cantidad
        } |
        Sort-Object -Unique
)

$raiz = Split-Path -Parent $PSScriptRoot
$secuencial = Join-Path $raiz "build/secuencial.exe"
$paralelo = Join-Path $raiz "build/paralelo.exe"
$directorioResultados = Join-Path $raiz "docs/resultados"
New-Item -ItemType Directory -Force -Path $directorioResultados | Out-Null

if (-not (Test-Path -LiteralPath $secuencial) -or
    -not (Test-Path -LiteralPath $paralelo)) {
    & (Join-Path $PSScriptRoot "build.ps1")
}

$nombreSeguro = $Integrante -replace '[^a-zA-Z0-9_-]', '_'
$cultura = [Globalization.CultureInfo]::InvariantCulture
$corridas = [System.Collections.Generic.List[object]]::new()

function Ejecutar-Medicion {
    param(
        [Parameter(Mandatory = $true)][string]$Ejecutable,
        [Parameter(Mandatory = $true)][string[]]$Argumentos,
        [Parameter(Mandatory = $true)][int]$Repeticion
    )

    $salida = & $Ejecutable @Argumentos
    if ($LASTEXITCODE -ne 0) {
        throw "La ejecucion de $Ejecutable fallo."
    }
    $registro = $salida | ConvertFrom-Csv
    return [PSCustomObject]@{
        integrante = $Integrante
        repeticion = $Repeticion
        modo = $registro.modo
        rectangulos = [long]::Parse($registro.rectangulos, $cultura)
        hilos = [int]::Parse($registro.hilos, $cultura)
        area = [double]::Parse($registro.area, $cultura)
        segundos = [double]::Parse($registro.segundos, $cultura)
    }
}

function Obtener-Mediana {
    param([Parameter(Mandatory = $true)][double[]]$Valores)

    $ordenados = @($Valores | Sort-Object)
    $centro = [int][Math]::Floor($ordenados.Count / 2)
    if ($ordenados.Count % 2 -eq 1) {
        return $ordenados[$centro]
    }
    return ($ordenados[$centro - 1] + $ordenados[$centro]) / 2.0
}

Write-Host "Calentamiento (no se registra)..."
& $secuencial --rectangles ([Math]::Min($Rectangulos, 1000000)) --csv | Out-Null
if ($LASTEXITCODE -ne 0) {
    throw "Fallo el calentamiento."
}

for ($repeticion = 1; $repeticion -le $Repeticiones; ++$repeticion) {
    Write-Host "Secuencial: repeticion $repeticion de $Repeticiones"
    $corridas.Add((Ejecutar-Medicion -Ejecutable $secuencial `
        -Argumentos @("--rectangles", "$Rectangulos", "--csv") `
        -Repeticion $repeticion))
}

foreach ($cantidadHilos in $cantidadesHilos) {
    for ($repeticion = 1; $repeticion -le $Repeticiones; ++$repeticion) {
        Write-Host "Paralelo ($cantidadHilos hilo(s)): repeticion $repeticion de $Repeticiones"
        $corridas.Add((Ejecutar-Medicion -Ejecutable $paralelo `
            -Argumentos @(
                "--rectangles", "$Rectangulos",
                "--threads", "$cantidadHilos",
                "--csv"
            ) `
            -Repeticion $repeticion))
    }
}

$tiemposSecuenciales = @(
    $corridas |
        Where-Object { $_.modo -eq "secuencial" } |
        ForEach-Object { $_.segundos }
)
$medianaSecuencial = Obtener-Mediana -Valores $tiemposSecuenciales
$areaSecuencial = ($corridas | Where-Object { $_.modo -eq "secuencial" } | Select-Object -First 1).area

$resumen = foreach ($cantidadHilos in $cantidadesHilos) {
    $mediciones = @(
        $corridas |
            Where-Object { $_.modo -eq "paralelo" -and $_.hilos -eq $cantidadHilos }
    )
    $medianaParalela = Obtener-Mediana -Valores @($mediciones | ForEach-Object { $_.segundos })
    $speedup = $medianaSecuencial / $medianaParalela
    $eficiencia = 100.0 * $speedup / $cantidadHilos
    $errorMaximo = ($mediciones | ForEach-Object {
        [Math]::Abs($_.area - $areaSecuencial) / [Math]::Abs($areaSecuencial)
    } | Measure-Object -Maximum).Maximum

    [PSCustomObject]@{
        integrante = $Integrante
        rectangulos = $Rectangulos
        hilos = $cantidadHilos
        tiempo_secuencial_mediana_s = $medianaSecuencial
        tiempo_paralelo_mediana_s = $medianaParalela
        speedup = $speedup
        eficiencia_porcentaje = $eficiencia
        error_relativo_maximo = $errorMaximo
    }
}

$rutaCorridas = Join-Path $directorioResultados "${nombreSeguro}_corridas.csv"
$rutaResumen = Join-Path $directorioResultados "${nombreSeguro}_resumen.csv"
$corridas | Export-Csv -LiteralPath $rutaCorridas -NoTypeInformation -Encoding utf8
$resumen | Export-Csv -LiteralPath $rutaResumen -NoTypeInformation -Encoding utf8

Write-Host ""
Write-Host "Resumen de $Integrante (medianas de $Repeticiones repeticiones):"
$resumen | Format-Table hilos, tiempo_secuencial_mediana_s, `
    tiempo_paralelo_mediana_s, speedup, eficiencia_porcentaje, error_relativo_maximo `
    -AutoSize
Write-Host "Corridas: $rutaCorridas"
Write-Host "Resumen: $rutaResumen"
