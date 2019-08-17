#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define BUFSIZE 128

int running = 0;
int checkedStat = 0;
char stat[100];

int parse_output(char* cmd) {
	char buf[BUFSIZE];
	FILE *fp;

	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {
		int out = atoi(buf);
		running = 1;
		return out;
	}

	if(pclose(fp))  {
		running = 0;
		return -1;
	}

	return 0;
}

void sendHelp(){
	printf("Commands:\n");
	printf("0\tDisplay this message\n");
	printf("1\tDon't allow sleeping\n");
	printf("2\tAllow sleeping\n");
	printf("3\tStart the daemon\n");
	printf("4\tStop the daemon\n");
	exit(0);
}

void noSleep(){
	checkedStat = 1;
	sprintf(stat, "Preventing Sleep");
}

void yesSleep(){
	checkedStat = 1;
	sprintf(stat, "Allowing Sleep");
}

void printStatus(int pid){
	if(running == 0){
		sprintf(stat, "Not Running");
	} else {
		kill(pid, 2);

		signal(SIGUSR1, noSleep);
		signal(SIGUSR2, yesSleep);

		while(!checkedStat){}
	}
	printf("nosleepd status: %s\n", stat);
}

int main(){
	int sigtype;
	int inp;
	int pid = parse_output("pidof nosleepd");

	printf("This is the handler for nosleepd\n");
	printStatus(pid);
	printf("Type 0 for help.\n");

	scanf("%d", &inp);

	if(inp == 0){
		sendHelp();
	} else if(inp == 1){
		sigtype = 10;
		printf("Preventing the computer from sleeping...\n");
	} else if(inp == 2){
		sigtype = 12;
		printf("Allowing the computer to sleep...\n");
	} else if(inp == 3){
		if(running == 0){
			parse_output("nosleepd");
		}
		running = 1;
		printf("Starting the nosleepd daemon...\n");
		return 0;
	} else if(inp == 4){
		sigtype = 15;
		printf("Shutting the nosleepd daemon down...\n");
	} else {
		printf("Unknown command, try one of these:\n");
		sendHelp();
	}

	if(running == 0){
		printf("The nosleepd daemon is not running, try starting it before sending other commands to it.\n");
		return 0; //exit(0);
	}

	kill(pid, sigtype);

	return 0;
}