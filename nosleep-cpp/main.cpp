#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include "../sigs.h"

Types type;

bool running = false;
bool checkedStat = false;
char stat[100];

void sendMsg(int pid, std::fstream *file);

int exec(const char* cmd);

void sendHelp();

void noSleep(int sig);
void yesSleep(int sig);

int main(){
	int sigtype;
	int inp;
	int pid = exec("pidof nosleepd");
	std::fstream file;
	file.open(FILELOC, std::ios::in | std::ios::out | std::ios::ate);
	if(!file.is_open()){
		std::cout << "Failed opening file\n";
		return -1;
	}

	std::cout << "This is the handler for nosleepd\n";
	if(running == false){
		sprintf(stat, "\033[1;31mNot Running\033[0m");
	} else {
		type = status;
		sendMsg(pid, &file);

		signal(SIGNSL, noSleep);
		signal(SIGASL, yesSleep);

		while(!checkedStat){}
	}
	std::cout << "nosleepd status: " << stat << std::endl;
	std::cout << "Type 0 for help.\n";

	scanf("%d", &inp);

	if(inp == 0){
		sendHelp();
	} else if(inp == 1){
		type = start;
		std::cout << ("Preventing the computer from sleeping...\n");
	} else if(inp == 2){
		type = stop;
		std::cout << ("Allowing the computer to sleep...\n");
	} else if(inp == 3){
		type = opend;
		if(!running){
			exec("nosleepd");
		}
		running = true;
		file.open(FILELOC, std::ios::in | std::ios::out | std::ios::ate);
		if(!file.is_open()){
			std::cout << "Failed reopening file\n";
			return -1;
		}
		std::cout << ("Starting the nosleepd daemon...\n");
	} else if(inp == 4){
		type = closed;
		std::cout << ("Shutting the nosleepd daemon down...\n");
	} else {
		std::cout << ("Unknown command, try one of these ");
		sendHelp();
	}

	if(!running){
		std::cout << ("The nosleepd daemon is not running, try starting it before sending other commands to it.\n");
		return 0;
	}

	sendMsg(pid, &file);
	file.close();
	return 0;
}

void sendMsg(int pid, std::fstream *file){
	std::string msg = std::to_string(type);
	int signal;
	if(type == closed){
		signal = SIGTERM;
		file->close();
	}else if(type == opend){
		return;
	}else{
		signal = SIGREQ;
		*file << msg << "\n";
	}
	kill(pid, signal);
}

int exec(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}

	if(result == ""){
		std::cout << "No pid\n";
		running = false;
		return -1;
	}

	int pid = std::stoi(result);


	running = true;

	return pid;
}

void sendHelp(){
	std::cout << ("Commands:\n");
	std::cout << ("0\tDisplay this message\n");
	std::cout << ("1\tDon't allow sleeping\n");
	std::cout << ("2\tAllow sleeping\n");
	std::cout << ("3\tStart the daemon\n");
	std::cout << ("4\tStop the daemon\n");
	exit(0);
}

void noSleep(int sig){
	checkedStat = true;
	sprintf(stat, "\033[1;36mPreventing Sleep\033[0m");
}

void yesSleep(int sig){
	checkedStat = true;
	sprintf(stat, "\033[1;32mAllowing Sleep\033[0m");
}
