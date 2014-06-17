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

            info.groupKey = "f31fc79434df8e4b7f9fd1f5bebe5b111baf8571";
            info.machineId = null;

            // quad-core
            CpuInfo cpu = new CpuInfo();
            cpu.unit.Add(15.5f);
            cpu.unit.Add(0.0f);
            cpu.unit.Add(0.0f);
            cpu.unit.Add(0.0f);

            cpu.total = (float)(15.5 * 0.25);
            info.cpuUsage = cpu;

            MemoryInfo memoy = new MemoryInfo();
            memoy.max = 8589934592; // 8 * 1024 * 1024 * 1024;
            memoy.current = 6442450944; // 6 * 1024 * 1024 * 1024;

            info.memoryUsage = memoy;

            string json = JsonConvert.SerializeObject(info);
            Trace.WriteLine(json);
        }
    }
}
