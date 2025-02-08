#include "pubsub_cpp/publisher.h"

#include <cstdio>
#include <cstring>
#include <vector>

static const char* topic = "";
static const char* msg = "";
static std::vector<uint8_t> bytes;

static void print_help()
{
    printf("Options:\n");
    printf("\t-t, --topic    Specify the topic to publish to.\n");
    printf("\t-m, --message  Specify the string to publish.\n");
    printf("\t-b, --bytes    Specify the bytes to publish.\n");
}

enum ParseState {
    ZERO,
    X,
    UPPER_NIBBLE,
    LOWER_NIBBLE,
};

static bool is_number(char chr)
{
    return chr >= '0' && chr <= '9';
}

static bool is_upper_hex(char chr)
{
    return chr >= 'A' && chr <= 'F';
}

static bool is_lower_hex(char chr)
{
    return chr >= 'a' && chr <= 'f';
}

static bool is_hex(char chr)
{
    return is_number(chr) || is_upper_hex(chr) || is_lower_hex(chr);
}

static uint8_t char_to_nibble(char chr)
{
    if (is_number(chr)) {
        return chr - '0';
    } else if (is_upper_hex(chr)) {
        return chr - 'A' + 0x0A;
    } else if (is_lower_hex(chr)) {
        return chr - 'a' + 0x0A;
    } else {
        return 0x00;
    }
}

static void save_to_bytes_vector(const char* str)
{
    ParseState state = ZERO;
    uint8_t byte;
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        switch (state) {
            case ZERO:
                if (str[i] == '0') {
                    state = X;
                }
                break;
            case X:
                if (str[i] == 'x' || str[i] == 'X') {
                    state = UPPER_NIBBLE;
                } else {
                    state = ZERO;
                }
                break;
            case UPPER_NIBBLE:
                if (is_hex(str[i])) {
                    byte = char_to_nibble(str[i]) << 4;
                    state = LOWER_NIBBLE;
                } else {
                    state = ZERO;
                }
                break;
            case LOWER_NIBBLE:
                if (is_hex(str[i])) {
                    byte += char_to_nibble(str[i]);
                    bytes.push_back(byte);
                }
                state = ZERO;
                break;
        }
    }
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
        } else if ((strcmp(argv[i], "-m") == 0) || (strcmp(argv[i], "--message") == 0)) {
            i++;
            msg = argv[i];
        } else if ((strcmp(argv[i], "-b") == 0) || (strcmp(argv[i], "--bytes") == 0)) {
            i++;
            save_to_bytes_vector(argv[i]);
        } else {
            printf("Unknown argument: \"%s\"\n", argv[i]);
            return false;
        }
    }
    return argc >= 5;
}

int main(int argc, char* argv[])
{
    if (!parse_args(argc, argv)) {
        return 1;
    }
    Publisher pub(topic);
    if (bytes.size() > 0) {
        pub.send_bytes(&bytes[0], bytes.size());
    } else {
        pub.send_string(msg);
    }
    return 0;
}
