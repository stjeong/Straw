


// 2014-05-25 오후 2:21:39

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
    public partial class CpuInfo
    			: 
    
                
    						INotifyPropertyChanged
    					
    {
        
    
        
    
    	
    	ObservableCollection<float>   _unit
        
            = new ObservableCollection<float>  ()         ;
        	
        /// <exclude />
    	[DataMember]
    	public ObservableCollection<float>  Unit
    	{
    		get { return this._unit; }
    		
    		set
    		{
    			if (this._unit == value)
    			{
    				return;
    			}
    			
    			this._unit = value;
    			OnPropertyChanged("Unit"); 
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
    		public const string Unit = "Unit";
    	}
        
    	public static  class SqlVarMembers
    	{
    		public const string Unit = "@Unit";
    	}
    }
    
    
    [DataContract]
    public partial class MemoryInfo
    			: 
    
                
    						INotifyPropertyChanged
    					
    {
        
    
        
    
    	
    	long   _maxMB
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public long  MaxMB
    	{
    		get { return this._maxMB; }
    		
    		set
    		{
    			if (this._maxMB == value)
    			{
    				return;
    			}
    			
    			this._maxMB = value;
    			OnPropertyChanged("MaxMB"); 
    		}
    	}
    	
    	long   _currentMB
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public long  CurrentMB
    	{
    		get { return this._currentMB; }
    		
    		set
    		{
    			if (this._currentMB == value)
    			{
    				return;
    			}
    			
    			this._currentMB = value;
    			OnPropertyChanged("CurrentMB"); 
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
    		public const string MaxMB = "MaxMB";
    		public const string CurrentMB = "CurrentMB";
    	}
        
    	public static  class SqlVarMembers
    	{
    		public const string MaxMB = "@MaxMB";
    		public const string CurrentMB = "@CurrentMB";
    	}
    }
    
    
    [DataContract]
    public partial class PacketBase
    			: 
    
                
    						INotifyPropertyChanged
    					
    {
        
    
        
    
    	
    	string   _apiKey
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public string  ApiKey
    	{
    		get { return this._apiKey; }
    		
    		set
    		{
    			if (this._apiKey == value)
    			{
    				return;
    			}
    			
    			this._apiKey = value;
    			OnPropertyChanged("ApiKey"); 
    		}
    	}
    	
    	string   _envInfo
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public string  EnvInfo
    	{
    		get { return this._envInfo; }
    		
    		set
    		{
    			if (this._envInfo == value)
    			{
    				return;
    			}
    			
    			this._envInfo = value;
    			OnPropertyChanged("EnvInfo"); 
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
    		public const string ApiKey = "ApiKey";
    		public const string EnvInfo = "EnvInfo";
    	}
        
    	public static  class SqlVarMembers
    	{
    		public const string ApiKey = "@ApiKey";
    		public const string EnvInfo = "@EnvInfo";
    	}
    }
    
    
    [DataContract]
    public partial class SystemInfo
    			: 
    
                			    PacketBase
    			
    					
    {
        
    
        
    
    	
    	ObservableCollection<CpuInfo>   _cpuUsage
        
            = new ObservableCollection<CpuInfo>  ()         ;
        	
        /// <exclude />
    	[DataMember]
    	public ObservableCollection<CpuInfo>  CpuUsage
    	{
    		get { return this._cpuUsage; }
    		
    		set
    		{
    			if (this._cpuUsage == value)
    			{
    				return;
    			}
    			
    			this._cpuUsage = value;
    			OnPropertyChanged("CpuUsage"); 
    		}
    	}
    	
    	MemoryInfo   _memoryUsage
        
            ;
        	
        /// <exclude />
    	[DataMember]
    	public MemoryInfo  MemoryUsage
    	{
    		get { return this._memoryUsage; }
    		
    		set
    		{
    			if (this._memoryUsage == value)
    			{
    				return;
    			}
    			
    			this._memoryUsage = value;
    			OnPropertyChanged("MemoryUsage"); 
    		}
    	}
    	
    
    
    
    
                 
    
    	public static  new  class Members
    	{
    		public const string ApiKey = "ApiKey";
    		public const string EnvInfo = "EnvInfo";
    		public const string CpuUsage = "CpuUsage";
    		public const string MemoryUsage = "MemoryUsage";
    	}
        
    	public static  new  class SqlVarMembers
    	{
    		public const string ApiKey = "ApiKey";
    		public const string EnvInfo = "EnvInfo";
    		public const string CpuUsage = "@CpuUsage";
    		public const string MemoryUsage = "@MemoryUsage";
    	}
    }
    
}

