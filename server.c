#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

//define colors
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_BLUE "\x1b[36m"
#define ANSI_DEFAULT "\x1b[0m"

int func_speak(char*, int); // It goes out and connects to the listening application

int func_listen(int);	// It listens for incoming connections

void print_color_text(char*, char, int);

void print_error(char*);	//prints error in red

int slow_down_interface(char *, int); //slow down the interface by the specified seconds

int main(int argc, char **argv) {
	int local_port, remote_port, choice = 1;
	//char* read_me;
	char* remote_ip;
	char* interface = "lo";

	//slow_down_interface("lo", 2);
	//slow the network
	slow_down_interface(interface, 2);
	//PRINT THE HEADING
	printf(ANSI_YELLOW);
	print_color_text(
			"\n       ▁ ▂ ▄ ▅ ▆ ▇ █ TCP ABNORMAL CONNECTION SIMULATOR - (Server) █ ▇ ▆ ▅ ▄ ▂ ▁",
			'Y', 2);

	print_color_text("INITIAL SETUP", 'D', 1);
	print_color_text("\nPlease enter the remote IP addr:- ", 'D', 0);
	scanf("%s", &remote_ip);
	print_color_text("\nPlease enter the remote port:- ", 'D', 0);
	scanf("%d", &remote_port);
	print_color_text("\nPlease enter the local port:- ", 'D', 0);
	scanf("%d", &local_port);
	print_color_text("", 'D', 1);

	//slow the network
	slow_down_interface(interface, 2);

	//set the socket handle

	while (choice != 8) {
		//show menu
		print_color_text(
				"\nPlease select one of the following choices:-\n 1. Normal Open\n 2. Simultaneous open\n 3. Active close by server\n 4. Simultaneous Close\n 5. Half Close - Client side\n 6. Half Close - Server side\n 7. Nyte Stream FIFO at the Reciever side\n 8. Quit\n ",
				'B', 1);
		switch (choice) {

			case 1:		//Normal Open
				func_listen(local_port);

				break;
			case 2:		//simultaneous open
				break;
			case 3:		//active close by server
				break;
			case 4:		//active close by client
				break;
			case 5:		//simultaneous close
				break;
			case 6:		//Half close by server
				break;
			case 7:		//half close by client
				break;
			case 8:		//quit
				break;
			default:		// abnormal choice, ask again
				break;
		} /* switch (choice) */

		//switch case
		choice = 8;
	}

	//set network to default speed
	//slow_down_interface(interface, 0);

	//exit

	return (0);
}

void print_color_text(char *message, char color, int underline) {
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
	if (length >= 79) printf("\n");
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
		perror("Please try again. ERROR:- \n");
		exit(0);
	}
	//capture the msg by the command for analysis
	fgets(result, sizeof(path) - 1, fp);

	//if not running as sudo then prompt for doing so and exit
	if (strstr(result, "not permitted") != NULL) {
		print_color_text("Please run the program as a superuser", 'R', 1);
		printf("\n");
		exit(0);
	}
	//if the command has already been issued then run the 'replace' command
	if (strstr(result, "exists") != NULL) {
		//		printf("%s\n", cmd_mod);
		fp = popen(cmd_mod, "r");
		if (fp == NULL) {
			perror("Please try again. ERROR:- \n");
			exit(0);
		}
	}
	/* close file pipe*/
	pclose(fp);

	return (1);

}

int func_listen(int local_port) {
	int socket_handle, socket_connection;
	struct sockaddr_in listen_addr, client_addr;
	char buffer[100] = { 0 }; //buffer to receive and send messages

	//create socket
	if ((socket_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
		print_error("Cannot open socket to listen. ERROR:- ");
		close(socket_handle);
		return (0);
	} /* if (sockethandle) */

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
		return (0);
	}

	//listen for incoming connection
	listen(socket_handle, 5);
	print_color_text("Waiting for incoming connection..", 'D', 0);

	if ((socket_connection = accept(socket_handle,
			(struct sockaddr *) &client_addr,
			(socklen_t*) (sizeof(struct sockaddr_in)))) > 0)
		print_color_text("Client Connected to server", 'G', 0);
	else {
		print_error("Could not accept incoming connection. ERROR:- ");
		return (0);
	}

	while (socket_connection > 0) {

		if (read(socket_connection, buffer, 99) < 0) {
			close(socket_handle);
			print_error("Could not read incoming connections. ERROR:- ");
			return (0);
		}
		//detect if command to quit the connection has been called
		if (buffer == "quit") break;
		//display message and return the same to client
		print_color_text("Received: ", 'D', 1);
		print_color_text(buffer, 'Y', 0);
		printf("\n");
		print_color_text("Sending: ", 'D', 1);
		print_color_text(buffer, 'Y', 0);
		printf("\n");

		if (write(socket_connection, buffer, 99) < 0) {
			close(socket_handle);
			print_error("Could not send message to the client. ERROR:- ");
			return (0);
		}

	}
	close(socket_connection);
	close(socket_handle);
	return (1);
}

void print_error(char* err_msg) {
	printf(ANSI_RED);
	perror(err_msg);
	printf(ANSI_DEFAULT);
	printf("\n");

}
