package receiver;

public interface Handler<T> {
    void handle(T v);
}
