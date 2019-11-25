/* 
proxy.cpp 
*/

#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h> 
#include <cstdlib>
#include <signal.h>

using namespace std;

int serv_socket; 
int connection_accepting_socket;
int client_socket; 


void catcher(int sig){
	close(connection_accepting_socket);
	close(client_socket);
	close(serv_socket);
	printf("catcher with signal %d\n", sig);
	exit(0);
}

int main(int argc, char * argv[]) {
	signal(SIGINT, catcher);
	int port = 8001;
	int num_mistakes = atoi(argv[1]);		// Command line arguement for number of mistakes

	// ADDRESS INITIALIZATION OF SERVER

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// SOCKET CREATION OF SERVER 

	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_socket == -1) {
		printf("Socket creation failed. \n");
		exit(-1);
	}

	// BINDING OF SERVER

	int binding_status;
	binding_status = bind(serv_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
	if(binding_status == -1) {
		printf("Binding failed.\n");
		exit(-1);
	}

	// LISTENING FOR SERVER

	int listening_status;
	listening_status = listen(serv_socket, 5);
	if(listening_status == -1) {
		printf("Listening failed.\n");
		exit(-1);
	}

	while(1) {

		// CONNECTION ACCEPTANCE 

		connection_accepting_socket = accept(serv_socket, NULL, NULL);
		if(connection_accepting_socket == -1) {
			printf("Connection was not accepted.\n");
			exit(-1);
		}

		// Initialization of variables.

		char snd_message[1024];
		char rcv_message[1024];
		char server_response[20000];
		char URL[1024];
		char section[20000];

		string str_URL;

		int count, index, i, x, count_client_send;

		int count_client_recieve = 0;
		int end = 0;
		int start = 0;
		int type = 3;

		// Recieving the header information

		count = recv(connection_accepting_socket, rcv_message, 1024, 0);
		if (count == -1) {
			printf("recv() call failed.");
		}

		// Copying it so I can use it for the manipulations. 
		strcpy(snd_message, rcv_message);

		//Stripping everything except the link.
		char *host = strtok(snd_message, "\r\n");
		sscanf(host, "GET http://%s", URL);

		//Stripping the path
		for (index = 0; index < strlen(URL); index++) {
			if (URL[index] == '/') {
				strncpy(host, URL, index);
				break;
			}
		}
		// Filing the rest of host with \0
		for(;index<strlen(host); index++){
			host[index] = '\0';
		}

		// Figuring if it is HTML, TXT or something else and sets the type accordingly so I know when to manipulate the webpage or not. 


		if(strstr(URL, ".html") != NULL) {
			type = 0;
		} else if(strstr(URL, ".txt") != NULL) {
			type = 1;
		} else {
			type = 3;
		}

		// ADDRESS INITIALIZATION
		struct hostent * getHost;

		getHost = gethostbyname(host);
		struct sockaddr_in client_addr;
		memset(&client_addr, 0, sizeof(client_addr));
		client_addr.sin_family = AF_INET; 
		client_addr.sin_port = htons(80);
		client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		bcopy((char *) getHost->h_addr,(char *)&client_addr.sin_addr.s_addr, getHost->h_length);
		
		// SOCKET CREATION 

		client_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(client_socket == -1) {
			printf("Socket creation failed. \n");
			exit(-1);
		}

		// CONNECTION REQUEST 
	
		int connection_status;
		connection_status = connect(client_socket, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in));
		if(connection_status == -1) {
			printf("Failed to connect.\n");
			exit(-1);
		}

		// SEND AND RECEIVE

		// Sending the webserver the header information
		count_client_send = send(client_socket, rcv_message, sizeof(rcv_message), 0);

		if(count_client_send == -1) {
			printf("Error in sending to web server.\n");
			exit(-1);
		}

		// Clearing the arrays I will be using to read to make sure I dont get any errors. 
		memset(section, '\0', sizeof(section));	
		memset(server_response, '\0', sizeof(server_response));	

		// Reading in a loop so I can receive all the information, rather than what a single packet can provide. 
		while(1) {
			count_client_recieve = recv(client_socket, section, sizeof(section), 0);
			if(count_client_recieve <= 0) {
				break;
			} else {
				strcat(server_response, section);			// Concatenating the servers response together using the pieces it sends in each loop
				memset(section, '\0', sizeof(section));		// Clearing the section array.
			}
		}

		if(count_client_recieve == -1) {
			printf("Error in recieving data from web server.\n");
		}

		// Finding the end index.
		while(server_response[end] != '\0') {
			end++;
		}
		end = end - 2;		// To account for the new line

		// Finding the start index
		while(start + 5 < end && server_response[start] != '=' && server_response[start + 1] != 'U') {
			start++;
		}

		// If it is a HTML file. 
		if(type == 0) {
			start = start + 18;			// To get from end of header to near start of html file. It goes past <html>  
			i = 0;
			srand ( time(NULL) );		// Initializing the random funciton
			while(i < num_mistakes) {

				index = (rand() % (end - start + 1) + start);	// Getting random number.
				// Checks to make sure the index is not in certain areas.
				if(server_response[index] != '<' && server_response[index] != '>' && server_response[index] != '\n' && server_response[index] != '\r' && server_response[index] != '/' ) {

					x = index - 1;	// Intializing x
					// Looking for the first '<' or '>' in the given range.
					while(server_response[x] != '<' && server_response[x] != '>' && x < end && x > start) {
						x--;
					}
					// Checks to make sure index is not inside < > and not overwriting a previous mistake or identical charactor.
					if(server_response[x] == '>' && server_response[index] != '@') {

							server_response[index] = '@';
							i++;							// Incrementing the number of mistakes 
							continue;

						} else {
							// If it is inside < > we chose a new random number by continuing.
							continue;
						}
				} else {
					// If index is on a new line, <, >, / or carage return, it will chose a new random number by continuing the loop.
					continue;
				}
			}
		} // If it is a text file.
		if (type == 1) {
			start = start + 10;			// Start is currently at end of header, adding 10 to get it to start of the text
			i = 0;
			srand ( time(NULL) );		// Random intialization
			while(i < num_mistakes) {
				index = (rand() % (end - start + 1));
				index = index + start;
				// Checks to make sure we are not messing up the format
				if(server_response[index] != '\n' && server_response[index] != '\r') {
					server_response[index] = '@';
					i++;
				}
			}
		}

		// Sending the information to the client. 
		count = send(connection_accepting_socket, server_response, sizeof(server_response), 0);
		if(count == -1) {
			printf("Error sending server response to browser.\n");
		}
	}
}
