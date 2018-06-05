//
// Created by d on 6/4/18.
//

#include <limits.h>
#include "gtest/gtest.h"
#include "byteArrQueue.h"
#include "glog/logging.h"

using std::min;
using std::rand;

class ByteArrQueueTest : public ::testing::Test {
protected:
    ByteArrQueue bq;

    virtual void SetUp() {
        srand(time(NULL));
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test
        // (right before the destructor).
    }

public:
    ByteArrQueueTest() : bq(1024) {

    }
};

TEST_F(ByteArrQueueTest, test_push_pop) {
    unsigned char buf[100 * 1024];
    unsigned char tmp[1024];
    int a = 0, b = 0;
    for (int i = 0; i < sizeof(buf); ++i) {
        buf[b + i] = (unsigned char) rand();
    }
    while (b < sizeof(buf)) {
        // push
        int delta = min((int) sizeof(buf), this->bq.size() - b + a);
        delta = min(delta, rand() % 20);
        DLOG(INFO) << "PUSH " << a << " " << b << " " << delta;
        ASSERT_EQ(this->bq.push_many(buf + b, delta), delta);
        b += delta;
        // pop
        delta = min(b - a, rand() % 20);
        DLOG(INFO) << "POP  " << a << " " << b << " " << delta;
        ASSERT_EQ(this->bq.pop_many(tmp, delta), delta);
        for (int j = 0; j < delta; ++j) {
            ASSERT_EQ(buf[a + j], tmp[j]) << "Current index is:" << j;
        }
        a += delta;
    }
}

TEST_F(ByteArrQueueTest, reserve_space){
    this->bq.clear();
    char tmp[6];
    ASSERT_EQ(this->bq.push_many("123456",6),6);
    ASSERT_EQ(this->bq.reserve_space(bq.size()-6).size(), bq.size()-6);
    bq.clear();
    ASSERT_EQ(this->bq.push_many("123456",6),6);
    ASSERT_EQ(this->bq.pop_many(tmp,6),6);
    ASSERT_EQ(this->bq.reserve_space(bq.size()).size(), bq.size()-6);
    ASSERT_EQ(this->bq.reserve_space(6).size(), 6);
}

TEST_F(ByteArrQueueTest, reserve_data_consistency){
    unsigned char buf[100 * 1024];
    unsigned char tmp[1024];
    int a = 0, b = 0;
    for (int i = 0; i < sizeof(buf); ++i) {
        buf[b + i] = (unsigned char) rand();
    }
    while (b < sizeof(buf)) {
        // push
        int delta = min((int) sizeof(buf), this->bq.size() - b + a);
        delta = min(delta, rand() % 20);
        ByteArrQueue bq = this->bq.reserve_space(delta);
        delta = bq.size();
        DLOG(INFO) << "PUSH " << a << " " << b << " " << delta;
        ASSERT_EQ(bq.push_many(buf + b, delta), delta);
        b += delta;
        // pop
        delta = min(b - a, rand() % 20);
        DLOG(INFO) << "POP  " << a << " " << b << " " << delta;
        ASSERT_EQ(this->bq.pop_many(tmp, delta), delta);
        for (int j = 0; j < delta; ++j) {
            ASSERT_EQ(buf[a + j], tmp[j]) << "Current index is:" << j;
        }
        a += delta;
    }
}