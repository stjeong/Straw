
$buildStartTime = [DateTime]::Now
Write-Output "===================== Build start straw agent for Windows"

$projectPath = [IO.Path]::Combine($PSScriptRoot, "windows")
$deployPath = [IO.Path]::Combine($projectPath, "deploy")
$projectFilePath = [IO.Path]::Combine($projectPath, "InfoCollector.vcxproj")

$x86AgentName = "ic32.exe"
$x64AgentName = "ic64.exe"
$autoDeployPath = "https://raw.githubusercontent.com/stjeong/Straw/master/agent/windows/deploy/"

Set-Location $projectPath
Write-Output($projectFilePath)

# $vsDevBatPath = "E:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.cmd"

[Environment]::SetEnvironmentVariable("VisualStudioVersion", "12.0")

$vsInstallPath = [Environment]::GetEnvironmentVariable("VisualStudioVersion")

$windirPath = [Environment]::GetEnvironmentVariable("windir")

$netFxPath = [Microsoft.Win32.Registry]::GetValue("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\.NETFramework", "InstallRoot", "")

Write-Output ("VisualStudioVersion: " + $vsInstallPath)
Write-Output ("windir: " + $windirPath)
Write-Output ("netFxPath: " + $netFxPath)

$msbuildPath = [IO.Path]::Combine($netFxPath, "v4.0.30319", "msbuild.exe")

while ($true)
{
    Write-Output "===================== x86 build"
    if ($true)
    {
        & $msbuildPath $projectFilePath "/p:Platform=Win32;Configuration=Release" /t:Rebuild

        # cmd.exe /C $msbuildPath $projectFilePath "/p:Platform=Win32;Configuration=Release" /t:Rebuild
        $buildResult = $?

        Write-Output("msbuild returns: " + $buildResult)
        if ($buildResult -eq $false)
        {
            break
        }

        $output32FilePath = [IO.Path]::Combine($projectPath, "release", $x86AgentName)
        Write-Output ($x86AgentName + " : " + $output32FilePath)
        $fileVersion = [Diagnostics.FileVersionInfo]::GetVersionInfo($output32FilePath).FileVersion
        Write-Output ($x86AgentName + " version : " + $fileVersion)

        $deployVersionPath = [IO.Path]::Combine($deployPath, $fileVersion)
        if ([IO.Directory]::Exists($deployVersionPath) -eq $false)
        {
            [IO.Directory]::CreateDirectory($deployVersionPath)
        }

        copy $output32FilePath $deployVersionPath
    }

    Write-Output "===================== x64 build"
    if ($true)
    {
        & $msbuildPath $projectFilePath "/p:Platform=x64;Configuration=Release" /t:Rebuild
        $buildResult = $?

        Write-Output("msbuild returns: " + $buildResult)
        if ($buildResult -eq $false)
        {
            break
        }

        $output64FilePath = [IO.Path]::Combine($projectPath, "x64", "release", $x64AgentName)
        Write-Output ($x64AgentName + " : " + $output64FilePath)
        $fileVersion = [Diagnostics.FileVersionInfo]::GetVersionInfo($output64FilePath).FileVersion
        Write-Output ($x64AgentName + " version : " + $fileVersion)

        copy $output64FilePath $deployVersionPath
    }

    Write-Output "===================== Update Link"
    $versionFilePath = [IO.Path]::Combine($projectPath, "version.xml")

    $xmlDoc = New-Object Xml.XmlDocument
    $xmlDoc.PreserveWhitespace = $true
    $xmlDoc.Load($versionFilePath)

    $xmlDoc.SelectSingleNode("/response/version").InnerText = $fileVersion
    $xmlDoc.SelectSingleNode("/response/location[@platform='win32']").InnerText = $autoDeployPath + $fileVersion + "/" + $x86AgentName
    $xmlDoc.SelectSingleNode("/response/location[@platform='win64']").InnerText = $autoDeployPath + $fileVersion + "/" + $x64AgentName

    $xmlDoc.Save($versionFilePath)

    break;
}

Write-Output "===================== Build Ended"

Write-Output ("Build ended (" + ([DateTime]::Now - $buildStartTime).TotalSeconds + " secs)")


Set-Location $PSScriptRoot