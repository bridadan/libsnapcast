#include "snapcast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* hello_message_str_expected = "{\"MAC\":\"mac\",\"HostName\":\"hostname\",\"Version\":\"version\",\"ClientName\":\"client_name\",\"OS\":\"os\",\"Arch\":\"arch\",\"Instance\":1,\"ID\":\"id\",\"SnapStreamProtocolVersion\":2}";

int test_hello_message() {
    hello_message_t hello;

    hello.mac = "mac";
    hello.hostname = "hostname";
    hello.version = "version";
    hello.client_name = "client_name";
    hello.os = "os";
    hello.arch = "arch";
    hello.instance = 1;
    hello.id = "id";
    hello.protocol_version = 2;

    char *hello_message_str = hello_message_serialize(&hello);
    if (!hello_message_str) {
        printf("Failed to serialize hello message\r\n");
        return 1;
    }

    if (strcmp(hello_message_str, hello_message_str_expected)) {
        printf("Incorrect hello message serialization.\r\n");
        printf("Expected: %s\r\n", hello_message_str_expected);
        printf("Actual: %s\r\n", hello_message_str);
        free(hello_message_str);
        return 1;
    }

    free(hello_message_str);
}


int main() {
    int fail = 0;

    fail |= test_hello_message();

    if (fail) {
        printf("Some tests failed\r\n");
    } else {
        printf("All tests passed\r\n");
    }
    return 0;
}
