

#pragma once


// 2014-05-25 오후 2:21:39

#include <string>
#include <vector>
using namespace std;
	

class CpuInfo
		
{
public:
	vector<float> _unit;
	

	class Members
	{
    public:
		static wstring Unit;
	};
    
	class SqlVarMembers
	{
    public:
		static wstring Unit;
	};
};


class MemoryInfo
		
{
public:
	long _maxMB;
	
	long _currentMB;
	

	class Members
	{
    public:
		static wstring MaxMB;
		static wstring CurrentMB;
	};
    
	class SqlVarMembers
	{
    public:
		static wstring MaxMB;
		static wstring CurrentMB;
	};
};


class PacketBase
		
{
public:
	wstring _apiKey;
	
	wstring _envInfo;
	

	class Members
	{
    public:
		static wstring ApiKey;
		static wstring EnvInfo;
	};
    
	class SqlVarMembers
	{
    public:
		static wstring ApiKey;
		static wstring EnvInfo;
	};
};


class SystemInfo
			: 

            			    PacketBase
			
					
{
public:
	vector<CpuInfo> _cpuUsage;
	
	MemoryInfo _memoryUsage;
	

	class Members
	{
    public:
		wstring ApiKey;
		wstring EnvInfo;
		static wstring CpuUsage;
		static wstring MemoryUsage;
	};
    
	class SqlVarMembers
	{
    public:
		wstring ApiKey;
		wstring EnvInfo;
		static wstring CpuUsage;
		static wstring MemoryUsage;
	};
};



