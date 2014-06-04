import play.*;
import play.Application;
import receiver.RawDataUdpServer;
import receiver.UTF8StringReceivingUdpServer;

public class Global extends GlobalSettings {

    @Override
    public void onStart(Application app) {
        RawDataUdpServer.start();
    }

    @Override
    public void onStop(Application app) {
        super.onStop(app);
    }
}
