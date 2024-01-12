// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include <arpa/inet.h>

#define PORT 8080
using namespace std;

int new_socket, sock;

int serverinit() {

	int server_fd;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
		== 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket
		= accept(server_fd, (struct sockaddr*)&address,
				(socklen_t*)&addrlen))
		< 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
	return 0;
}

char* serverfn(int my_x, int my_y, int score, int dottype, int direction) //dottype is 1 when player is on yulu
{
	char buffer[1024] = { 0 };

	string hello_st = to_string(my_x) + "," + to_string(my_y)+ "," + to_string(score) + "," + to_string(dottype) + "," + to_string(direction) + '\0'; //this is the msg that will be sent
	int hello_len = hello_st.length();
 
    // declaring character array
    char char_array[hello_len + 1];
 
    // copying the contents of the
    // string to char array
    strcpy(char_array, hello_st.c_str());

	char* hello = char_array;
	// char* hello = (char*)"Hello from server";

	int valread = read(new_socket, buffer, 1024);
	// printf("%s\n", buffer);
	send(new_socket, hello, strlen(hello), 0);
	// printf("Hello message sent\n");

    char *str = (char*)malloc(1024 * sizeof(char));
	strcpy (str, buffer);
	return str;
}

int clientinit() {

	struct sockaddr_in serv_addr;
	char* hello = (char*)"Hello from client";
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "192.168.128.60", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
	return 0;

}

char* clientfn(int my_x, int my_y, int score, int dottype, int direction) //dottype is 1 when player is on yulu
{
	int valread;
	char buffer[1024] = { 0 };

	string hello_st = to_string(my_x) + "," + to_string(my_y)+ "," + to_string(score) + "," + to_string(dottype) + "," + to_string(direction) + '\0'; //this is the msg that will be sent

	int hello_len = hello_st.length();
 
    // declaring character array
    char char_array[hello_len + 1];
 
    // copying the contents of the
    // string to char array
    strcpy(char_array, hello_st.c_str());

	char* hello = char_array;
	// char* hello = (char*)"Hello from client";

	send(sock, hello, strlen(hello), 0);
	// printf("Hello message sent\n");
	valread = read(sock, buffer, 1024);
	// printf("%s\n", buffer);


	char *str = (char*)malloc(1024 * sizeof(char));
	strcpy (str, buffer);
	return str;
}
