#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int offset = 65;

static char vig_lookup_enc(char c, char k){
	/* Skip special characters */
	if(c < 65 || c > 122) return c;
	if(c > 90 && c < 97) return c;
	/* Transform to capital letters */
	if(c > 96 && c < 123) c -= 32;
	if(k > 96 && k < 123) k -= 32;
	//fprintf(stdout, "k = %d, c = %d, k+c = %d, mod26 = %d, +offset = %d\n", k, c, k+c, (k+c)%26, (k+c)%26+offset);
	return (k+c)%26+offset;
}

static char vig_lookup_dec(char c, char k){
	/* Skip special characters */
	if(c < 65 || c > 122) return c;
	if(c > 90 && c < 97) return c;
	/* Transform to capital letters */
	if(c > 96 && c < 123) c -= 32;
	if(k > 96 && k < 123) k -= 32;

	int col = c-k;
	if(col < 0) col = 26+col;
	return col+offset;
}

void vig_print_enc(){
	for(int i = 0; i < 26; i++){
		for(int j = 0; j < 26; j++){
			printf("%c ", vig_lookup_enc(j+offset, i+offset));
		}
		printf("\n");
	}
}

void vig_print_dec(){
	for(int i = 0; i < 26; i++){
		for(int j = 0; j < 26; j++){
			printf("%c ", vig_lookup_dec(j+offset, i+offset));
		}
		printf("\n");
	}
}

char* vig_dec(const char *text, const char *key){

	int key_pos = 0;
	int key_len = strlen(key);

	int text_pos = 0;
	int text_len = strlen(text);

	char *res = malloc(text_len * sizeof(char) + 1);
	if(!res) return 0;

	for(int i = 0; i < text_len; i++){
		if((key[key_pos] < 65 || key[key_pos] > 122) || (key[key_pos] > 90 && key[key_pos] < 97)) return 0;
		res[text_pos] = vig_lookup_dec(text[text_pos], key[key_pos]);
		//printf("lookup of %c as key and %c gave: %c\n", key[key_pos], text[text_pos], res[text_pos]);
		key_pos = (key_pos+1)%key_len;
		text_pos++;
	}

	res[text_len] = '\0';

	return res;

}

char* vig_enc(const char *text, const char *key){

	int key_pos = 0;
	int key_len = strlen(key);

	int text_pos = 0;
	int text_len = strlen(text);

	char *res = malloc(text_len * sizeof(char) + 1);
	if(!res) return 0;

	for(int i = 0; i < text_len; i++){
		if((key[key_pos] < 65 || key[key_pos] > 122) || (key[key_pos] > 90 && key[key_pos] < 97)) return 0;
		res[text_pos] = vig_lookup_enc(text[text_pos], key[key_pos]);
		//printf("lookup of %c as key and %c gave: %c\n", key[key_pos], text[text_pos], res[text_pos]);
		key_pos = (key_pos+1)%key_len;
		text_pos++;
	}

	res[text_len] = '\0';

	return res;
}
