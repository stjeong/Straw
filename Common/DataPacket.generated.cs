


// 06/26/2014 23:20:15

namespace Kerberos.Data
{
    using System.Data;
    using System;
    using System.Collections.Generic;
    using System.Text;
    using System.IO;
    using System.Diagnostics;
    using System.Configuration;
    using System.Runtime.InteropServices;
    using System.ComponentModel;
    using System.Runtime.CompilerServices;
    using System.Runtime.Serialization;
    using System.Collections.ObjectModel;
    
    	
    
    [DataContract]
    public partial class ConnectionInfo
    			: 
    
                
    						INotifyPropertyChanged
    					
    {
        
    
        
    
    	
    	string   _address = "straw.imyoyo.net"
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public string  Address
    	{
    		get { return this._address; }
    		
    		set
    		{
    			if (this._address == value)
    			{
    				return;
    			}
    			
    			this._address = value;
    			OnPropertyChanged("Address"); 
    		}
    	}
    	
    	int   _port = 8282
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public int  Port
    	{
    		get { return this._port; }
    		
    		set
    		{
    			if (this._port == value)
    			{
    				return;
    			}
    			
    			this._port = value;
    			OnPropertyChanged("Port"); 
    		}
    	}
    	
    
    
    
    
        	[field:NonSerialized]
        	public event PropertyChangedEventHandler PropertyChanged;
    	protected virtual void OnPropertyChanged(string propertyName)
    	{
    		if (PropertyChanged == null)
    		{
    			return;
    		}
    
    		PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
    	}   
        
             
    
    	public static  class Members
    	{
    		public const string Address = "Address";
    		public const string Port = "Port";
    	}
        
    	public static  class SqlVarMembers
    	{
    		public const string Address = "@Address";
    		public const string Port = "@Port";
    	}
    }
    
    
    [DataContract]
    public partial class CpuInfo
    			: 
    
                
    						INotifyPropertyChanged
    					
    {
        
    
        
    
    	
    	ObservableCollection<float>   _unit
        
            = new ObservableCollection<float>  ()         ;
        	
        /// <exclude />
    	[DataMember]
    	public ObservableCollection<float>  unit
    	{
    		get { return this._unit; }
    		
    		set
    		{
    			if (this._unit == value)
    			{
    				return;
    			}
    			
    			this._unit = value;
    			OnPropertyChanged("unit"); 
    		}
    	}
    	
    	float   _total
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public float  total
    	{
    		get { return this._total; }
    		
    		set
    		{
    			if (this._total == value)
    			{
    				return;
    			}
    			
    			this._total = value;
    			OnPropertyChanged("total"); 
    		}
    	}
    	
    
    
    
    
        	[field:NonSerialized]
        	public event PropertyChangedEventHandler PropertyChanged;
    	protected virtual void OnPropertyChanged(string propertyName)
    	{
    		if (PropertyChanged == null)
    		{
    			return;
    		}
    
    		PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
    	}   
        
             
    
    	public static  class Members
    	{
    		public const string unit = "unit";
    		public const string total = "total";
    	}
        
    	public static  class SqlVarMembers
    	{
    		public const string unit = "@unit";
    		public const string total = "@total";
    	}
    }
    
    
    [DataContract]
    public partial class MemoryInfo
    			: 
    
                
    						INotifyPropertyChanged
    					
    {
        
    
        
    
    	
    	long   _max
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public long  max
    	{
    		get { return this._max; }
    		
    		set
    		{
    			if (this._max == value)
    			{
    				return;
    			}
    			
    			this._max = value;
    			OnPropertyChanged("max"); 
    		}
    	}
    	
    	long   _current
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public long  current
    	{
    		get { return this._current; }
    		
    		set
    		{
    			if (this._current == value)
    			{
    				return;
    			}
    			
    			this._current = value;
    			OnPropertyChanged("current"); 
    		}
    	}
    	
    
    
    
    
        	[field:NonSerialized]
        	public event PropertyChangedEventHandler PropertyChanged;
    	protected virtual void OnPropertyChanged(string propertyName)
    	{
    		if (PropertyChanged == null)
    		{
    			return;
    		}
    
    		PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
    	}   
        
             
    
    	public static  class Members
    	{
    		public const string max = "max";
    		public const string current = "current";
    	}
        
    	public static  class SqlVarMembers
    	{
    		public const string max = "@max";
    		public const string current = "@current";
    	}
    }
    
    
    [DataContract]
    public partial class DiskInfo
    			: 
    
                
    						INotifyPropertyChanged
    					
