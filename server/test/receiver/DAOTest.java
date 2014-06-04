package receiver;

import org.junit.Assert;
import org.junit.Test;

import java.util.List;

public class DAOTest {

    public static final String GROUP = "UNIT_TEST_GROUP";

    @Test
    public void testInsertRawAndQueryRange() throws Exception {
        DAO.clearGroup(GROUP);
        for (int i = 0; i <= 10; i++)
            DAO.insertRaw(GROUP, "A", i * 1000, i*10);
        List<Double> actual = DAO.getRawsBetween(GROUP, "A", 2000, 7000);
        Assert.assertEquals(5, actual.size());
        Assert.assertEquals(20, actual.get(0), 0);
    }
}