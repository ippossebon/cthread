#
# Makefile ESQUELETO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "fila2.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
#

AR=ar
CRS=crs
CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TST_DIR=./testes
CFLAGS= -Wall -c
OBJETOS= $(BIN_DIR)/cthread.o

all: cthread
	$(AR) $(CRS) $(LIB_DIR)/libcthread.a $(BIN_DIR)/cthread.o

cthread: $(SRC_DIR)/cthread.c $(INC_DIR)/cthread.h $(INC_DIR)/cdata.h $(INC_DIR)/support.h
	$(CC) -g -o $(BIN_DIR)/cthread.o $(CFLAGS) $(SRC_DIR)/cthread.c


# Para usar a lib, basta acrescentar a linha de baixo e tirar o cthread.c
# -L./lib/ -lcthread
clean:
	rm $(LIB_DIR)/libcthread.a
	rm -rf $(OBJETOS)
