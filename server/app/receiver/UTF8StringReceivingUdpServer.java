package receiver;

import com.fasterxml.jackson.databind.JsonNode;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import play.libs.Json;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.concurrent.Executors;

public class UTF8StringReceivingUdpServer {

    public static final Logger LOGGER = LoggerFactory.getLogger(UTF8StringReceivingUdpServer.class);

    private final int port;
    private final Handler<String> handler;

    public UTF8StringReceivingUdpServer(int port, Handler<String> handler) {
        this.port = port;
        this.handler = handler;
    }

    private Thread loopThread;

    public synchronized void start() {
        assert loopThread == null;
        loopThread = new Thread(new Runnable() {
            @Override
            public void run() {
                doLoop();
            }
        });
        loopThread.start();
    }

    private void doLoop() {
        DatagramSocket serverSocket;
        try {
            serverSocket = new DatagramSocket(port);
        } catch (SocketException e) {
            LOGGER.warn("", e);
            return;
        }
        byte[] buffer = new byte[64*1024];
        LOGGER.info("Start UDP Server");
        while (!Thread.currentThread().isInterrupted()) {
            try {
                final String jsonString = readStringFromOnePacket(serverSocket, buffer);
                final long receivedTime = System.currentTimeMillis();
                handler.handle(jsonString);
            } catch (IOException e) {
                LOGGER.warn("", e);
            } catch(Exception e) {
                LOGGER.error("", e);
            }
        }
    }

    private static String readStringFromOnePacket(DatagramSocket serverSocket, byte[] buffer) throws IOException {
        DatagramPacket receivePacket = new DatagramPacket(buffer, buffer.length);
        serverSocket.receive(receivePacket);
        return new String(receivePacket.getData(), 0, receivePacket.getLength(), "UTF-8");
    }

}
