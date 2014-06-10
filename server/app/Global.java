import com.fasterxml.jackson.databind.ser.impl.PropertySerializerMap;
import play.*;
import play.Application;
import receiver.DAO;
import receiver.RawDataUdpServer;

import java.util.Random;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class Global extends GlobalSettings {

    @Override
    public void onStart(Application app) {
        DAO.insertGroupIfNotExist(DAO.TEST_GROUP);
        RawDataUdpServer.start();
        Executors.newSingleThreadScheduledExecutor().scheduleAtFixedRate(new Runnable() {
            Random random = new Random();
            @Override
            public void run() {
                double cpu = random.nextDouble();
                DAO.insertRaw(DAO.TEST_GROUP, "random-ghost", System.currentTimeMillis(), cpu);
            }
        }, 1, 1, TimeUnit.SECONDS);
    }

    @Override
    public void onStop(Application app) {
        super.onStop(app);
    }
}
