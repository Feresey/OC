#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char buf[1024];
char resp[256]="<h1 id='text' style='text-align:center'>TEST</h1><script>setInterval(function(){a=document.getElementById('text');a.innerHTML=(a.innerHTML=='test')?'TEST':'test';},1000);</script>";

const int port = 80;

int main(){
	
	int resp_len = strlen(resp);
	int srv_sock, cl_sock;	
	struct sockaddr_in srv_addr, cl_addr;

	if((srv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket");
		exit(-1);
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(srv_sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0){
		perror("Bind error");
		exit(-2);
	}

	listen(srv_sock, 1);

	int cl_len = sizeof(cl_addr);

	if ((cl_sock = accept(srv_sock,(struct sockaddr *) &cl_addr, &cl_len)) < 0){
		perror("Accept error");
		exit(-3);
	}

	int n = read(cl_sock, buf, 1023);
	if (n < 0){
		perror("Request error");
		exit(-4);
	}

	printf("Request:\n%s",buf);

	n = write(cl_sock, resp, resp_len);
	if (n < 0){
		perror("Responce error");
		exit(-5);
	}
	
	close(cl_sock);
	close(srv_sock);	

	return 0;
}
