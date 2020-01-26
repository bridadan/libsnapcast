#include "snapcast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint8_t base_message_serialized[] = {
    // type
    0x05, 0x00,
    // id
    0x01, 0x00,
    // refersTo
    0x34, 0x12,
    // received.sec
    0xEF, 0xBE, 0xAD, 0xDE,
    // received.usec
    0x0D, 0xD0, 0x37, 0x13,
    // sent.sec
    0x78, 0x56, 0x34, 0x12,
    // sent.usec
    0x21, 0x43, 0x65, 0x87,
    // size
    0x33, 0x22, 0x11, 0x00,
};

base_message_t base_message_deserialized = {
    SNAPCAST_MESSAGE_HELLO,
    0x1,
    0x1234,
    { 0xDEADBEEF, 0x1337D00D },
    { 0x12345678, 0x87654321 },
    0x00112233,
};

void print_buffer(const char* buffer, size_t size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%02x", buffer[i] & 0xff);
    }
}

int test_base_message_serialize() {
    char base_message_buffer[BASE_MESSAGE_SIZE];

    int result = base_message_serialize(
        &base_message_deserialized,
        base_message_buffer,
        BASE_MESSAGE_SIZE
    );

    if (result) {
        printf("Failed to serialize base message\r\n");
        return 1;
    }

    result = memcmp(
        base_message_buffer,
        base_message_serialized,
        BASE_MESSAGE_SIZE
    );

    if (result) {
        printf("Serialized base message did not match expected value\r\n");
        printf("Expected: ");
        print_buffer(base_message_serialized, BASE_MESSAGE_SIZE);
        printf("\r\nActual:   ");
        print_buffer(base_message_buffer, BASE_MESSAGE_SIZE);
        printf("\r\n");
        return 1;
    }

    return 0;
}

int test_base_message_deserialize() {
    base_message_t base_message;

    int result = base_message_deserialize(&base_message, base_message_serialized, BASE_MESSAGE_SIZE);
    if (result) {
        printf("Failed to deserialize base message\r\n");
        return 1;
    }

    if (base_message.type != base_message_deserialized.type) {
        printf(
            "Expected type to be %u, actually was %u\r\n",
            base_message_deserialized.type,
            base_message.type
        );
        result = 1;
    }

    if (base_message.id != base_message_deserialized.id) {
        printf(
            "Expected id to be %u, actually was %u\r\n",
            base_message_deserialized.id,
            base_message.id
        );
        result = 1;
    }

    if (base_message.refersTo != base_message_deserialized.refersTo) {
        printf(
            "Expected refersTo to be %u, actually was %u\r\n",
            base_message_deserialized.refersTo,
            base_message.refersTo
        );
        result = 1;
    }

    if (base_message.received.sec != base_message_deserialized.received.sec) {
        printf(
            "Expected received.sec to be %d, actually was %d\r\n",
            base_message_deserialized.received.sec,
            base_message.received.sec
        );
        result = 1;
    }

    if (base_message.received.usec != base_message_deserialized.received.usec) {
        printf(
            "Expected received.usec to be %d, actually was %d\r\n",
            base_message_deserialized.received.usec,
            base_message.received.usec
        );
        result = 1;
    }

    if (base_message.sent.sec != base_message_deserialized.sent.sec) {
        printf(
            "Expected sent.sec to be %d, actually was %d\r\n",
            base_message_deserialized.sent.sec,
            base_message.sent.sec
        );
        result = 1;
    }

    if (base_message.sent.usec != base_message_deserialized.sent.usec) {
        printf(
            "Expected sent.usec to be %d, actually was %d\r\n",
            base_message_deserialized.sent.usec,
            base_message.sent.usec
        );
        result = 1;
    }

    if (base_message.size != base_message_deserialized.size) {
        printf(
            "Expected size to be %u, actually was %u\r\n",
            base_message_deserialized.size,
            base_message.size
        );
        result = 1;
    }

    return result;
}

const char* hello_message_str_json = "{\"MAC\":\"mac\",\"HostName\":\"hostname\",\"Version\":\"version\",\"ClientName\":\"client_name\",\"OS\":\"os\",\"Arch\":\"arch\",\"Instance\":1,\"ID\":\"id\",\"SnapStreamProtocolVersion\":2}";

