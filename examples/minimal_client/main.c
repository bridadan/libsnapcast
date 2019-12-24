#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "snapcast.h"

#define MAX 512
#define PORT 1704
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[MAX];
    int n;
	char base_message_serialized[BASE_MESSAGE_SIZE];
	char size_buffer[4];
	char *hello_message_serialized;
	size_t hello_message_size;
	int result, i;
	
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

	hello_message_serialized = hello_message_serialize(&hello_message);
	if (!hello_message_serialized) {
		printf("Failed to serialize hello message\r\b");
		return;
	}

	hello_message_size = strlen(hello_message_serialized);
	base_message.size = hello_message_size + sizeof(uint32_t);

	size_buffer[0] = hello_message_size & 0xff;
	size_buffer[1] = (hello_message_size >> 8) & 0xff;
	size_buffer[2] = (hello_message_size >> 16) & 0xff;
	size_buffer[3] = (hello_message_size >> 24) & 0xff;

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
	write(sockfd, size_buffer, 4); 
	write(sockfd, hello_message_serialized, base_message.size);
	result = read(sockfd, buff, MAX);
	if (result < 0) {
		printf("Failed to read from server: %d\r\n", result);
		return;
	}

	printf("Read:\r\n");

	for (i = 0; i < result; i++) {
		printf("%02x", buff[i] & 0xff);
	}


    /*
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
    */
	free(hello_message_serialized);
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
        printf("Socket successfully created..\n");
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
        printf("connected to the server..\n");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}

