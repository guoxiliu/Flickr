CC = gcc
CFLAGS = -Wall -g

# project name and header files
PROJECT = flickr
HEADERS = utils.h louvain.h label_propagation.h suggestion.h

# path relevant with igraph
IGRAPH_INCLUDE_PATH = /home/guoxil/usr/local/include/igraph
IGRAPH_LIB_PATH = /home/guoxil/usr/local/lib

compile: ${PROJECT}.out

flickr.out: ${PROJECT}.c ${HEADERS}
	${CC} ${CFLAGS} -o ${PROJECT}.out ${PROJECT}.c -I${IGRAPH_INCLUDE_PATH} -L${IGRAPH_LIB_PATH} -ligraph

run: ${PROJECT}.out
	./${PROJECT}.out

clean:
	rm *.out
