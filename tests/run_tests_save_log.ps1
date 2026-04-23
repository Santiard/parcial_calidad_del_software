# Ejecuta los dos binarios de prueba y guarda la salida en texto (y opcionalmente XML de Google Test).
# Uso (desde la carpeta tests):
#   .\run_tests_save_log.ps1
#   .\run_tests_save_log.ps1 -QtBin "C:\Qt\5.15.2\mingw81_64\bin" -MingwBin "C:\Qt\Tools\mingw810_64\bin"
# Requiere haber compilado antes (cmake --build build).

param(
    [string] $BuildDir = "$PSScriptRoot\build",
    [string] $OutTxt = "$PSScriptRoot\test_results.txt",
    [string] $QtBin = "C:\Qt\5.15.2\mingw81_64\bin",
    [string] $MingwBin = "C:\Qt\Tools\mingw810_64\bin",
    [switch] $Xml
)

$ErrorActionPreference = "Continue"

if (Test-Path $QtBin) {
    $env:Path = "$QtBin;$MingwBin;" + $env:Path
}

$blackbox = Join-Path $BuildDir "buggy_calc_blackbox_tests.exe"
$whitebox = Join-Path $BuildDir "buggy_calc_whitebox_tests.exe"

$stamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss zzz"
$lines = New-Object System.Collections.Generic.List[string]
[void]$lines.Add("Buggy Calculator - salida de pruebas")
[void]$lines.Add("Generado: $stamp")
[void]$lines.Add("")

function Invoke-Suite {
    param([string] $Title, [string] $Exe, [string] $XmlPath)

    [void]$lines.Add("================================================================================")
    [void]$lines.Add(" $Title ")
    [void]$lines.Add("================================================================================")
    if (-not (Test-Path $Exe)) {
        [void]$lines.Add("ERROR: No existe el ejecutable: $Exe")
        [void]$lines.Add("Compila antes: cmake -S . -B build ... ; cmake --build build")
        [void]$lines.Add("")
        return 1
    }

    $argList = @()
    if ($Xml -and $XmlPath) {
        $argList += "--gtest_output=xml:$XmlPath"
    }

    $pinfo = New-Object System.Diagnostics.ProcessStartInfo
    $pinfo.FileName = $Exe
    $pinfo.Arguments = ($argList -join " ")
    $pinfo.RedirectStandardOutput = $true
    $pinfo.RedirectStandardError = $true
    $pinfo.UseShellExecute = $false
    $pinfo.CreateNoWindow = $true
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $pinfo
    [void]$p.Start()
    $out = $p.StandardOutput.ReadToEnd()
    $err = $p.StandardError.ReadToEnd()
    [void]$p.WaitForExit()
    foreach ($line in ($out -split "`n")) {
        [void]$lines.Add($line.TrimEnd("`r"))
    }
    foreach ($line in ($err -split "`n")) {
        if ($line.Trim().Length -gt 0) {
            [void]$lines.Add($line.TrimEnd("`r"))
        }
    }
    [void]$lines.Add("")
    [void]$lines.Add("(codigo salida: $($p.ExitCode))")
    [void]$lines.Add("")
    return $p.ExitCode
}

$exit1 = Invoke-Suite "Caja negra - buggy_calc_blackbox_tests" $blackbox $(if ($Xml) { Join-Path $BuildDir "gtest_blackbox.xml" } else { $null })
$exit2 = Invoke-Suite "Caja blanca - buggy_calc_whitebox_tests" $whitebox $(if ($Xml) { Join-Path $BuildDir "gtest_whitebox.xml" } else { $null })

$utf8NoBom = New-Object System.Text.UTF8Encoding $false
[System.IO.File]::WriteAllLines($OutTxt, $lines, $utf8NoBom)

Write-Host "Listo. Salida guardada en:"
Write-Host "  $OutTxt"
if ($Xml) {
    Write-Host "XML (Google Test):"
    Write-Host "  $(Join-Path $BuildDir 'gtest_blackbox.xml')"
    Write-Host "  $(Join-Path $BuildDir 'gtest_whitebox.xml')"
}

exit [Math]::Max($exit1, $exit2)
