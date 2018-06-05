#ifndef __ByteArrQueue__
#define __ByteArrQueue__ value


class ByteArrQueue;

class ByteArrQueue {
private:
    int front;
    int end;
    int len;
    void *data_ptr;
    bool tmp;
public:
    ByteArrQueue(int len = 1024)  ;

    ByteArrQueue(void *data_ptr, int len);

    ~ByteArrQueue();

    int get_free();

    int length();

    int size();

    int read_many(void *a, int len);

    int pop_many(void *a, int len);
    int pop_many(int len);

    int push_many(const void *a, int len);

    void *get_front_ptr();

    ByteArrQueue reserve_space(int len);

    void clear();

};



#endif
