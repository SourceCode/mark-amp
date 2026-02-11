<#
.SYNOPSIS
    Package MarkAmp for Windows: NSIS installer and portable ZIP.

.DESCRIPTION
    Creates MarkAmp-Setup.exe (NSIS installer) and MarkAmp-Portable.zip.
    Requires NSIS to be installed (choco install nsis).

.PARAMETER SkipSign
    Skip Authenticode code signing.
#>
[CmdletBinding()]
param(
    [switch]$SkipSign
)

$ErrorActionPreference = "Stop"

$ProjectDir = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$BuildDir = Join-Path $ProjectDir "build\release"
$DistDir = Join-Path $ProjectDir "dist"

# Read version from CMakeLists.txt
$CMakeLists = Get-Content (Join-Path $ProjectDir "CMakeLists.txt") -Raw
if ($CMakeLists -match 'project\(MarkAmp VERSION (\d+\.\d+\.\d+)') {
    $Version = $Matches[1]
} else {
    $Version = "1.0.0"
}

Write-Host "=== Packaging MarkAmp v$Version for Windows ===" -ForegroundColor Cyan

$Binary = Join-Path $BuildDir "src\Release\markamp.exe"
if (-not (Test-Path $Binary)) {
    $Binary = Join-Path $BuildDir "src\markamp.exe"
}
if (-not (Test-Path $Binary)) {
    Write-Error "Release binary not found. Run build_release first."
    exit 1
}

# Clean and create dist directory
if (Test-Path $DistDir) { Remove-Item $DistDir -Recurse -Force }
New-Item -ItemType Directory -Path $DistDir -Force | Out-Null

# ── Portable ZIP ──
Write-Host "`n--- Creating portable ZIP ---" -ForegroundColor Yellow

$PortableDir = Join-Path $DistDir "MarkAmp-$Version-Portable"
New-Item -ItemType Directory -Path $PortableDir -Force | Out-Null

Copy-Item $Binary (Join-Path $PortableDir "markamp.exe")

# Copy resources
$ResourceDirs = @("themes", "fonts", "sample_files")
foreach ($dir in $ResourceDirs) {
    $src = Join-Path $ProjectDir "resources\$dir"
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $PortableDir $dir) -Recurse
    }
}

# Create README
@"
MarkAmp v$Version - Portable Edition

Run markamp.exe to start the application.
No installation required.

Website: https://github.com/markamp/markamp
"@ | Set-Content (Join-Path $PortableDir "README.txt")

$ZipName = "MarkAmp-$Version-Windows-Portable.zip"
Compress-Archive -Path $PortableDir -DestinationPath (Join-Path $DistDir $ZipName)
Remove-Item $PortableDir -Recurse -Force
Write-Host "  Created: $ZipName"

# ── NSIS Installer ──
Write-Host "`n--- Creating NSIS installer ---" -ForegroundColor Yellow

$NsisScript = Join-Path $ProjectDir "packaging\windows\installer.nsi"
$MakeNsis = Get-Command makensis -ErrorAction SilentlyContinue

if ($MakeNsis -and (Test-Path $NsisScript)) {
    & makensis /DVERSION="$Version" /DBUILD_DIR="$BuildDir" /DPROJECT_DIR="$ProjectDir" /DOUTDIR="$DistDir" "$NsisScript"
    Write-Host "  Created: MarkAmp-$Version-Setup.exe"
} else {
    Write-Host "  Warning: NSIS not found or installer.nsi missing, skipping installer"
}

# ── Code signing ──
if (-not $SkipSign) {
    $CertPath = $env:MARKAMP_CERT_PATH
    $CertPass = $env:MARKAMP_CERT_PASSWORD
    if ($CertPath -and (Test-Path $CertPath)) {
        Write-Host "`n--- Code signing ---" -ForegroundColor Yellow
        $FilesToSign = Get-ChildItem $DistDir -Filter "*.exe"
        foreach ($file in $FilesToSign) {
            signtool sign /f $CertPath /p $CertPass /t http://timestamp.digicert.com /d "MarkAmp" $file.FullName
            Write-Host "  Signed: $($file.Name)"
        }
    } else {
        Write-Host "`n  Warning: MARKAMP_CERT_PATH not set, skipping code signing"
    }
}

Write-Host "`n=== Windows packaging complete ===" -ForegroundColor Cyan
Get-ChildItem $DistDir | Format-Table Name, Length -AutoSize
