package receiver;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class UTF8StringSendingUdpClient {
    public static void send(String addressName, int port, String jsonString) throws IOException {
        DatagramSocket clientSocket = new DatagramSocket();
        try {
            InetAddress address = InetAddress.getByName(addressName);
            byte[] bytes = jsonString.getBytes("UTF-8");
            DatagramPacket sendPacket = new DatagramPacket(bytes, bytes.length, address, port);
            clientSocket.send(sendPacket);
        } finally {
            clientSocket.close();
        }
    }
}
