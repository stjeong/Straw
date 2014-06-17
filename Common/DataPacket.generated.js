/*
SystemInfo
{
	"CpuUsage":
		{
            "Unit":[15.5,0.0,0.0,0.0]},
		    "Total": 3.87	
		},
	"MemoryUsage": {"Max":8589934592, "Current":6442450944},
	"ApiKey": "f31fc79434df8e4b7f9fd1f5bebe5b111baf8571",
	"EnvInfo": "machineName"
}
*/




// 06/16/2014 20:30:05


var ConnectionInfo =	
{

	address: "straw.imyoyo.net",
	
	port: 8282,
	

	Members: [
		"Address",
		"Port",
	]
}

	


var CpuInfo =	
{

	unit: 0.0,
	
	total: 0.0,
	

	Members: [
		"unit",
		"total",
	]
}

	


var MemoryInfo =	
{

	max: 0,
	
	current: 0,
	

	Members: [
		"max",
		"current",
	]
}

	


var PacketBase =	
{

	groupKey: "",
	
	machineId: "",
	

	Members: [
		"groupKey",
		"machineId",
	]
}

	


var SystemInfo =	
{

	cpuUsage: null,
	
	memoryUsage: null,
	

	Members: [
		"groupKey",
		"machineId",
		"cpuUsage",
		"memoryUsage",
	]
}

			
            SystemInfo.prototype = new PacketBase();

				





