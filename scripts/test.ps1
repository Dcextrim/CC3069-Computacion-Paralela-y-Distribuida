param(
    [long]$Rectangulos = 200000
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$raiz = Split-Path -Parent $PSScriptRoot
$secuencial = Join-Path $raiz "build/secuencial.exe"
$paralelo = Join-Path $raiz "build/paralelo.exe"

if (-not (Test-Path -LiteralPath $secuencial) -or
    -not (Test-Path -LiteralPath $paralelo)) {
    & (Join-Path $PSScriptRoot "build.ps1")
}

function Ejecutar-Csv {
    param(
        [Parameter(Mandatory = $true)][string]$Ejecutable,
        [Parameter(Mandatory = $true)][string[]]$Argumentos
    )

    $salida = & $Ejecutable @Argumentos
    if ($LASTEXITCODE -ne 0) {
        throw "La ejecucion de $Ejecutable fallo."
    }
    return ($salida | ConvertFrom-Csv)
}

$referencia = Ejecutar-Csv -Ejecutable $secuencial `
    -Argumentos @("--rectangles", "$Rectangulos", "--csv")
$areaReferencia = [double]::Parse(
    $referencia.area,
    [Globalization.CultureInfo]::InvariantCulture
)
$areaEsperada = 30.80821253638
if ([Math]::Abs($areaReferencia - $areaEsperada) -gt 1e-8) {
    throw "La version secuencial no coincide con el valor numerico de referencia."
}
Write-Host "OK: area secuencial cercana al valor de referencia $areaEsperada"

foreach ($cantidadHilos in @(1, 2, 4)) {
    $medicion = Ejecutar-Csv -Ejecutable $paralelo `
        -Argumentos @(
            "--rectangles", "$Rectangulos",
            "--threads", "$cantidadHilos",
            "--csv"
        )
    $area = [double]::Parse(
        $medicion.area,
        [Globalization.CultureInfo]::InvariantCulture
    )
    $errorRelativo = [Math]::Abs($area - $areaReferencia) / [Math]::Abs($areaReferencia)
    if ($errorRelativo -gt 1e-11) {
        throw "El resultado con $cantidadHilos hilos difiere: error relativo $errorRelativo"
    }
    Write-Host "OK: $cantidadHilos hilo(s), error relativo = $errorRelativo"
}

Write-Host "Todas las pruebas finalizaron correctamente."
