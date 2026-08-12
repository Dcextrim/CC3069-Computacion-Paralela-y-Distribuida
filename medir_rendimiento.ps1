param(
    [string]$Archivo = "texto_benchmark.txt",
    [int]$Repeticiones = 5,
    [string]$Salida = "resultados_medicion.csv"
)

$ErrorActionPreference = "Stop"

function Obtener-TiempoMs {
    param(
        [ValidateSet("secuencial", "paralelo")]
        [string]$Modo,
        [int]$Hilos = 1
    )

    if ($Modo -eq "secuencial") {
        $salidaPrograma = & ".\contador_secuencial.exe" $Archivo
    }
    else {
        $salidaPrograma = & ".\contador_paralelo.exe" $Archivo $Hilos
    }

    if ($LASTEXITCODE -ne 0) {
        throw "El programa $Modo termino con codigo $LASTEXITCODE."
    }

    $coincidencia = [regex]::Match(
        ($salidaPrograma -join "`n"),
        "TIEMPO_PROCESAMIENTO_MS=([0-9]+(?:\.[0-9]+)?)"
    )
    if (-not $coincidencia.Success) {
        throw "No se encontro la medicion en la salida de $Modo."
    }

    return [double]::Parse(
        $coincidencia.Groups[1].Value,
        [Globalization.CultureInfo]::InvariantCulture
    )
}

# Una corrida de calentamiento por configuracion; no se registra.
[void](Obtener-TiempoMs -Modo secuencial)
[void](Obtener-TiempoMs -Modo paralelo -Hilos 2)
[void](Obtener-TiempoMs -Modo paralelo -Hilos 4)

$resultados = for ($i = 1; $i -le $Repeticiones; $i++) {
    # Se rota el orden para reducir el sesgo por calentamiento o carga temporal.
    $orden = switch ($i % 3) {
        1 { @("secuencial", "hilos2", "hilos4") }
        2 { @("hilos2", "hilos4", "secuencial") }
        0 { @("hilos4", "secuencial", "hilos2") }
    }

    $mediciones = @{}
    foreach ($configuracion in $orden) {
        $mediciones[$configuracion] = switch ($configuracion) {
            "secuencial" { Obtener-TiempoMs -Modo secuencial }
            "hilos2" { Obtener-TiempoMs -Modo paralelo -Hilos 2 }
            "hilos4" { Obtener-TiempoMs -Modo paralelo -Hilos 4 }
        }
    }

    $speedup2 = $mediciones.secuencial / $mediciones.hilos2
    $speedup4 = $mediciones.secuencial / $mediciones.hilos4

    [pscustomobject]@{
        Ejecucion = $i
        TiempoSecuencialMs = [math]::Round($mediciones.secuencial, 6)
        Tiempo2HilosMs = [math]::Round($mediciones.hilos2, 6)
        Tiempo4HilosMs = [math]::Round($mediciones.hilos4, 6)
        Speedup2 = [math]::Round($speedup2, 6)
        Speedup4 = [math]::Round($speedup4, 6)
        Efficiency2 = [math]::Round($speedup2 / 2.0, 6)
        Efficiency4 = [math]::Round($speedup4 / 4.0, 6)
    }
}

$resultados | Export-Csv -LiteralPath $Salida -NoTypeInformation -Encoding utf8
$resultados | Format-Table -AutoSize
