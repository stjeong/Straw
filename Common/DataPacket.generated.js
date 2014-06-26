/*
SystemInfo
{
	"cpuUsage":
		{
            "unit":[15.5,0.0,0.0,0.0]},
		    "total": 3.87	
		},
	"memoryUsage": {"max":8589934592, "current":6442450944},
	"groupKey": "f31fc79434df8e4b7f9fd1f5bebe5b111baf8571",
	"machineId": "machineName"
}

StorageInfo
{
    "disk":
        [
            { "name": "C", "size": 163243, "current": 34322 },
            { "name": "D", "size": 163243, "current": 34322 }
        ],
	"groupKey": "f31fc79434df8e4b7f9fd1f5bebe5b111baf8571",
	"machineId": "machineName"
}

*/




// 06/26/2014 23:20:15


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

	


var DiskInfo =	
{

	name: "",
	
	size: 0,
	
	current: 0,
	

	Members: [
		"name",
		"size",
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

				


var StorageInfo =	
{

	disk: null,
	

	Members: [
		"groupKey",
		"machineId",
		"disk",
	]
}

			
            StorageInfo.prototype = new PacketBase();

				





