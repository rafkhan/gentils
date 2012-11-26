#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

struct arg_list {
	int show_hidden;
	char** dir_list;
};

int one(const struct dirent *ent);
void itr_ent(char* dir_name, struct arg_list args);
int no_dot(const struct dirent *ent);

int main(int argc, char** argv) {

	//This stores all the arguments
	struct arg_list args = {0, NULL};

	// Parse flags and store them in args
	int c;
	while((c = getopt(argc, argv, "a")) != -1) {
		switch(c) {
			case 'a':
				args.show_hidden = 1;
				break;
		}
	}

	/*
	 * Iterate over all args, looking for those
	 * that do not begin with '-', those are 
	 * directories that need to be printed out
	 */
	int i;
	int j = 0; //Counts location in dir list
	char** dl = malloc((argc - 1) * sizeof(char**));
	if(dl == NULL) exit(1);
	for(i = 1; i < argc; i++) {
		//Check for not flag
		if(argv[i][0] != '-') {
			dl[j] = argv[i];
			j++;
		}
	}
	dl[j] = NULL; //NULL terminator 
	args.dir_list = dl; //Set directory list in args

	itr_ent(argv[1], args);
}

/*
 * Iterate over directory entries
 */
void itr_ent(char* dir_name, struct arg_list args) {
	struct dirent **eps;

	// Iterate over each directory in arg_list
	int i = 0;
	while(1) {
		char* dir = args.dir_list[i]; //get curren
		if(dir == NULL) {
			if(i == 0) {
				dir = "./";
			} else {
				break;
			}
		}
		int d;
		if(args.show_hidden == 1) {
			d  = scandir(dir, &eps, one, alphasort);
		} else {
			d = scandir(dir, &eps, no_dot, alphasort);
		}
		
		if(d >= 0) {
			int n;
			for(n = 0; n < d; n++) {
				printf("%s  ", eps[n]->d_name);
			}
			printf("\n");
		} else {
			printf("ls: cannot access %s: No such file or directory",
							dir_name);
		}

		i++;
	}
}

int no_dot(const struct dirent *ent) {
	if(ent->d_name[0] != '.') {
		return 1;
	} else {
		return 0;
	}
}

int one(const struct dirent *ent) {
	return 1;
}