    {
        
    
        
    
    	
    	string   _name
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public string  name
    	{
    		get { return this._name; }
    		
    		set
    		{
    			if (this._name == value)
    			{
    				return;
    			}
    			
    			this._name = value;
    			OnPropertyChanged("name"); 
    		}
    	}
    	
    	long   _size
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public long  size
    	{
    		get { return this._size; }
    		
    		set
    		{
    			if (this._size == value)
    			{
    				return;
    			}
    			
    			this._size = value;
    			OnPropertyChanged("size"); 
    		}
    	}
    	
    	long   _current
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public long  current
    	{
    		get { return this._current; }
    		
    		set
    		{
    			if (this._current == value)
    			{
    				return;
    			}
    			
    			this._current = value;
    			OnPropertyChanged("current"); 
    		}
    	}
    	
    
    
    
    
        	[field:NonSerialized]
        	public event PropertyChangedEventHandler PropertyChanged;
    	protected virtual void OnPropertyChanged(string propertyName)
    	{
    		if (PropertyChanged == null)
    		{
    			return;
    		}
    
    		PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
    	}   
        
             
    
    	public static  class Members
    	{
    		public const string name = "name";
    		public const string size = "size";
    		public const string current = "current";
    	}
        
    	public static  class SqlVarMembers
    	{
    		public const string name = "@name";
    		public const string size = "@size";
    		public const string current = "@current";
    	}
    }
    
    
    [DataContract]
    public partial class PacketBase
    			: 
    
                
    						INotifyPropertyChanged
    					
    {
        
    
        
    
    	
    	string   _groupKey
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public string  groupKey
    	{
    		get { return this._groupKey; }
    		
    		set
    		{
    			if (this._groupKey == value)
    			{
    				return;
    			}
    			
    			this._groupKey = value;
    			OnPropertyChanged("groupKey"); 
    		}
    	}
    	
    	string   _machineId
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public string  machineId
    	{
    		get { return this._machineId; }
    		
    		set
    		{
    			if (this._machineId == value)
    			{
    				return;
    			}
    			
    			this._machineId = value;
    			OnPropertyChanged("machineId"); 
    		}
    	}
    	
    
    
    
    
        	[field:NonSerialized]
        	public event PropertyChangedEventHandler PropertyChanged;
    	protected virtual void OnPropertyChanged(string propertyName)
    	{
    		if (PropertyChanged == null)
    		{
    			return;
    		}
    
    		PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
    	}   
        
             
    
    	public static  class Members
    	{
    		public const string groupKey = "groupKey";
    		public const string machineId = "machineId";
    	}
        
    	public static  class SqlVarMembers
    	{
    		public const string groupKey = "@groupKey";
    		public const string machineId = "@machineId";
    	}
    }
    
    
    [DataContract]
    public partial class SystemInfo
    			: 
    
                			    PacketBase
    			
    					
    {
        
    
        
    
    	
    	CpuInfo   _cpuUsage
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public CpuInfo  cpuUsage
    	{
    		get { return this._cpuUsage; }
    		
    		set
    		{
    			if (this._cpuUsage == value)
    			{
    				return;
    			}
    			
    			this._cpuUsage = value;
    			OnPropertyChanged("cpuUsage"); 
    		}
    	}
    	
    	MemoryInfo   _memoryUsage
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public MemoryInfo  memoryUsage
    	{
    		get { return this._memoryUsage; }
    		
    		set
    		{
    			if (this._memoryUsage == value)
    			{
    				return;
    			}
    			
    			this._memoryUsage = value;
    			OnPropertyChanged("memoryUsage"); 
    		}
    	}
    	
    
    
    
    
                 
    
    	public static  new  class Members
    	{
    		public const string groupKey = "groupKey";
    		public const string machineId = "machineId";
    		public const string cpuUsage = "cpuUsage";
    		public const string memoryUsage = "memoryUsage";
    	}
        
    	public static  new  class SqlVarMembers
    	{
    		public const string groupKey = "groupKey";
    		public const string machineId = "machineId";
    		public const string cpuUsage = "@cpuUsage";
    		public const string memoryUsage = "@memoryUsage";
    	}
    }
    
    
    [DataContract]
    public partial class StorageInfo
    			: 
    
                			    PacketBase
    			
    					
    {
        
    
        
    
    	
    	ObservableCollection<DiskInfo>   _disk
        
            = new ObservableCollection<DiskInfo>  ()         ;
        	
        /// <exclude />
    	[DataMember]
    	public ObservableCollection<DiskInfo>  disk
    	{
    		get { return this._disk; }
    		
    		set
    		{
    			if (this._disk == value)
    			{
    				return;
    			}
    			
    			this._disk = value;
    			OnPropertyChanged("disk"); 
    		}
    	}
    	
    
    
    
    
                 
    
    	public static  new  class Members
    	{
    		public const string groupKey = "groupKey";
    		public const string machineId = "machineId";
    		public const string disk = "disk";
    	}
        
    	public static  new  class SqlVarMembers
    	{
    		public const string groupKey = "groupKey";
    		public const string machineId = "machineId";
    		public const string disk = "@disk";
    	}
    }
    
}

