#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
//#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

//define colors
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_BLUE "\x1b[36m"
#define ANSI_DEFAULT "\x1b[0m"

int speaker(int, char*, int); // It goes out and connects to the listening application
int listener(int, int);	// It listens for incoming connections
void print_color_text(char*, char, int);
int slow_down_interface(char *, int); //slow down the interface by the specified seconds
int main(int argc, char **argv) {
	int local_port, remote_port;
	char* read_me;
	char* remote_ip;

	slow_down_interface("lo", 2);

	//PRINT THE HEADING
	printf(ANSI_YELLOW);
	print_color_text(
			"\n       ▁ ▂ ▄ ▅ ▆ ▇ █ TCP ABNORMAL CONNECTION SIMULATOR - M █ ▇ ▆ ▅ ▄ ▂ ▁",
			'Y', 1);
	//	printf("\n");

	print_color_text("\nPlease enter the remote IP addr:- ", 'D', 0);
	scanf("%s", remote_ip);
	print_color_text("\nPlease enter the remote port:- ", 'D', 0);
	scanf("%d", &remote_port);

	//show menu
	print_color_text(
			"\nPlease select one of the following choices:-\n 1. Normal Open\n 2. Simultaneous open\n 3. Active close by server\n 4. Simultaneous Close\n 5. Half Close - Client side\n 6. Half Close - Server side\n 7. Nyte Stream FIFO at the Reciever side ",
			'B', 1);

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
	int lines = 0, length = 0;
	if (underline > 0) {
		for (lines = 0; lines < underline; lines++) {
			printf("\n");
			for (length = 0; length < strlen(message) && length < 79; length++)
				printf("-");
		}
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
		printf("%s\n", cmd_mod);
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
