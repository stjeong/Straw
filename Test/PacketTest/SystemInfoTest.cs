using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Kerberos.Data;
using System.Text;
using Newtonsoft.Json;
using System.Diagnostics;

namespace PacketTest
{
    [TestClass]
    public class SystemInfoTest
    {
        [TestMethod]
        public void SerializeTest()
        {
            SystemInfo info = new SystemInfo();

            info.ApiKey = "f31fc79434df8e4b7f9fd1f5bebe5b111baf8571";
            info.EnvInfo = null;

            // quad-core
            CpuInfo cpu = new CpuInfo();
            cpu.Unit.Add(15.5f);
            cpu.Unit.Add(0.0f);
            cpu.Unit.Add(0.0f);
            cpu.Unit.Add(0.0f);

            cpu.Total = (float)(15.5 * 0.25);
            info.CpuUsage = cpu;

            MemoryInfo memoy = new MemoryInfo();
            memoy.MaxMB = 8589934592; // 8 * 1024 * 1024 * 1024;
            memoy.CurrentMB = 6442450944; // 6 * 1024 * 1024 * 1024;

            info.MemoryUsage = memoy;

            string json = JsonConvert.SerializeObject(info);
            Trace.WriteLine(json);
        }
    }
}
