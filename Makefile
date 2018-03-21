CC = gcc
CFLAGS += -g -O3 -std=c99 -pedantic -Wall -Werror -Wno-error=unused-function -Wno-error=unused-but-set-variable -pthread
CPPFLAGS += -D_XOPEN_SOURCE=700 -D_FORTIFY_SOURCE=2

all: bf vig_enc vig_dec docu

bf: vigenere.o bf.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

vig_enc: vigenere.o vig_enc.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

vig_dec: vigenere.o vig_dec.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

docu: docu.tex
	pdflatex docu.tex

clean:
	rm -f *.o 

.PHONY: clean all
