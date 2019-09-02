#ifndef NOSLEEP_CPP_SIGS_H
#define NOSLEEP_CPP_SIGS_H

#define SIGREQ SIGUSR1
#define SIGNSL SIGUSR1
#define SIGASL SIGUSR2

#define FILELOC "/home/kip/bin/nosleepstat"

enum Types{
	closed = 0,
	opend,
	stop,
	start,
	status
};

#endif //NOSLEEP_CPP_SIGS_H
