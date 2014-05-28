/*
SystemInfo
{
	"CpuUsage":
		[
			{"Unit":[15.5,0.0,0.0,0.0]},
			{"Unit":[12.5,0.0,3.0,2.0]}
		],
	"MemoryUsage": {"Max":8589934592, "Current":6442450944},
	"ApiKey": "f31fc79434df8e4b7f9fd1f5bebe5b111baf8571",
	"EnvInfo":null
}
*/




// 05/29/2014 01:41:33


var ConnectionInfo =	
{

	address: "www.geeksaga.com",
	
	port: 80,
	

	Members: [
		"Address",
		"Port",
	]
}

	


var CpuInfo =	
{

	unit: 0.0,
	

	Members: [
		"Unit",
	]
}

	


var MemoryInfo =	
{

	maxMB: 0,
	
	currentMB: 0,
	

	Members: [
		"MaxMB",
		"CurrentMB",
	]
}

	


var PacketBase =	
{

	apiKey: "",
	
	envInfo: "",
	

	Members: [
		"ApiKey",
		"EnvInfo",
	]
}

	


var SystemInfo =	
{

	cpuUsage: null,
	
	memoryUsage: null,
	

	Members: [
		"ApiKey",
		"EnvInfo",
		"CpuUsage",
		"MemoryUsage",
	]
}

			
            SystemInfo.prototype = new PacketBase();

				





