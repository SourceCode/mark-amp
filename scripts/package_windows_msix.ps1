<#
.SYNOPSIS
    Package MarkAmp as MSIX for Microsoft Store submission.

.DESCRIPTION
    Creates an MSIX package using makeappx.exe from the Windows SDK.
    Requires: Windows SDK with makeappx.exe and signtool.exe.

.PARAMETER SkipSign
    Skip MSIX signing (for CI without certificates).

.PARAMETER CertPath
    Path to .pfx certificate for signing (or set MARKAMP_CERT_PATH env var).
#>
[CmdletBinding()]
param(
    [switch]$SkipSign,
    [string]$CertPath
)

$ErrorActionPreference = "Stop"

$ProjectDir = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$BuildDir = Join-Path $ProjectDir "build\release"
$DistDir = Join-Path $ProjectDir "dist"

# Read version
$CMakeLists = Get-Content (Join-Path $ProjectDir "CMakeLists.txt") -Raw
if ($CMakeLists -match 'project\(MarkAmp VERSION (\d+\.\d+\.\d+)') {
    $Version = $Matches[1]
} else {
    $Version = "1.0.0"
}

Write-Host "=== Creating MSIX Package for MarkAmp v$Version ===" -ForegroundColor Cyan

$Binary = Join-Path $BuildDir "src\Release\markamp.exe"
if (-not (Test-Path $Binary)) {
    $Binary = Join-Path $BuildDir "src\markamp.exe"
}
if (-not (Test-Path $Binary)) {
    Write-Error "Release binary not found. Run build_release first."
    exit 1
}

# ── Prepare MSIX content directory ──
$MsixContent = Join-Path $DistDir "msix-staging"
if (Test-Path $MsixContent) { Remove-Item $MsixContent -Recurse -Force }
New-Item -ItemType Directory -Path $MsixContent -Force | Out-Null

# Copy binary
Copy-Item $Binary (Join-Path $MsixContent "markamp.exe")

# Copy manifest
$ManifestSrc = Join-Path $ProjectDir "packaging\windows\AppxManifest.xml"
Copy-Item $ManifestSrc (Join-Path $MsixContent "AppxManifest.xml")

# Copy resources
$ResourceDirs = @("themes", "fonts", "sample_files")
foreach ($dir in $ResourceDirs) {
    $src = Join-Path $ProjectDir "resources\$dir"
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $MsixContent $dir) -Recurse
    }
}

# ── Create Assets directory with placeholder logos ──
$AssetsDir = Join-Path $MsixContent "Assets"
New-Item -ItemType Directory -Path $AssetsDir -Force | Out-Null

# Copy icon as store assets (these should be replaced with properly sized assets)
$IconSrc = Join-Path $ProjectDir "resources\icons\markamp.png"
if (Test-Path $IconSrc) {
    $AssetNames = @(
        "Square44x44Logo.png",
        "Square150x150Logo.png",
        "Square310x310Logo.png",
        "Wide310x150Logo.png",
        "StoreLogo.png",
        "SplashScreen.png"
    )
    foreach ($asset in $AssetNames) {
        Copy-Item $IconSrc (Join-Path $AssetsDir $asset)
    }
    Write-Host "  Note: Asset PNGs are placeholders. Replace with correctly sized images before Store submission."
}

# ── Create MSIX ──
$MsixName = "MarkAmp-$Version.msix"
$MsixPath = Join-Path $DistDir $MsixName

Write-Host "`n--- Creating MSIX package ---" -ForegroundColor Yellow

$Makeappx = Get-Command makeappx -ErrorAction SilentlyContinue
if ($Makeappx) {
    & makeappx pack /d $MsixContent /p $MsixPath /o
    Write-Host "  Created: $MsixName"
} else {
    # Try finding in Windows SDK
    $SdkPaths = @(
        "${env:ProgramFiles(x86)}\Windows Kits\10\bin\*\x64\makeappx.exe"
    )
    $Found = $null
    foreach ($pattern in $SdkPaths) {
        $Found = Get-Item $pattern -ErrorAction SilentlyContinue | Sort-Object -Descending | Select-Object -First 1
        if ($Found) { break }
    }
    if ($Found) {
        & $Found.FullName pack /d $MsixContent /p $MsixPath /o
        Write-Host "  Created: $MsixName"
    } else {
        Write-Host "  Warning: makeappx.exe not found. Install Windows SDK."
        Write-Host "  Manual command: makeappx pack /d $MsixContent /p $MsixPath"
    }
}

# Clean staging
Remove-Item $MsixContent -Recurse -Force

# ── Sign MSIX ──
if (-not $SkipSign) {
    $Cert = if ($CertPath) { $CertPath } else { $env:MARKAMP_CERT_PATH }
    $CertPass = $env:MARKAMP_CERT_PASSWORD

    if ($Cert -and (Test-Path $Cert) -and (Test-Path $MsixPath)) {
        Write-Host "`n--- Signing MSIX ---" -ForegroundColor Yellow
        signtool sign /fd SHA256 /a /f $Cert /p $CertPass $MsixPath
        Write-Host "  Signed: $MsixName"
    } else {
        Write-Host "`n  Warning: Certificate not found, MSIX is unsigned"
    }
}

# ── Validate with WACK ──
Write-Host "`n--- Validation ---" -ForegroundColor Yellow
$Wack = Get-Command "appcert.exe" -ErrorAction SilentlyContinue
if ($Wack -and (Test-Path $MsixPath)) {
    Write-Host "  Running Windows App Certification Kit..."
    & appcert.exe test -appxpackagepath $MsixPath -reportoutputpath (Join-Path $DistDir "wack-report.xml")
} else {
    Write-Host "  WACK not found. Run manually: appcert.exe test -appxpackagepath $MsixPath"
}

Write-Host "`n=== MSIX packaging complete ===" -ForegroundColor Cyan
Write-Host "  Package: $MsixPath"
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Replace placeholder Assets with correctly sized logos"
Write-Host "  2. Sign with a trusted certificate"
Write-Host "  3. Test with WACK: appcert.exe test -appxpackagepath $MsixPath"
Write-Host "  4. Upload to Microsoft Partner Center"
