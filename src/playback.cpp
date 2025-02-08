#include "pubsub_cpp/publisher.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>

#define MAX_TOPIC_LEN 64
#define MAX_DATA_LEN UDP_MAX_LEN

struct Msg {
        double time;
        uint8_t topic_len;
        char topic[MAX_TOPIC_LEN];
        uint16_t data_len;
        uint8_t data[MAX_DATA_LEN];
};

static bool read_msg(Msg& msg, FILE* fp)
{
    if ((fread(&msg.time, sizeof(msg.time), 1, fp) == 0)
        || (fread(&msg.topic_len, sizeof(msg.topic_len), 1, fp) == 0)
        || (fread(msg.topic, sizeof(char), msg.topic_len, fp) == 0)
        || (fread(&msg.data_len, sizeof(msg.data_len), 1, fp) == 0)) {
        return false;
    }
    if (msg.data_len) {
        if (fread(msg.data, sizeof(uint8_t), msg.data_len, fp) == 0) {
            return false;
        }
    }
    if ((msg.topic_len >= MAX_TOPIC_LEN) || (msg.data_len >= MAX_DATA_LEN)) {
        return false;
    }
    msg.topic[msg.topic_len] = 0;
    msg.data[msg.data_len] = 0;
    return true;
}

static void display_msg(const Msg& msg)
{
    printf("[%.3f] [%s]: ", msg.time, msg.topic);
    for (int i = 0; i < msg.data_len; i++) {
        printf("%c", msg.data[i]);
    }
    printf("\n");
}

static void publish_msg(const Msg& msg)
{
    Publisher pub(msg.topic);
    pub.send_bytes(msg.data, msg.data_len);
}

int main(int argc, char* argv[])
{
    const char* filename = argv[1];

    FILE* fp = fopen(filename, "rb");
    Msg msg;
    double last_msg_time = 0;
    while (read_msg(msg, fp)) {
        if (last_msg_time != 0) {
            int sleep_ms = (msg.time - last_msg_time) * 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        }
        last_msg_time = msg.time;
        display_msg(msg);
        publish_msg(msg);
    }
    fclose(fp);
    return 0;
}
