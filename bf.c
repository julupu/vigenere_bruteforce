#include "vigenere.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//#define DEBUG

#define PWD_LENGTH 6
#define WORD_LENGTH 64
#define INIT_SIZE 4096

#define MODE_MAX 0
#define MODE_THRESHOLD 1

/*
This combination takes about ~37 seconds per 1000 tries
#define CUTPOS 100
#define THRESHOLD 10
*/

/*
This combination takes about ~45 seconds per 1000 tries
#define CUTPOS 300
#define THRESHOLD 30
*/

/*
* This combination takes about ~60 seconds per 1000 tries
#define CUTPOS 750
#define THRESHOLD 75
*/

/*
This combination takes about ~73 seconds per 1000 tries
#define CUTPOS 1000
#define THRESHOLD 100
*/

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

static void dbg(char *str){
	#ifdef DEBUG
	printf("%s\n", str);
	#endif
}

static void err(char *str){
	fprintf(stderr, "%s\n", str);
	exit(1);
}

void usage(char *prog){
	fprintf(stdout, "%s [--mode max|threshold] [--threshold t] [--chars c] [--in infile] [--out outfile] --passwords passwordlist --words wordlist\n", prog);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int tmp;
	char *file;
	/*
	* Check if mode was specified	
	*/
	tmp = 0;
	int mode = MODE_MAX;
	if(opt_is_set("--mode", argc, argv)){
		tmp = get_param("--mode", argc, argv);
		if(tmp > 0){
			if(strcmp(argv[tmp], "threshold")==0){
				mode = MODE_THRESHOLD;
				dbg("Threshold mode specified");
			}else if(strcmp(argv[tmp], "max")==0){
				mode = MODE_MAX;
				dbg("Max mode specified");
			}else{
				dbg("Unknown mode specified");
				usage(argv[0]);
			}
		}else if(tmp == -1){
			dbg("Mode opt set, but no mode specified");
			usage(argv[0]);
		}
	}else{
		dbg("No mode specified, using max");
	}

	/*
	* If mode is threshold, require threshold
	*/
	int threshold = 0;
	if(mode == MODE_THRESHOLD){
		tmp = 0;
		if(opt_is_set("--threshold", argc, argv)){
			tmp = get_param("--threshold", argc, argv);
			if(tmp > 0){
				threshold = atoi(argv[tmp]);
			}else if(tmp == -1){
				dbg("Threshold opt set, but no value specified");
				usage(argv[0]);
			}
		}else{
			threshold = 10;
			dbg("No threshold specified, running with 10");
		}
	}else if(mode == MODE_MAX && opt_is_set("--threshold", argc, argv)){
		fprintf(stderr, "WARNING: Mode is max, ignoring option threshold\n");
	}

	/*
	* Check number of chars to check
	*/
	tmp = 0;
	int chars = 0;
	if(opt_is_set("--chars", argc, argv)){
		tmp = get_param("--chars", argc, argv);
		if(tmp > 0){
			chars = atoi(argv[tmp]);
		}else if(tmp == -1){
			dbg("Chars opt set, but no value specified");
			usage(argv[0]);
		}
	}else{
		chars = 100;
		dbg("No chars specified, running with 100");
	}

	/*
	* Passwordlist
	*/
	tmp = 0;
	FILE *fp_passwords;
	if(opt_is_set("--passwords", argc, argv)){
		tmp = get_param("--passwords", argc, argv);
		if(tmp > 0){
			file = argv[tmp];
			dbg("Passwords specified");
			fp_passwords = fopen(file, "r");
			if(!fp_passwords){
				perror("fopen");
				exit(1);
			}
		}else if(tmp == -1){
			dbg("passwords opt set, but no file specified");
			usage(argv[0]);
		}
	}else{
		dbg("No password file specified");
		usage(argv[0]);
	}
	/*
	* Wordlist
	*/
	tmp = 0;
	FILE *fp_words;
	if(opt_is_set("--words", argc, argv)){
		tmp = get_param("--words", argc, argv);
		if(tmp > 0){
			file = argv[tmp];
			dbg("Words specified");
			fp_words = fopen(file, "r");
			if(!fp_words){
				perror("fopen");
				exit(1);
			}
		}else if(tmp == -1){
			dbg("words opt set, but no file specified");
			usage(argv[0]);
		}
	}else{
		dbg("No word file specified");
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
		//if(c == '\n') continue;
		text[len] = c;
		len++;
		if(len == size){
			size*=2;
			text = realloc(text, size);
		}
	}

	char *enc = text;

	char *pwd_buffer = malloc(PWD_LENGTH*sizeof(char)+2);
	if(!pwd_buffer){
		perror("bf: malloc pwd_buffer");
		exit(EXIT_FAILURE);
	}
	
	char *word_buffer = malloc(WORD_LENGTH*sizeof(char)+2);
	if(!word_buffer){
		perror("bf: malloc word_buffer");
		exit(EXIT_FAILURE);
	}

	int counter = 0;
	int found_counter = 0;
	char *res;
	char cutpos_sav = enc[chars];
	enc[chars] = '\0';
	time_t start, end;
	start = time(NULL);
	int max_counter = 0;
	char cur_pwd[PWD_LENGTH+1];
	while(fgets(pwd_buffer, PWD_LENGTH+2, fp_passwords)){
		if(pwd_buffer[strlen(pwd_buffer)-1] == '\n'){
			pwd_buffer[strlen(pwd_buffer)-1] = '\0';
		}
		//fprintf(stdout, "Checking with %s\n", pwd_buffer);
		res = vig_dec(enc, pwd_buffer);
		if(res == 0) err("Error decrypting\n");

		while(fgets(word_buffer, WORD_LENGTH+2, fp_words)){


			if(word_buffer[strlen(word_buffer)-1] == '\n'){
				word_buffer[strlen(word_buffer)-1] = '\0';

			}

			if(strlen(word_buffer) < 5) continue;
			//printf("Res is %s, compared word is %s\n", res, word_buffer);
			char *substr = res;
			while((substr = strstr(substr, word_buffer)) != 0){
				//printf("Found: %s\n", word_buffer);
				found_counter++;
				substr++;
				//exit(EXIT_SUCCESS);
			}

			if(mode == MODE_THRESHOLD && found_counter >= threshold){
				fprintf(stdout, "Got more than THRESHOLD hits (%d), guess that's enough for now\nPassword is:%s\n", found_counter, pwd_buffer);
				enc[chars] = cutpos_sav;
				char *dec = vig_dec(enc, pwd_buffer);
				if(dec == 0) err("Error decrypting\n");
				fprintf(fp_out, "%s", dec);
				exit(EXIT_SUCCESS);
			}
				
		}
		//fprintf(stdout, "Found %d words, strlen(text) is %d, threshold is %d\n", found_counter, (int) strlen(text), threshold);
		rewind(fp_words);
		counter++;
		if(counter%1000 == 0){
			end = time(NULL);
			fprintf(stdout, "Checked %d words\n", counter);
			fprintf(stdout, "Took %d seconds\n", (int) (end-start));
			start = time(NULL);
		}

		if(found_counter > max_counter){
			max_counter = found_counter;
			strcpy(cur_pwd, pwd_buffer);
		}
		found_counter = 0;
	}

	if(mode == MODE_THRESHOLD){
		printf("Nothing found\n");
	}else{
		printf("Maximum found words: %d, password is: %s\n", max_counter, cur_pwd);
		enc[chars] = cutpos_sav;
		char *dec = vig_dec(enc, cur_pwd);
		if(dec == 0) err("Error decrypting\n");
		fprintf(fp_out, "%s", dec);
	}
	exit(EXIT_FAILURE);
}
