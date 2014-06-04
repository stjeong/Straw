package receiver;

import org.junit.Assert;
import org.junit.Test;

public class UTF8StringReceivingUdpServerTest {

    String received = "";
    @Test
    public void testSending() throws Exception {
        UTF8StringReceivingUdpServer server = new UTF8StringReceivingUdpServer(8384, new Handler<String>() {
            @Override
            public void handle(String v) {
                received = v;
            }
        });
        server.start();
        UTF8StringSendingUdpClient.send("localhost", 8384,"ABC");
        Thread.sleep(100);
        Assert.assertEquals("ABC", received);
    }
}