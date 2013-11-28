#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

//define colors
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_BLUE "\x1b[36m"
#define ANSI_DEFAULT "\x1b[0m"

int func_bind(int); // It goes out and connects to the listening application

int func_listen(int);	// It listens for incoming connections

int func_accept(int);

int func_connect(int, char*, int);

void func_close_socket(int);  //closes socket

void print_color_text(char*, char, int, int);

void print_error(char*);	//prints error in red

int slow_down_interface(char *, int); //slow down the interface by the specified seconds

void read_fresh_data(int*, int*); //read user input

int main(int argc, char **argv) {
	int local_port, remote_port, choice = 1, socket_connection, socket_handle,
			counter;
	char buffer[2] = { 0 }, temp;
	char remote_ip[15] = { 0 };
	char* interface = "lo";

	//slow_down_interface("lo", 2);
	//slow the network
//	slow_down_interface(interface, 2);
	//PRINT THE HEADING
	printf(ANSI_YELLOW);
	print_color_text(
			"\n       ▁ ▂ ▄ ▅ ▆ ▇ █ TCP ABNORMAL CONNECTION SIMULATOR - (Client) █ ▇ ▆ ▅ ▄ ▂ ▁",
			'Y', 2, 0);

	print_color_text("\nPlease enter the Server IP addr:- ", 'D', 0, 0);
	scanf("%s", &remote_ip);
	//slow the network
	//slow_down_interface(interface, 2);

	//set the socket handle

	while (choice != 8) {
		//show menu
		print_color_text(
				"\nPlease select one of the following choices:-\n 1. Normal Open\n 2. Simultaneous open\n 3. Active close by server\n 4. Simultaneous Close\n 5. Half Close - Client side\n 6. Half Close - Server side\n 7. Nyte Stream FIFO at the Reciever side\n 8. Quit\n ",
				'B', 1, 0);
		print_color_text("CHOICE:= ", 'Y', 0, 0);
		scanf("%d", &choice);
		print_color_text("", 'D', 0, 2);
		//initialize variables to default
		socket_handle = -1;
		bzero((char*) &buffer, sizeof(buffer));
		//draw a line after each demo
		print_color_text("", 'R', 1, 1);
		//read fresh user input
		read_fresh_data(&remote_port, &local_port);

		switch (choice) {
			case 1:		//Normal Open
				socket_handle = func_bind(local_port);
				if (func_listen(socket_handle) > 0) {

					if ((socket_connection = func_accept(socket_handle)) > 0) {
						while (socket_connection > 0) {
							if (read(socket_connection, buffer, 1) < 0) {
								func_close_socket(socket_handle);
								print_error(
										"Could not read incoming connections. ERROR:- ");
								return (0);
							}

							//display message and return the same to client
							print_color_text("Received: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
							print_color_text("Sending: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
							print_color_text("", 'D', 0, 2);

							if (write(socket_connection, buffer, 1) < 0) {
								func_close_socket(socket_handle);
								print_error(
										"Could not send message to the client. ERROR:- ");
								return (0);
							}

							//detect if command to quit the connection has been called
							if (buffer[0] == 'C') break;

						}
						print_color_text("Closing Connection..", 'R', 0, 1);
						func_close_socket(socket_connection);
					}
				} else {
					print_error("Please try again..");
				}
				func_close_socket(socket_handle);
				break;
			case 2:
				//simultaneous open
				if ((socket_handle = func_bind(local_port)) >= 0) {
					if (func_connect(socket_handle, remote_ip, remote_port)
							> 0) {
						if (recv(socket_handle, buffer, 1, 0) < 0) {
							print_error("Receive failed. ERROR:- ");
							func_close_socket(socket_handle);
							continue;
						} else {
							print_color_text("Received: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}
						if (send(socket_handle, buffer, 1, 0) < 0) {
							print_error("Send failed. ERROR:- ");
							func_close_socket(socket_handle);
							continue;
						} else {
							print_color_text("Send: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}
					}
				} else {
					print_error("Please try again..");
				}
				func_close_socket(socket_handle);

				break;
			case 3: 	//active close by server
				socket_handle = func_bind(local_port);
				//strcat(buffer, "A");
				if (func_listen(socket_handle) > 0) {
					if ((socket_connection = func_accept(socket_handle)) > 0) {
						if (recv(socket_handle, buffer, 1, 0) < 0) {
							print_error("Receive failed. ERROR:- ");
							func_close_socket(socket_handle);
							continue;
						} else {
							print_color_text("Received: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}
						if (send(socket_handle, buffer, 1, 0) < 0) {
							print_error("Send failed. ERROR:- ");
							func_close_socket(socket_handle);
							continue;
						} else {
							print_color_text("Send: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}
					}
				} else {
					print_error("Please try again. ERROR:- ");
				}
				print_color_text("Initiating Active close...", 'G', 0, 1);
				close(socket_connection);
				func_close_socket(socket_handle);
				//print_color_text("Connection closed..", 'R', 0, 1);

				break;
			case 4:		//active close by client
				socket_handle = func_bind(local_port);
				//strcat(buffer, "A");
				if (func_listen(socket_handle) > 0) {
					if ((socket_connection = func_accept(socket_handle)) > 0) {
						if (recv(socket_handle, buffer, 1, 0) < 0) {
							print_error("Receive failed. ERROR:- ");
							close(socket_handle);
							continue;
						} else {
							print_color_text("Received: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}
						if (send(socket_handle, buffer, 1, 0) < 0) {
							print_error("Send failed. ERROR:- ");
							continue;
						} else {
							print_color_text("Send: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}
						print_color_text(
								"Waiting for Active close by client ..", 'R', 0,
								1);
					}
				} else {
					print_error("Please try again. ERROR:- ");
					close(socket_connection);
					func_close_socket(socket_handle);
				}
//				close(socket_connection);
//				func_close_socket(socket_handle);
//				while ()
//				print_color_text("Connection closed..", 'R', 0, 1);
				break;
			case 5:		//simultaneous close
				socket_handle = func_bind(local_port);
				if (func_listen(socket_handle) > 0) {

					if ((socket_connection = func_accept(socket_handle)) > 0) {
						while (socket_connection > 0) {
							if (read(socket_connection, buffer, 1) < 0) {
								close(socket_handle);
								print_error(
										"Could not read incoming connections. ERROR:- ");
								return (0);
							}

							//display message and return the same to client
							print_color_text("Received: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
							print_color_text("Sending: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
							print_color_text("", 'D', 0, 2);

							if (write(socket_connection, buffer, 1) < 0) {
								close(socket_handle);
								print_error(
										"Could not send message to the client. ERROR:- ");
								return (0);
							}

							//simultaneous closing of connection
							print_color_text(
									"Please enter any character/ number but press <enter> simultaneously on both the systems (Server- Client)",
									'Y', 0, 1);
							scanf("%s", &temp);
						}
						close(socket_connection);
					}
				} else {
					print_error("Please try again..");
					func_close_socket(socket_handle);
				}
				break;
			case 6:		//Half close by server
				socket_handle = func_bind(local_port);
				if (func_listen(socket_handle) > 0) {
					if ((socket_connection = func_accept(socket_handle)) > 0) {
						if (recv(socket_handle, buffer, 1, 0) < 0) {
							print_error("Receive failed. ERROR:- ");
							close(socket_handle);
							continue;
						} else {
							print_color_text("Received: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}
						if (send(socket_handle, buffer, 1, 0) < 0) {
							print_error("Send failed. ERROR:- ");
							continue;
						} else {
							print_color_text("Send: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}

						print_color_text("Performing Half close by server..",
								'R', 0, 1);
						shutdown(socket_handle, 1);

						print_color_text("Received data after half close", 'Y',
								1, 1);
						while (recv(socket_handle, buffer, 1, 0) > 0) {
							print_color_text("Received: ", 'D', 0, 0);
							print_color_text(buffer, 'Y', 0, 1);
						}
						close(socket_connection);
						func_close_socket(socket_handle);
					}
				} else {
					print_error("Please try again. ERROR:- ");
					func_close_socket(socket_handle);
				}
				break;
			case 7:		//half close by client
				counter = 'A';
				socket_handle = func_bind(local_port);
				if (func_listen(socket_handle) > 0) {
					if ((socket_connection = func_accept(socket_handle)) > 0) {
						while (counter <= (int) ('F')) {
							strcat(buffer, (char*) (&counter));
							if (send(socket_handle, buffer, 1, 0) < 0) {
								print_error("Send failed. ERROR:- ");
								continue;
							} else {
								print_color_text("Send: ", 'D', 0, 0);
								print_color_text(buffer, 'Y', 0, 1);
							}
							counter++;
						}
						func_close_socket(socket_handle);
					}
				} else {
					print_error("Please try again. ERROR:- ");
					func_close_socket(socket_handle);
				}
				break;
			case 8:
				//quit
				break;
			default:
				// abnormal choice, ask again
				break;
		} /* switch (choice) */
	}

//set network to default speed
//slow_down_interface(interface, 0);

//exit

	return (0);
}

void print_color_text(char *message, char color, int underline,
		int print_return) {
//	?select color
	switch (color) {
		case 'R':
			printf(ANSI_RED);
			break;
		case 'G':
			printf(ANSI_GREEN);
			break;
		case 'Y':
			printf(ANSI_YELLOW);
			break;
		case 'B':
			printf(ANSI_BLUE);
			break;
		case 'D':
			printf(ANSI_DEFAULT);
			break;
		default:
			printf(ANSI_DEFAULT);
	};

	printf("%s", message);
	if (strlen(message) == 0)
		message =
				"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	int length = 0;
	if (underline > 0) {
		int lines = 0;
		for (lines = 0; lines < underline; lines++) {
			printf("\n");
			for (length = 0; length < strlen(message) && length < 79; length++)
				printf("-");
		}
	}
	int i = 0;
	if (length >= 79 || print_return > 0) {
		if (length >= 79 && print_return < 1) print_return = 1;
		for (i = 0; i < print_return; i++)
			printf("\n");
	}
	printf(ANSI_DEFAULT);
}

int slow_down_interface(char * interface, int secs) {
	FILE * fp;
	char path[200];
	char result[1000] = { 0 };
	char cmd[68];
	char cmd_mod[73];

//prepare the cmd
	sprintf(cmd, "tc qdisc add dev %s root handle 1:0 netem delay %dsec 2>&1",
			interface, secs);
	sprintf(cmd_mod,
			"tc qdisc replace dev %s root handle 1:0 netem delay %dsec 2>&1",
			interface, secs);

//run the cmd
	fp = popen(cmd, "r");
	if (fp == NULL) {
		print_error("Please try again. ERROR:- s");
		exit(0);
	}
//capture the msg by the command for analysis
	fgets(result, sizeof(path) - 1, fp);

//if not running as sudo then prompt for doing so and exit
	if (strstr(result, "not permitted") != NULL) {
		print_color_text("Please run the program as a superuser", 'R', 1, 1);
		exit(0);
	}
//if the command has already been issued then run the 'replace' command
	if (strstr(result, "exists") != NULL) {
		//		printf("%s\n", cmd_mod);
		fp = popen(cmd_mod, "r");
		if (fp == NULL) {
			print_error("Please try again. ERROR:- ");
			exit(0);
		}
	}
	/* close file pipe*/
	pclose(fp);
	return (1);
}

int func_bind(int local_port) {
	int socket_handle;
	struct sockaddr_in listen_addr;	//, client_addr;

//create socket
	if ((socket_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
		print_error("Cannot open socket to listen. ERROR:- ");
		close(socket_handle);
		return (-1);
	}

//initialize the struct to zero
	bzero((char*) &listen_addr, sizeof(listen_addr));

//set up the sockaddr_in
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(local_port);

//try binding the server to the port
	if (bind(socket_handle, (struct sockaddr *) &listen_addr,
			sizeof(listen_addr)) < 0) {
		close(socket_handle);
		print_error("Cannot bind server to the specified socket. ERROR:- ");
		return (-1);
	}
	print_color_text("Socket binded to port - ", 'D', 0, 0);
	printf(ANSI_GREEN);
	printf("%d\n", local_port);
	printf(ANSI_DEFAULT);
	return (socket_handle);

}

int func_listen(int socket_handle) {
//listen for incoming connection
	if (listen(socket_handle, 5) < 0) {
		print_error("Could not listen on the socket. ERROR:- ");
		return (0);
	} else {
		print_color_text("Waiting for incoming connection..", 'D', 0, 1);
		return (1);
	}
}

int func_connect(int socket_handle, char* remote_ip, int remote_port) {
	struct sockaddr_in remote_addr;
	bzero((char*) &remote_addr, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(remote_ip);
	remote_addr.sin_port = htons(remote_port);
	if (connect(socket_handle, (struct sockaddr*) &remote_addr,
			sizeof(remote_addr)) < 0) {
		print_error("Could not connect to the remote machine. ERROR:- ");
		return (0);
	}
	print_color_text("Connected to the remote machine..", 'G', 0, 1);
	return (1);
}

int func_accept(int socket_handle) {
	int socket_connection;
	if ((socket_connection = accept(socket_handle, NULL, NULL)) > -1) {
		print_color_text("Client Connected to server..", 'G', 0, 2);
		return (socket_connection);
	} else {
		print_error("Could not accept incoming connection. ERROR:- ");
		return (-1);
	}
}
void func_close_socket(int socket_handle) {
	close(socket_handle);
	print_color_text("Closing connection..", 'R', 0, 1);
}

void read_fresh_data(int* remote_port, int* local_port) {
	print_color_text("Enter setup data", 'D', 1, 0);
	print_color_text("\nPlease enter the remote port:- ", 'G', 0, 0);
	scanf("%d", remote_port);
	print_color_text("\nPlease enter the local port:- ", 'G', 0, 0);
	scanf("%d", local_port);
	print_color_text("", 'D', 1, 0);
}

void print_error(char* err_msg) {
	printf(ANSI_RED);
	perror(err_msg);
	printf(ANSI_DEFAULT);
	printf("\n");
}
