#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

const char * fname = "test1.txt";

char buf[256];

int main(){
	
	int fd;

	if ((fd = open(fname, O_RDONLY)) < 0){
		perror(fname);
		exit(-1);
	}

	read(fd, buf, 4);

	buf[4] = '\n';

	write(1, buf, 5);

	lseek(fd, 15, SEEK_SET);
	
	if(read(fd, buf, 4) <= 0){
		printf("Cannot read data\n");
	} else {
		buf[4] = '\n';
		write(1, buf, 5);
	}

	close(fd);
	

	return 0;
}
