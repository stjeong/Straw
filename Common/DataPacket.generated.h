

#pragma once


// 06/26/2014 23:20:15

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
	wstring _address = L"www.naver.com";
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
	int _port = 8282;
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
private:
	float _total;
public:
	float Gettotal()
    {
        return _total;
    }
	void Settotal(float value)
    {
        _total = value;
    }

public:
	class Members
	{
    public:
		static wstring unit;
		static wstring total;
	};
    
	class SqlVarMembers
	{
    public:
		static wstring unit;
		static wstring total;
	};
};


class MemoryInfo
		
{
public:

    MemoryInfo()
    {
    }

private:
	long _max;
public:
	long Getmax()
    {
        return _max;
    }
	void Setmax(long value)
    {
        _max = value;
    }
private:
	long _current;
public:
	long Getcurrent()
    {
        return _current;
    }
	void Setcurrent(long value)
    {
        _current = value;
    }

public:
	class Members
	{
    public:
		static wstring max;
		static wstring current;
	};
    
	class SqlVarMembers
	{
    public:
		static wstring max;
		static wstring current;
	};
};


class DiskInfo
		
{
public:

    DiskInfo()
    {
    }

private:
	wstring _name;
public:
	wstring Getname()
    {
        return _name;
    }
	void Setname(wstring value)
    {
        _name = value;
    }
private:
	long _size;
public:
	long Getsize()
    {
        return _size;
    }
	void Setsize(long value)
    {
        _size = value;
    }
private:
	long _current;
public:
	long Getcurrent()
    {
        return _current;
    }
	void Setcurrent(long value)
    {
        _current = value;
    }

public:
	class Members
	{
    public:
		static wstring name;
		static wstring size;
		static wstring current;
	};
    
	class SqlVarMembers
	{
    public:
		static wstring name;
		static wstring size;
		static wstring current;
	};
};


class PacketBase
		
{
public:

    PacketBase()
    {
    }

private:
	wstring _groupKey;
public:
	wstring GetgroupKey()
    {
        return _groupKey;
    }
	void SetgroupKey(wstring value)
    {
        _groupKey = value;
    }
private:
	wstring _machineId;
public:
	wstring GetmachineId()
    {
        return _machineId;
    }
	void SetmachineId(wstring value)
    {
        _machineId = value;
    }

public:
	class Members
	{
    public:
		static wstring groupKey;
		static wstring machineId;
	};
    
	class SqlVarMembers
	{
    public:
		static wstring groupKey;
		static wstring machineId;
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
		wstring groupKey;
		wstring machineId;
		static wstring cpuUsage;
		static wstring memoryUsage;
	};
    
	class SqlVarMembers
	{
    public:
		wstring groupKey;
		wstring machineId;
		static wstring cpuUsage;
		static wstring memoryUsage;
	};
};


class StorageInfo
			: 

            			    PacketBase
			
					
{
public:

    StorageInfo()
    {
    }

private:
	vector<DiskInfo> _disk;
public:
	vector<DiskInfo> Getdisk()
    {
        return _disk;
    }
	void Setdisk(vector<DiskInfo> value)
    {
        _disk = value;
    }

public:
	class Members
	{
    public:
		wstring groupKey;
		wstring machineId;
		static wstring disk;
	};
    
	class SqlVarMembers
	{
    public:
		wstring groupKey;
		wstring machineId;
		static wstring disk;
	};
};



