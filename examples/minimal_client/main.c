#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "snapcast.h"

#define MAX 6000
#define PORT 1704
#define SA struct sockaddr

void print_buffer(const char* buffer, size_t size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%02x", buffer[i] & 0xff);
    }
}

void func(int sockfd)
{
    char buff[MAX];
    char *start;
    int n;
    char base_message_serialized[BASE_MESSAGE_SIZE];
    char size_buffer[4];
    char *hello_message_serialized;
    int result, i, size;

    codec_header_message_t codec_header_message;
    wire_chunk_message_t wire_chunk_message;
    
    bool received_header = false;
    base_message_t base_message = {
        hello,
        0x0,
        0x0,
        { 0x5df65146, 0x0 },
        { 0x5df65146, 0x0 },
        0x0,
    };

    hello_message_t hello_message = {
        "0c:8b:fd:d0:e4:d1",
        "brianlaptop",
        "0.0.0",
        "libsnapcast",
        "Arch Linux",
        "x86_64",
        1,
        "0c:8b:fd:d0:e4:d1",
        2,
    };

    hello_message_serialized = hello_message_serialize(&hello_message, (size_t*) &(base_message.size));
    if (!hello_message_serialized) {
        printf("Failed to serialize hello message\r\b");
        return;
    }

    result = base_message_serialize(
        &base_message,
        base_message_serialized,
        BASE_MESSAGE_SIZE
    );
    if (result) {
        printf("Failed to serialize base message\r\n");
        return;
    }

    write(sockfd, base_message_serialized, BASE_MESSAGE_SIZE);
    write(sockfd, hello_message_serialized, base_message.size);
    free(hello_message_serialized);

    for (;;) {
        size = read(sockfd, buff, MAX);
        if (size < 0) {
            printf("Failed to read from server: %d\r\n", size);
            return;
        }

        result = base_message_deserialize(&base_message, buff, size);
        if (result) {
            printf("Failed to read base message: %d\r\n", result);
            // TODO there should be a big circular buffer or something for this
            return;
        }

        start = &(buff[BASE_MESSAGE_SIZE]);
        size -= BASE_MESSAGE_SIZE;
        ///print_buffer(start, size);
        ///printf("\r\n");

        switch (base_message.type) {
            case codec_header:
                result = codec_header_message_deserialize(&codec_header_message, start, size);
                if (result) {
                    printf("Failed to read codec header: %d\r\n", result);
                    return;
                }

                for (i = 0; i < codec_header_message.size; i++) {
                    printf("%c", codec_header_message.payload[i]);
                }

                codec_header_message_free(&codec_header_message);
                received_header = true;
            break;
            
            case wire_chunk:
                if (!received_header) {
                    continue;
                }

                result = wire_chunk_message_deserialize(&wire_chunk_message, start, size);
                if (result) {
                    printf("Failed to read wire chunk: %d\r\n", result);
                    return;
                }

                for (i = 0; i < wire_chunk_message.size; i++) {
                    printf("%c", wire_chunk_message.payload[i]);
                }
                
                wire_chunk_message_free(&wire_chunk_message);
            break;
        }
    }
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        ///printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        ///printf("connected to the server..\n");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}

