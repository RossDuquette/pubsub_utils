#include "pubsub_cpp/subscriber.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

static const char* topic = "";
static const char* filename = "";
static const auto start_time = std::chrono::high_resolution_clock::now();

static void print_help()
{
    printf("Options:\n");
    printf("\t-t, --topic     Specify the topic to record.\n");
    printf("\t-f, --filename  Specify the filename to save to.\n");
}

static bool parse_args(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            print_help();
            return false;
        } else if ((strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "--topic") == 0)) {
            i++;
            topic = argv[i];
        } else if ((strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--filename") == 0)) {
            i++;
            filename = argv[i];
        } else {
            printf("Unknown argument: \"%s\"\n", argv[i]);
            return false;
        }
    }
    return argc >= 5;
}

static double get_run_time_sec()
{
    auto time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(time - start_time);
    return duration.count() / 1000.0;
}

int main(int argc, char* argv[])
{
    if (!parse_args(argc, argv)) {
        return 1;
    }
    Subscriber sub(topic);
    FILE* fp = fopen(filename, "wb");
    while (true) {
        if (sub.recv()) {
            double time = get_run_time_sec();
            fwrite(&time, sizeof(time), 1, fp);
            uint8_t topic_len = strlen(sub.get_recv_topic());
            fwrite(&topic_len, sizeof(topic_len), 1, fp);
            fwrite(sub.get_recv_topic(), sizeof(char), topic_len, fp);
            uint16_t len = sub.get_len();
            fwrite(&len, sizeof(len), 1, fp);
            fwrite(sub.get_bytes(), sizeof(uint8_t), len, fp);
            fflush(fp);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    fclose(fp);
    return 0;
}
