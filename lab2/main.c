#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tree.h>
#include <tree_io.h>

#define SGN(x) ((x) > 0 ? 1 : (x) < 0 ? -1 : 0)
#define LINE_SZ 100

typedef struct {
	char *str;
	void *fun;
	void *fp;
	int   argc;
} cmd;

int compare(void *a, void *b) {
	return SGN(*(int *)a - *(int *)b);
}

void print(void *f, void *v) {
	fprintf((FILE *)f, "%d\n", *(int *)v);
}

/* This workaround exists to call a complicated print_tree function */
void print_caller(ntree *t) {
	print_tree(NULL, t, print);
}

int parent(char *flnm, int f_wt, int f_rd) {
	FILE *inf  = flnm ? fopen(flnm, "r") : stdin;
	void *null_ptr = NULL;
	char  line[LINE_SZ];
	cmd   carr[] = {
		{"add",   bin_add,      compare, 1},
		{"print", print_caller, print,   0}
	};

	printf("%s started\n", __FUNCTION__);

	while (fgets(line, LINE_SZ, inf)) {
		int    i;
		char   c[LINE_SZ];
		cmd   *com = NULL;
		int arg = 0;

		for (i = 0; i < 2; ++i) {
			sscanf(line, "%s", c);

			if (!strcmp(carr[i].str, c)) {
				com = carr + i;
				break;
			}
		}

		if (!com) 
			fprintf(stderr, "Unknown command %s\n", c);
		else {
			if (com->argc > 0) 
				arg = atoi(line + strlen(com->str));

			write(f_wt, &(com->fun), sizeof(void *)); 
			write(f_wt, &(com->fp),  sizeof(void *)); 
			write(f_wt, &arg,        sizeof(arg)); 
		}
	}

	write(f_wt, &null_ptr, sizeof(void *));

	printf("%s stopped\n", __FUNCTION__);

	if (inf != stdin)
		fclose(inf);

	return 0;
}

int child (int f_wt, int f_rd) {
	int    err = 0;
	ntree *tree = NULL;
	void  *(*ptr)(ntree *, void *, int, void *);
	void  *fp;
	int    arg = 0;

	printf("%s started\n", __FUNCTION__);

	new_tree (&tree);

	do {
		read(f_rd, &ptr, sizeof(void *));

		if (ptr) {
			read(f_rd, &fp,  sizeof(void *));
			read(f_rd, &arg, sizeof(arg));
			printf("calling %p %d %d\n", ptr, arg, sizeof(arg));
			ptr (tree, &arg, sizeof(arg), fp);
			printf("called\n");
		}
	} while (ptr);

	del_tree(tree);

	printf("%s stopped\n", __FUNCTION__);

	return 0;
}

int main(void) {
	int fd1[2], fd2[2];
	int er1 = pipe(fd1);
	int er2 = pipe(fd2);
	int err = 0;          
	int erp = 0;
	int erc = 0;
	int pid = fork();

	if (pid > 0)
		erp = parent(NULL, fd1[1], fd2[0]);
	else
		erc = child (fd2[1], fd1[0]);

	err = er1 | er2 | erc | erp;

	return err;
}
