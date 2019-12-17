#include "snapcast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* hello_message_str_expected = "{\"MAC\":\"mac\",\"HostName\":\"hostname\",\"Version\":\"version\",\"ClientName\":\"client_name\",\"OS\":\"os\",\"Arch\":\"arch\",\"Instance\":1,\"ID\":\"id\",\"SnapStreamProtocolVersion\":2}";

int test_hello_message_serialize() {
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

const char* server_settings_message_json_string = "{\"bufferMs\":1000,\"latency\":20,\"muted\":false,\"volume\":100}";

int test_server_settings_message_parse() {
    server_settings_message_t ss_msg;
    int result = server_settings_message_parse(
        server_settings_message_json_string, &ss_msg
    );

    if (result) {
        printf("Failed to parse server settings message: code %d\r\n", result);
        return 1;
    }

    if (ss_msg.buffer_ms != 1000) {
        printf("Expected buffer_ms to be %d, actually was %d\r\n", 1000, ss_msg.buffer_ms);
        result = 1;
    }

    if (ss_msg.latency != 20) {
        printf("Expected latency to be %d, actually was %d\r\n", 20, ss_msg.latency);
        result = 1;
    }

    if (ss_msg.volume != 100) {
        printf("Expected volume to be %d, actually was %d\r\n", 100, ss_msg.volume);
        result = 1;
    }

    if (ss_msg.muted) {
        printf("Expected muted to be false, actually was true\r\n");
        result = 1;
    }

    return result;
}

int main() {
    int fail = 0;

    fail |= test_hello_message_serialize();
    fail |= test_server_settings_message_parse();

    if (fail) {
        printf("Some tests failed\r\n");
    } else {
        printf("All tests passed\r\n");
    }
    return 0;
}
