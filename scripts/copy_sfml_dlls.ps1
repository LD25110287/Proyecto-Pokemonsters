<#
copy_sfml_dlls.ps1
Copia sfml-audio-2.dll y openal32.dll desde la instalación de SFML hacia la carpeta del ejecutable.
Uso:
  .\copy_sfml_dlls.ps1 [-SFMLBin <ruta>] [-Destination <ruta_destino>]
Si no se suministra -Destination, intentará copiar a ./build si existe, sino a la raíz del repo.
#>
[CmdletBinding()]
param(
    [string]$SFMLBin = 'C:\msys64\mingw64\bin',
    [string]$Destination = ''
)

function Write-Err { param($m) Write-Host "[ERROR] $m" -ForegroundColor Red }
function Write-Ok  { param($m) Write-Host "[OK]    $m" -ForegroundColor Green }
function Write-Info{ param($m) Write-Host "[INFO]  $m" -ForegroundColor Cyan }

# Archivos a copiar
$files = @('sfml-audio-2.dll','openal32.dll')

# Validar carpeta SFML bin
if (!(Test-Path -Path $SFMLBin)) {
    Write-Err "La carpeta de SFML no existe: $SFMLBin"
    Write-Host "Pasa el parámetro -SFMLBin con la ruta correcta, por ejemplo: -SFMLBin 'C:\msys64\mingw64\bin'"
    exit 2
}

# Determinar destino
if ([string]::IsNullOrWhiteSpace($Destination)) {
    if (Test-Path -Path './build') { $Destination = (Resolve-Path './build').Path }
    else { $Destination = (Resolve-Path '.').Path }
}

# Comprobar que destino existe
if (!(Test-Path -Path $Destination)) {
    try { New-Item -ItemType Directory -Path $Destination -Force | Out-Null }
    catch { Write-Err "No se pudo crear la carpeta destino: $Destination"; exit 3 }
}

Write-Info "Origen SFML bin: $SFMLBin"
Write-Info "Destino: $Destination"

$missing = @()
foreach ($f in $files) {
    $src = Join-Path $SFMLBin $f
    if (!(Test-Path -Path $src)) { $missing += $f }
}

if ($missing.Count -gt 0) {
    Write-Err "Faltan los siguientes archivos en ${SFMLBin}: $($missing -join ', ')"
    Write-Host "Asegúrate de que SFML esté instalado o indica la ruta correcta con -SFMLBin"
    exit 4
}

# Copiar
$errors = $false
foreach ($f in $files) {
    $src = Join-Path $SFMLBin $f
    $dst = Join-Path $Destination $f
    try {
        Copy-Item -Path $src -Destination $dst -Force -ErrorAction Stop
        Write-Ok "Copiado $f -> $dst"
    } catch {
        Write-Err "Error copiando ${f}: ${_}"
        $errors = $true
    }
}

if ($errors) { Write-Err "Algunos archivos no se copiaron correctamente."; exit 5 }
Write-Ok "Todas las DLLs fueron copiadas correctamente."
exit 0
