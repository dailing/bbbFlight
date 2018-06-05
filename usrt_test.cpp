#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<glog/logging.h>
#include "byteArrQueue.h"

int set_interface_attribs(int fd, int speed, int parity) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        LOG(ERROR) << "error" << errno << "from tcgetattr";
        return -1;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN] = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        LOG(ERROR) << "error" << errno;
        return -1;
    }
    return 0;
}

void set_blocking(int fd, int should_block) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        LOG(ERROR) << "error" << errno;
        return;
    }

    tty.c_cc[VMIN] = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        LOG(ERROR) << "error" << errno;
    }
}


struct position {
    float roll, pitch, yaw;
};

position decode_frame(char *frame) {

}


int main(int argc, char const *argv[]) {

    google::InitGoogleLogging(argv[0]);
    DLOG(INFO) << "FUCK";


    char file_name[] = "/dev/ttyO5";
    int file;
    if ((file = open(file_name, O_RDWR | O_NOCTTY | O_SYNC)) < 0) {
        LOG(ERROR) << "Cannot open file";
    }

    set_interface_attribs(file, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking(file, 0);                    // set no blocking

    unsigned char buf[100];
    ByteArrQueue bq(1024 * 10); // 10k queue

    while (true) {
        ssize_t n = read(file, (void *) buf, 33);
        for (int i = 0; i < n; ++i) {
            printf("%3X", buf[i]);
        }
        printf("\n");
        if (n <= 0) {
            LOG(ERROR) << "Error reading usart!";
            break;
        }
        bq.push_many(buf, (int) n);
        while (bq.length() >= 33) {
            int l = bq.read_many(buf, 2);
            if (l == 2) {
                if (!(buf[0] == 0x55 && buf[1] == 0x51)) {
                    bq.pop_many(1);
                    continue;
                }
            }
            if (bq.length() < 33) {
                LOG(ERROR) << "data Not enough";
                break;
            }

            bq.read_many(buf, 33);
            if (!(buf[11] == 0x55 and buf[12] == 0x52)) {
                LOG(ERROR) << "ERROR FRAME";
                bq.pop_many(1);
                continue;
            }
            if (!(buf[22] == 0x55 and buf[23] == 0x53)) {
                LOG(ERROR) << "ERROR FRAME" << buf[22] << buf[23];
                bq.pop_many(1);
                continue;
            }
            LOG(INFO) << "GET ONE!";
            bq.pop_many(33);
            break;
        }
    }

    close(file);
    LOG(INFO) << "END";
    return 0;
}

