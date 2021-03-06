
# Install from Web

$client = New-Object System.Net.WebClient

$noCacheNoStore = [System.Net.Cache.RequestCacheLevel]::NoCacheNoStore
$client.CachePolicy = New-Object System.Net.Cache.RequestCachePolicy($noCacheNoStore)
$client.Headers.Add("Cache-Control", "no-cache")

[xml]$xmlData = $client.DownloadString("https://raw.githubusercontent.com/stjeong/Straw/master/agent/windows/version.xml")

$locationNode = Select-Xml -Xml $xmlData -XPath "//location[@platform='win32']"

$moduleAddress = $locationNode[0].Node.InnerXml

$moduleAddress

$ServiceName = "StrawAgent"

if ( Get-Service "$ServiceName*" -Include $ServiceName ) {
    Stop-Service $ServiceName
}

$url = New-Object System.Uri($moduleAddress)

$fileName = [IO.Path]::GetFileName($url.LocalPath)
$localFilePath = [IO.Path]::Combine($PSScriptRoot, $fileName)

Write-Output $localFilePath

[IO.File]::Delete($localFilePath)

Write-Host $moduleAddress

$client.DownloadFile($moduleAddress, $localFilePath)

[System.Diagnostics.FileVersionInfo]::GetVersionInfo($localFilePath).FileVersion

./ic32.exe -key ce3bd840-f0a7-11e3-ac10-0800200c9a66 -regservice
./ic32.exe -start
