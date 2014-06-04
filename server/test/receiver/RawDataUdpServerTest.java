package receiver;

import org.junit.Assert;
import org.junit.Test;

import java.util.List;

public class RawDataUdpServerTest {
    @Test
    public void testSending() throws Exception {
        DAO.clearGroup("TEST_G");
        RawDataUdpServer.processData("{\"CpuUsage\":{\"Total\": 3.87}, \"ApiKey\":\"TEST_G\"}", 10 * 1000);
        Thread.sleep(100);
        List<Double> list = DAO.getRawsBetween("TEST_G", "my pc", 0, 100 * 1000);
        Assert.assertEquals(3.87, (Double)list.get(0), 0);
    }

}
