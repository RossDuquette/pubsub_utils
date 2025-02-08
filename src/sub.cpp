#include "pubsub_cpp/subscriber.h"

#include <cstdio>
#include <cstring>
#include <thread>

static const char* topic = "";
static bool print_bytes = false;

static void print_help()
{
    printf("Options:\n");
    printf("\t-t, --topic    Specify the topic to subscribe to.\n");
    printf("\t-b, --bytes    Print the hex notation of recv packets.\n");
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
        } else if ((strcmp(argv[i], "-b") == 0) || (strcmp(argv[i], "--bytes") == 0)) {
            print_bytes = true;
        } else {
            printf("Unknown argument: \"%s\"\n", argv[i]);
            return false;
        }
    }
    return argc >= 3;
}

int main(int argc, char* argv[])
{
    if (!parse_args(argc, argv)) {
        return 1;
    }
    Subscriber sub(topic);
    while (true) {
        if (sub.recv()) {
            if (print_bytes) {
                printf("[%s]:", sub.get_recv_topic());
                for (int i = 0; i < sub.get_len(); i++) {
                    printf(" 0x%02x", sub.get_bytes()[i]);
                }
                printf("\n");
            } else {
                printf("[%s]: %s\n", sub.get_recv_topic(), sub.get_string());
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}
