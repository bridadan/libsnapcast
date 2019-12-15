#ifndef __SNAPCAST_H__
#define __SNAPCAST_H__

#include <stdint.h>

enum message_type {
    base = 0,
    codec_header = 1,
    wire_chunk = 2,
    server_settings = 3,
    time = 4,
    hello = 5,
    stream_tags = 6,

    first = base,
    last = stream_tags
};

typedef struct tv {
    int32_t sec;
    int32_t usec;
} tv_t;

typedef struct base_message {
    uint16_t type;
    uint16_t id;
    uint16_t refersTo;
    tv_t received;
    tv_t sent;
    uint32_t size;
} base_message_t;

/* Sample Hello message
{
    "Arch": "x86_64",
    "ClientName": "Snapclient",
    "HostName": "my_hostname",
    "ID": "00:11:22:33:44:55",
    "Instance": 1,
    "MAC": "00:11:22:33:44:55",
    "OS": "Arch Linux",
    "SnapStreamProtocolVersion": 2,
    "Version": "0.17.1"
}
*/

typedef struct hello_message {
    char *mac;
    char *hostname;
    char *version;
    char *client_name;
    char *os;
    char *arch;
    int instance;
    char *id;
    int protocol_version;
} hello_message_t;

char* hello_message_serialize(hello_message_t* msg);

#endif // __SNAPCAST_H__
