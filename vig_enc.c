#include "vigenere.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define INIT_SIZE 4096

//#define DEBUG

/*
* returns 1 if opt is set
* or 0 if opt is not set
*/
static int opt_is_set(char *opt, int argc, char **argv){
	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], opt) == 0) return 1;
	}
	return 0;
}

/*
* Returns position of parameter in argument array
* or -1 if opt is set but parameter missing
* or 0 if opt is not set at all
*/
static int get_param(char *opt, int argc, char **argv){
	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], opt) == 0){
			if(i == argc-1) return -1;
			if(strncmp(argv[i+1], "--", 2) == 0) return -1;
			else return i+1;
		}
	}
	return 0;
}

void usage(char *name){
	fprintf(stdout, "%s [--in infile] [--out outfile] --password password\n", name);
	exit(EXIT_FAILURE);
}

static void dbg(char *str){
	#ifdef DEBUG
	printf("%s\n", str);
	#endif
}

static void err(char *str){
	fprintf(stderr, "%s\n", str);
	exit(1);
}

int main(int argc, char **argv){
	int tmp;
	char *file;
	char *key = 0;
	/*
	* Check if password was specified	
	*/
	tmp = 0;
	if(opt_is_set("--password", argc, argv)){
		tmp = get_param("--password", argc, argv);
		if(tmp > 0){
			key = argv[tmp];
			dbg("Password specified");
		}else if(tmp == -1){
			dbg("Password opt set, but no password specified");
			usage(argv[0]);
		}
	}else{
		dbg("No password specified");
		usage(argv[0]);
	}

	/*
	* Check if input-file was specified	
	*/
	tmp = 0;
	FILE *fp_in = stdin;
	if(opt_is_set("--in", argc, argv)){
		tmp = get_param("--in", argc, argv);
		if(tmp > 0){
			file = argv[tmp];
			dbg("File specified");
			fp_in = fopen(file, "r");
			if(!fp_in){
				perror("fopen");
				exit(1);
			}
		}else if(tmp == -1){
			dbg("File opt set, but no file specified");
			usage(argv[0]);
		}
	}else{
		dbg("No file specified, using stdin");
	}
	/*
	* Check if output-file was specified	
	*/
	tmp = 0;
	FILE *fp_out = stdout;
	if(opt_is_set("--out", argc, argv)){
		tmp = get_param("--out", argc, argv);
		if(tmp > 0){
			file = argv[tmp];
			dbg("File specified");
			fp_out = fopen(file, "w");
			if(!fp_out){
				perror("fopen");
				exit(1);
			}
		}else if(tmp == -1){
			dbg("File opt set, but no file specified");
			usage(argv[0]);
		}
	}else{
		dbg("No file specified, using stdout");
	}

	char *text = malloc(INIT_SIZE * sizeof(char));
	if(!text){
		perror("bf: text malloc");
		exit(EXIT_FAILURE);
	}
	char c;
	int len = 0;
	int size = INIT_SIZE;
	while((c = fgetc(fp_in)) != EOF){
		/* Skip special characters */
		if(c < 65 || c > 122) continue;
		if(c > 90 && c < 97) continue;
		text[len] = c;
		len++;
		if(len == size){
			size*=2;
			text = realloc(text, size);
		}
	}

	char *enc = vig_enc(text, key);
	if(enc == 0){
		fprintf(stderr, "Error encrypting\n");
	}else{
		fprintf(fp_out, "%s", vig_enc(text, key));
	}

	exit(EXIT_FAILURE);
}
