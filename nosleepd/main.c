#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFSIZE 128

int nAllowSleep = 0;

int parse_output(char* cmd) {
	char buf[BUFSIZE];
	FILE *fp;

	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {
		int out = atoi(buf);
		return out;
	}

	if(pclose(fp))  {
		return -1;
	}

	return 0;
}

void handle_shutdown(int signal) {
	exit(0);
}

void noSleep(){
	nAllowSleep = 1;
}

void yesSleep(){
	nAllowSleep = 0;
}

void status(){
	kill(parse_output("pidof nosleep"), nAllowSleep ? 10 : 12);
}

int main(){

	if(daemon(1, 0) == -1){ // Fork to a background daemon process.
		printf("Failed opening Daemon\n");
		return 1;
	}

	signal(SIGTERM, handle_shutdown);   // Call handle_shutdown when killed.
	signal(SIGINT, status);   // Call handle_shutdown when interrupted.
	signal(SIGUSR1, noSleep);
	signal(SIGUSR2, yesSleep);

	while(1){
		if(nAllowSleep){
			system("xdotool mousemove_relative 1 0");
			sleep(240);
			system("xdotool mousemove_relative -- -1 0");
			sleep(240);
		}
	}

}

