#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>
#include <fcntl.h>

int main(){
	
	int res, fd;
	char buf[] = "This file reside on memory disk.\n";
	res = mount("tmpfs", "tmpdir", "tmpfs", 0, "size=1M");
	if (res < 0){
		perror("Mount error");
		exit(-1);
	}

	if ((fd = open("tmpdir/memfile.txt", O_CREAT | O_WRONLY , S_IRUSR | S_IWUSR)) < 0){
		perror("File error");
		exit(-2);
	}

	
	write(fd, buf, sizeof(buf));

	close(fd);
	return 0;
}
