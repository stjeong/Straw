import org.junit.*;

import play.mvc.*;
import static play.test.Helpers.*;
import static org.fest.assertions.Assertions.*;


public class ApplicationTest {

    @Test
    public void simpleCheck() {
        int a = 1 + 1;
        assertThat(a).isEqualTo(2);
    }

    @Test
    public void renderTemplate() {
        Content html = views.html.index.render("gg");
        assertThat(contentType(html)).isEqualTo("text/html");
        assertThat(contentAsString(html)).contains("gg");
    }


}
