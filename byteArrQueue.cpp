#include "byteArrQueue.h"
#include <stdio.h>
#include <glog/logging.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

using std::min;

ByteArrQueue::ByteArrQueue(int len) : tmp(false) {
    LOG(INFO) << "init queue of size:" << len;
    void *data_prt = malloc(len);
    if (data_prt == NULL) {
        LOG(ERROR) << "No Memory!";
        return;
    }
    this->len = len;
    this->data_ptr = data_prt;
    this->front = 0;
    this->end = 0;
}

ByteArrQueue::ByteArrQueue(void *data_ptr, int len) : tmp(true) {
    this->data_ptr = data_ptr;
    this->len = len;
    this->front = 0;
    this->end = 0;
}

ByteArrQueue::~ByteArrQueue() {
    if (!this->tmp) free(this->data_ptr);
}

int ByteArrQueue::size() {
    return this->len;
}

int ByteArrQueue::length() {
    return this->end - this->front;
}

int ByteArrQueue::get_free() {
    return this->len - this->end + this->front;
}

int ByteArrQueue::read_many(void *a, int len) {
    len = min(this->length(), len);
    // read first block
    int l = min(len, this->len - this->front);
    memcpy(a, (char *) this->data_ptr + this->front, l);
    DLOG(INFO) << "Reading : @" << this->front << " LEN:" << l;
    // read second block
    if (l < len) {
        memcpy((char *) a + l, (char *) this->data_ptr, len - l);
        DLOG(INFO) << "Reading : @" << 0 << " LEN:" << len - l;
    }
    return len;
}

int ByteArrQueue::pop_many(void *a, int len) {
    DLOG(INFO) << "FRONT:" << this->front << " END:" << this->end;
    len = this->read_many(a, len);
    this->front += len;
    if (this->front >= this->len) {
        this->front -= this->len;
        this->end -= this->len;
    }
    return len;
}

int ByteArrQueue::pop_many(int len) {
    this->front += len;
    if (this->front >= this->len) {
        this->front -= this->len;
        this->end -= this->len;
    }
    return len;
}

int ByteArrQueue::push_many(const void *a, int len) {
    DLOG(INFO) << "FRONT:" << this->front << " END:" << this->end;
    if (this->get_free() < len) {
        return 0;
    }
    if (this->end >= this->len) {
        memcpy((char *) this->data_ptr + this->end - this->len, a, len);
        DLOG(INFO) << "Writing : @" << this->end - this->len << " LEN:" << len;
    } else { // copy the first one
        int l = min(this->len - this->end, len);
        memcpy((char *) this->data_ptr + this->end, a, l);
        DLOG(INFO) << "Writing : @" << this->end << " LEN:" << l;
        if (l < len) { // copy the second one
            memcpy(this->data_ptr, (char *) a + l, len - l);
            DLOG(INFO) << "Writing : @" << 0 << " LEN:" << len - l;
        }
    }
    this->end += len; // update position
    return len;
}

void *ByteArrQueue::get_front_ptr() {
    if (this->end < this->len) {
        return (char *) this->data_ptr + this->end;
    } else {
        return (char *) this->data_ptr + this->end - this->len;
    }
}

ByteArrQueue ByteArrQueue::reserve_space(int len) {
    if (len > this->get_free()) {
        return ByteArrQueue(NULL, 0);
    }
    if (this->end < this->len) {
        int l = min(this->len - this->end, len);
        this->end += l;
        return ByteArrQueue((char *) this->data_ptr + this->end - l, l);
    } else {
        int l = min(this->len - this->end + this->front, len);
        this->end += l;
        return ByteArrQueue((char *) this->data_ptr + this->end - l - this->len, l);
    }
}

void ByteArrQueue::clear() {
    this->front = 0;
    this->end = 0;
}
