


// 06/26/2014 23:20:15

#include "stdafx.h"
#include "DataPacket.generated.h"

	


		wstring ConnectionInfo::Members::Address = L"Address";
		wstring ConnectionInfo::Members::Port = L"Port";
    
		wstring ConnectionInfo::SqlVarMembers::Address = L"Address";
		wstring ConnectionInfo::SqlVarMembers::Port = L"Port";



		wstring CpuInfo::Members::unit = L"unit";
		wstring CpuInfo::Members::total = L"total";
    
		wstring CpuInfo::SqlVarMembers::unit = L"unit";
		wstring CpuInfo::SqlVarMembers::total = L"total";



		wstring MemoryInfo::Members::max = L"max";
		wstring MemoryInfo::Members::current = L"current";
    
		wstring MemoryInfo::SqlVarMembers::max = L"max";
		wstring MemoryInfo::SqlVarMembers::current = L"current";



		wstring DiskInfo::Members::name = L"name";
		wstring DiskInfo::Members::size = L"size";
		wstring DiskInfo::Members::current = L"current";
    
		wstring DiskInfo::SqlVarMembers::name = L"name";
		wstring DiskInfo::SqlVarMembers::size = L"size";
		wstring DiskInfo::SqlVarMembers::current = L"current";



		wstring PacketBase::Members::groupKey = L"groupKey";
		wstring PacketBase::Members::machineId = L"machineId";
    
		wstring PacketBase::SqlVarMembers::groupKey = L"groupKey";
		wstring PacketBase::SqlVarMembers::machineId = L"machineId";



		wstring SystemInfo::Members::cpuUsage = L"cpuUsage";
		wstring SystemInfo::Members::memoryUsage = L"memoryUsage";
    
		wstring SystemInfo::SqlVarMembers::cpuUsage = L"cpuUsage";
		wstring SystemInfo::SqlVarMembers::memoryUsage = L"memoryUsage";



		wstring StorageInfo::Members::disk = L"disk";
    
		wstring StorageInfo::SqlVarMembers::disk = L"disk";



