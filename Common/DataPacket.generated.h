

#pragma once


// 2014-05-29 오전 1:07:31

#include <string>
#include <vector>
using namespace std;
	

class ConnectionInfo
		
{
public:

    ConnectionInfo()
    {
    }

	wstring _address = L"192.168.0.10";
	
	int _port = 80;
	

	class Members
	{
    public:
		static wstring Address;
		static wstring Port;
	};
    
	class SqlVarMembers
	{
    public:
		static wstring Address;
		static wstring Port;
	};
};


class CpuInfo
		
{
public:

    CpuInfo()
    {
    }

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

    MemoryInfo()
    {
    }

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

    PacketBase()
    {
    }

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

    SystemInfo()
    {
    }

	CpuInfo _cpuUsage;
	
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



