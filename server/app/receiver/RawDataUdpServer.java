package receiver;

import com.fasterxml.jackson.databind.JsonNode;
import org.slf4j.LoggerFactory;
import play.libs.Json;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class RawDataUdpServer {

    public static final ExecutorService EXECUTOR = Executors.newSingleThreadExecutor();

    public static void start() {
        new UTF8StringReceivingUdpServer(8282, new Handler<String>() {
            @Override
            public void handle(String v) {
                enqueInserting(v, System.currentTimeMillis());
            }
        }).start();
    }

    private static void enqueInserting(final String jsonString, final long time) {
        EXECUTOR.submit(new Runnable() {
            @Override
            public void run() {
                try {
                    processData(jsonString, time);
                } catch(RuntimeException e) {
                    LoggerFactory.getLogger(getClass()).error("", e);
                }
            }
        });
    }

    protected static void processData(String jsonString, long time) {
        JsonNode root = Json.parse(jsonString);
        String groupKey = root.get("groupKey").asText();
        double cpu = root.get("cpuUsage").get("total").asDouble();
        String id = root.get("machineId").asText();
        DAO.insertRaw(groupKey, id, time, cpu);
    }

}
