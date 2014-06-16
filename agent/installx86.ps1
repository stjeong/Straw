

$client = New-Object System.Net.WebClient
[xml]$xmlData = $client.DownloadString("https://raw.githubusercontent.com/stjeong/Straw/master/agent/windows/version.xml")

$locationNode = Select-Xml -Xml $xmlData -XPath "//location[@platform='win32']"
$moduleAddress = $locationNode[0].Node.InnerXml

$url = New-Object System.Uri($moduleAddress)

$fileName = [IO.Path]::GetFileName($url.LocalPath)
$localFilePath = [IO.Path]::Combine($PSScriptRoot, $fileName)

[IO.File]::Delete($localFilePath)

$client.DownloadFile($moduleAddress, $localFilePath)

./ic32.exe -key ce3bd840-f0a7-11e3-ac10-0800200c9a66 -regservice -debug
./ic32.exe -start


# $client.DownloadFile(