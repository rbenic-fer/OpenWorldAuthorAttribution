CC=g++
CFLAGS=-std=c++11

default: owaa_ngram owaa_dict

owaa_ngram: owaa_ngram.cpp
	$(CC) $(CFLAGS) -o owaa_ngram owaa_ngram.cpp

owaa_dict: owaa_dict.cpp
	$(CC) $(CFLAGS) -o owaa_dict owaa_dict.cpp

clean:
	$(RM) owaa_ngram
	$(RM) owaa_dict
