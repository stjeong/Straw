package receiver;

import org.junit.Assert;
import org.junit.Test;

import java.util.List;
import java.util.Map;

public class RawDataUdpServerTest {
    @Test
    public void testSending() throws Exception {
        DAO.clearGroup("TEST_G");
        RawDataUdpServer.processData("{\"CpuUsage\":{\"Total\": 3.87}, \"ApiKey\":\"TEST_G\"}", 10 * 1000);
        Thread.sleep(100);
        Map<String, Map<Long, Double>> map = DAO.getRawsBetween("TEST_G", 0, 100 * 1000);
        Assert.assertEquals(3.87, (Double)map.get("my pc").get(100*1000), 0);
    }

}
