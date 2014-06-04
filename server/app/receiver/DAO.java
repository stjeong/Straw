package receiver;

import com.mongodb.*;

import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;

public class DAO {

    public static void insertRaw(String groupKey, String memberId, long time, double cpu) {
        BasicDBObject doc = new BasicDBObject();
        doc.append("time", time);
        doc.append("groupKey", groupKey);
        doc.append("memberId", memberId);
        doc.append("cpu", cpu);
        getRawCollection().insert(doc);
    }

    public static void clearGroup(String groupKey) {
        getRawCollection().remove(new BasicDBObject("groupKey", new BasicDBObject("$eq", groupKey)));
    }

    public static List<Double> getRawsBetween(String groupKey, String memberId, long startTime, long endTime) {
        BasicDBObject query = new BasicDBObject();
        query.append("groupKey", new BasicDBObject("$eq", groupKey));
        query.append("memberId", new BasicDBObject("$eq", memberId));
        query.append("time", new BasicDBObject("$gte", startTime).append("$lt", endTime));

        ArrayList<Double> r = new ArrayList<>();
        DBCursor cur = getRawCollection().find(query);
        try {
            while(cur.hasNext()) {
                DBObject obj = cur.next();
                double cpu = (Double)obj.get("cpu");
                r.add(cpu);
            }
        } finally {
            cur.close();
        }
        return r;
    }

    private static DBCollection getRawCollection() {
        return getDb().getCollection("raw");
    }

    private static DB getDb() {
        try {
            MongoClient result = new MongoClient("straw.imyoyo.net", 27017);
            return result.getDB("straw");
        } catch (UnknownHostException e) {
            throw new RuntimeException(e);
        }
    }

}
