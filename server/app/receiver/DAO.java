package receiver;

import com.mongodb.*;

import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;

public class DAO {

    // TODO db.raw.ensureIndex({time:1}); 근데 언제 날리지?? 그리고 그룹에도 인덱스 걸어야하는데?

    public static final String TEST_GROUP = "ce3bd840-f0a7-11e3-ac10-0800200c9a66";

    public static void insertGroupIfNotExist(String groupKey) {
        BasicDBObject q = new BasicDBObject();
        q.append("key", groupKey);
        BasicDBObject o = new BasicDBObject();
        o.append("key", groupKey);
        getGroupCollection().update(q, o, true, false);
    }

    private static DBCollection getGroupCollection() {
        return getDb().getCollection("group");
    }

    public static boolean isGroupExist(String groupKey) {
        BasicDBObject ref = new BasicDBObject();
        ref.append("key", groupKey);
        return getGroupCollection().find(ref).count() == 1;
    }

    public static void removeGroupIfExist(String groupKey) {
        BasicDBObject o = new BasicDBObject();
        o.append("key", groupKey);
        getGroupCollection().remove(o);
    }

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

    public static Map<String, Map<Long, Double>> getRawsBetween(String groupKey, long startTime, long endTime) {
        // TODO add index on time
        BasicDBObject query = new BasicDBObject();
        query.append("groupKey", new BasicDBObject("$eq", groupKey));
        query.append("time", new BasicDBObject("$gte", startTime).append("$lt", endTime));

        HashMap<String, Map<Long, Double>> r = new HashMap<>();
        try (DBCursor cur = getRawCollection().find(query)) {
            while (cur.hasNext()) {
                DBObject obj = cur.next();
                double cpu = (Double) obj.get("cpu");
                String memberId = (String) obj.get("memberId");
                if (!r.containsKey(memberId))
                    r.put(memberId, new HashMap<Long, Double>());
                r.get(memberId).put((Long) obj.get("time"), cpu);
            }
        }
        return r;
    }

    private static DBCollection getRawCollection() {
        return getDb().getCollection("raw");
    }

    private static MongoClient client = null;

    private synchronized static DB getDb() {
        if (client == null) {
            try {
                client =  new MongoClient("straw.imyoyo.net", 27017);
            } catch (UnknownHostException e) {
                throw new RuntimeException(e);
            }
        }
        return client.getDB("straw");
    }

}
