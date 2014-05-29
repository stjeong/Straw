


// 2014-05-29 오후 11:17:20

#include "stdafx.h"
#include "DataPacket.generated.h"

	


		wstring ConnectionInfo::Members::Address = L"Address";
		wstring ConnectionInfo::Members::Port = L"Port";
    
		wstring ConnectionInfo::SqlVarMembers::Address = L"Address";
		wstring ConnectionInfo::SqlVarMembers::Port = L"Port";



		wstring CpuInfo::Members::Unit = L"Unit";
		wstring CpuInfo::Members::Total = L"Total";
    
		wstring CpuInfo::SqlVarMembers::Unit = L"Unit";
		wstring CpuInfo::SqlVarMembers::Total = L"Total";



		wstring MemoryInfo::Members::MaxMB = L"MaxMB";
		wstring MemoryInfo::Members::CurrentMB = L"CurrentMB";
    
		wstring MemoryInfo::SqlVarMembers::MaxMB = L"MaxMB";
		wstring MemoryInfo::SqlVarMembers::CurrentMB = L"CurrentMB";



		wstring PacketBase::Members::ApiKey = L"ApiKey";
		wstring PacketBase::Members::EnvInfo = L"EnvInfo";
    
		wstring PacketBase::SqlVarMembers::ApiKey = L"ApiKey";
		wstring PacketBase::SqlVarMembers::EnvInfo = L"EnvInfo";



		wstring SystemInfo::Members::CpuUsage = L"CpuUsage";
		wstring SystemInfo::Members::MemoryUsage = L"MemoryUsage";
    
		wstring SystemInfo::SqlVarMembers::CpuUsage = L"CpuUsage";
		wstring SystemInfo::SqlVarMembers::MemoryUsage = L"MemoryUsage";



