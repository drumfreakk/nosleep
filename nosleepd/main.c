#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "../sigs.h"

#define BUFSIZE 128

int active = 0;
FILE *fp;

int parse_output(char* cmd) {
	char buf[BUFSIZE];
	FILE *pfp;

	if ((pfp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgets(buf, BUFSIZE, pfp) != NULL) {
		int out = atoi(buf);
		return out;
	}

	if(pclose(pfp))  {
		return -1;
	}

	return 0;
}

void handle_shutdown(int signal) {
	fclose(fp);
	exit(0);
}

void noSleep(){
	active = 1;
}

void yesSleep(){
	active = 0;
}

void handelSig(){
	char msg[255];
	fgets(msg, 255, (FILE*)fp);
	enum Types type = atoi(msg);
	if(type == status){
		kill(parse_output("pidof nosleep"), active ? SIGNSL : SIGASL);
	} else if(type == start){
		active = 1;
	} else if(type == stop){
		active = 0;
	}
}

int main(){

	fp = fopen(FILELOC, "w+");

	if(daemon(1, 0) == -1){ // Fork to a background daemon process.
		printf("Failed opening Daemon\n");
		return 1;
	}

	signal(SIGTERM, handle_shutdown);   // Call handle_shutdown when killed.
	signal(SIGREQ, handelSig);

	while(1){
		if(active){
			system("xdotool mousemove_relative 50 0");
			sleep(2);//40);
			system("xdotool mousemove_relative -- -50 0");
			sleep(2);//40);
		}
	}

}