int test_hello_message_serialize() {
    hello_message_t hello;
	size_t hello_message_json_size, hello_message_size, expected_size;
    int result = 0;
	char *hello_message_expected = NULL;

	hello_message_json_size = strlen(hello_message_str_json);
	expected_size = hello_message_json_size + 4;
	hello_message_expected = malloc(expected_size);

	hello_message_expected[0] = hello_message_json_size & 0xff;
	hello_message_expected[1] = (hello_message_json_size >> 8) & 0xff;
	hello_message_expected[2] = (hello_message_json_size >> 16) & 0xff;
	hello_message_expected[3] = (hello_message_json_size >> 24) & 0xff;
	memcpy(
		&(hello_message_expected[4]),
		hello_message_str_json,
		hello_message_json_size
	);

    hello.mac = "mac";
    hello.hostname = "hostname";
    hello.version = "version";
    hello.client_name = "client_name";
    hello.os = "os";
    hello.arch = "arch";
    hello.instance = 1;
    hello.id = "id";
    hello.protocol_version = 2;

    char *hello_message_str = hello_message_serialize(&hello, &hello_message_size);
    if (!hello_message_str) {
        printf("Failed to serialize hello message\r\n");
        return 1;
    }

	if (hello_message_size != expected_size) {
        printf("Expected size to be %d, actually was %d\r\n", expected_size, hello_message_size);
        return 1;
    }

    if (memcmp(hello_message_str, hello_message_expected, hello_message_size)) {
        printf("Incorrect hello message serialization.\r\n");
        printf("Expected: %s\r\n", hello_message_expected);
        printf("Actual: %s\r\n", hello_message_str);
        result = 1;
    }

    free(hello_message_str);
    free(hello_message_expected);
    return result;
}

const uint8_t codec_header_serialized[] = {
    // codec length
    0x05, 0x00, 0x00, 0x00,
    // codec
    'c', 'o', 'd', 'e', 'c',
    // size
    0x04, 0x00, 0x00, 0x00,
    // payload
    0xEF, 0XBE, 0xAD, 0xDE,
};

int test_codec_header_message_deserialize() {
    codec_header_message_t codec_header_message;
    int result = codec_header_message_deserialize(&codec_header_message, codec_header_serialized, 17);
    if (result) {
        printf("Failed to deserialize codec header message: code %d\r\n", result);
        return 1;
    }

    if (strcmp(codec_header_message.codec, "codec")) {
        printf("Expected codec to be \"codec\", actually was \"%s\"\r\n", codec_header_message.codec);
        result = 1;
    }

    if (codec_header_message.size != 4) {
        printf("Expected size to be %d, actually was %d\r\n", 4, codec_header_message.size);
        result = 1;
    }

    if (memcmp(codec_header_message.payload, &(codec_header_serialized[13]), 4)) {
        printf("Payload did not match expected value\r\n");
        printf("Expected: ");
        print_buffer(&(codec_header_serialized[13]), 4);
        printf("\r\nActual:   ");
        print_buffer(codec_header_message.payload, 4);
        printf("\r\n");
        result = 1;
    }

    codec_header_message_free(&codec_header_message);

    return result;
}

const char* server_settings_message_json_string = "{\"bufferMs\":1000,\"latency\":20,\"muted\":false,\"volume\":100}";

int test_server_settings_message_deserialize() {
    server_settings_message_t ss_msg;
    int result = server_settings_message_deserialize(
        &ss_msg, server_settings_message_json_string
    );

    if (result) {
        printf("Failed to deserialize server settings message: code %d\r\n", result);
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

const uint8_t time_message_serialized[] = {
    // latency.sec
    0x78, 0x56, 0x34, 0x12,
    // latency.usec
    0x21, 0x43, 0x65, 0x87,
};

time_message_t time_message_deserialized = {
    { 0x12345678, 0x87654321 },
};

int test_time_message_serialize() {
    char time_message_buffer[TIME_MESSAGE_SIZE];

    int result = time_message_serialize(
        &time_message_deserialized,
        time_message_buffer,
        TIME_MESSAGE_SIZE
    );

    if (result) {
        printf("Failed to serialize time message\r\n");
        return 1;
    }

    result = memcmp(
        time_message_buffer,
        time_message_serialized,
        TIME_MESSAGE_SIZE
    );

    if (result) {
        printf("Serialized time message did not match expected value\r\n");
        printf("Expected: ");
        print_buffer(time_message_serialized, TIME_MESSAGE_SIZE);
        printf("\r\nActual:   ");
        print_buffer(time_message_buffer, TIME_MESSAGE_SIZE);
        printf("\r\n");
        return 1;
    }

    return 0;
}

int test_time_message_deserialize() {
    time_message_t time_message;

    int result = time_message_deserialize(&time_message, time_message_serialized, BASE_MESSAGE_SIZE);
    if (result) {
        printf("Failed to deserialize time message\r\n");
        return 1;
    }

    if (time_message.latency.sec != time_message_deserialized.latency.sec) {
        printf(
            "Expected latency.sec to be %d, actually was %d\r\n",
            time_message_deserialized.latency.sec,
            time_message.latency.sec
        );
        result = 1;
    }

    if (time_message.latency.usec != time_message_deserialized.latency.usec) {
        printf(
            "Expected latency.usec to be %d, actually was %d\r\n",
            time_message_deserialized.latency.usec,
            time_message.latency.usec
        );
        result = 1;
    }

    return result;
}

int main() {
    int fail = 0;

    fail |= test_base_message_serialize();
    fail |= test_base_message_deserialize();
    fail |= test_hello_message_serialize();
    fail |= test_codec_header_message_deserialize();
    fail |= test_server_settings_message_deserialize();
    fail |= test_time_message_serialize();
    fail |= test_time_message_deserialize();

    if (fail) {
        printf("Some tests failed\r\n");
    } else {
        printf("All tests passed\r\n");
    }
    return 0;
}
