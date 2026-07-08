param(
    [string]$Version = "1.1.1",
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$RepoRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")).Path
$BuildDir = Join-Path $RepoRoot "build"
$OutputDir = Join-Path $BuildDir $Configuration
$DistDir = Join-Path $RepoRoot "dist"
$PackageName = "KeySprint-$Version-windows"
$PackageDir = Join-Path $DistDir $PackageName
$ZipPath = Join-Path $DistDir "$PackageName.zip"
$ExePath = Join-Path $OutputDir "KeySprint.exe"
$AssetsPath = Join-Path $RepoRoot "assets"

function Assert-InRepo {
    param([string]$Path)

    $fullPath = [System.IO.Path]::GetFullPath($Path)
    $rootWithSlash = $RepoRoot.TrimEnd([System.IO.Path]::DirectorySeparatorChar) + [System.IO.Path]::DirectorySeparatorChar

    if (-not ($fullPath.Equals($RepoRoot, [System.StringComparison]::OrdinalIgnoreCase) -or
        $fullPath.StartsWith($rootWithSlash, [System.StringComparison]::OrdinalIgnoreCase))) {
        throw "Refusing to touch path outside repository: $fullPath"
    }

    return $fullPath
}

function Remove-IfExists {
    param([string]$Path)

    $safePath = Assert-InRepo $Path
    if (Test-Path -LiteralPath $safePath) {
        Remove-Item -LiteralPath $safePath -Recurse -Force
    }
}

Push-Location $RepoRoot
try {
    if (-not (Test-Path -LiteralPath (Join-Path $BuildDir "CMakeCache.txt"))) {
        throw "Build directory is not configured. Run CMake configure before packaging."
    }

    Write-Host "Building Key Sprint $Version ($Configuration)..."
    & cmake --build $BuildDir --config $Configuration
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE."
    }

    if (-not (Test-Path -LiteralPath $ExePath)) {
        throw "Executable not found: $ExePath"
    }

    if (-not (Test-Path -LiteralPath $AssetsPath)) {
        throw "Assets folder not found: $AssetsPath"
    }

    New-Item -ItemType Directory -Force -Path $DistDir | Out-Null
    Remove-IfExists $PackageDir
    Remove-IfExists $ZipPath
    New-Item -ItemType Directory -Force -Path $PackageDir | Out-Null

    Copy-Item -LiteralPath $ExePath -Destination (Join-Path $PackageDir "KeySprint.exe") -Force
    Copy-Item -LiteralPath $AssetsPath -Destination (Join-Path $PackageDir "assets") -Recurse -Force

    Get-ChildItem -LiteralPath $OutputDir -Filter "*.dll" -File -ErrorAction SilentlyContinue | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination $PackageDir -Force
    }

    $readme = @"
Key Sprint $Version

How to run:
1. Unzip the archive.
2. Run KeySprint.exe.
3. Keep the assets folder next to KeySprint.exe. It contains fonts, icons, and typing content.

Useful keys:
- ESC: return to menu.
- F11: toggle fullscreen.

Typing content can be expanded in assets/content/typing_content.json.
"@

    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText((Join-Path $PackageDir "README.txt"), $readme, $utf8NoBom)

    Compress-Archive -Path (Join-Path $PackageDir "*") -DestinationPath $ZipPath -Force

    Write-Host "Package folder: $PackageDir"
    Write-Host "Archive: $ZipPath"
}
finally {
    Pop-Location
}
