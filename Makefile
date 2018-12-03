CC = gcc
CFLAGS = -Wall -g
IGRAPH_INCLUDE_PATH = /home/guoxil/usr/local/include/igraph
IGRAPH_LIB_PATH = /home/guoxil/usr/local/lib
PROJECT = flickr

compile: ${PROJECT}.out

flickr.out: ${PROJECT}.c utils.h louvain.h
	${CC} ${CFLAGS} -o ${PROJECT}.out ${PROJECT}.c -I${IGRAPH_INCLUDE_PATH} -L${IGRAPH_LIB_PATH} -ligraph

run: ${PROJECT}.out
	./${PROJECT}.out

clean:
	rm *.out
