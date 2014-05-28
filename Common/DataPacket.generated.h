

#pragma once


// 2014-05-29 오전 1:41:34

#include <string>
#include <vector>
using namespace std;
	

class ConnectionInfo
		
{
public:

    ConnectionInfo()
    {
    }

private:
	wstring _address = L"www.geeksaga.com";
public:
	wstring Getaddress()
    {
        return _address;
    }
	void Setaddress(wstring value)
    {
        _address = value;
    }
private:
	int _port = 80;
public:
	int Getport()
    {
        return _port;
    }
	void Setport(int value)
    {
        _port = value;
    }

public:
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

private:
	vector<float> _unit;
public:
	vector<float> Getunit()
    {
        return _unit;
    }
	void Setunit(vector<float> value)
    {
        _unit = value;
    }

public:
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

private:
	long _maxMB;
public:
	long GetmaxMB()
    {
        return _maxMB;
    }
	void SetmaxMB(long value)
    {
        _maxMB = value;
    }
private:
	long _currentMB;
public:
	long GetcurrentMB()
    {
        return _currentMB;
    }
	void SetcurrentMB(long value)
    {
        _currentMB = value;
    }

public:
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

private:
	wstring _apiKey;
public:
	wstring GetapiKey()
    {
        return _apiKey;
    }
	void SetapiKey(wstring value)
    {
        _apiKey = value;
    }
private:
	wstring _envInfo;
public:
	wstring GetenvInfo()
    {
        return _envInfo;
    }
	void SetenvInfo(wstring value)
    {
        _envInfo = value;
    }

public:
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

private:
	CpuInfo _cpuUsage;
public:
	CpuInfo GetcpuUsage()
    {
        return _cpuUsage;
    }
	void SetcpuUsage(CpuInfo value)
    {
        _cpuUsage = value;
    }
private:
	MemoryInfo _memoryUsage;
public:
	MemoryInfo GetmemoryUsage()
    {
        return _memoryUsage;
    }
	void SetmemoryUsage(MemoryInfo value)
    {
        _memoryUsage = value;
    }

public:
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



