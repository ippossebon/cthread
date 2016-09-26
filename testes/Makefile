AR=ar
CRS=crs
CC=gcc
EXE_DIR=../exemplos
LIB_DIR=../lib
INC_DIR=../include
BIN_DIR=../bin
SRC_DIR=../src
CFLAGS= -Wall -c
OBJETOS= teste_escalonador teste_dispatcher teste_ccreate teste_cjoin teste_cyield teste1 teste2 teste3 teste4 teste5 prodcons filosofos barbeiro mandel series teste_vetor

all: teste_dispatcher teste_ccreate teste_cjoin teste_cyield

teste_escalonador: teste_escalonador.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste_escalonador $(BIN_DIR)/support.o teste_escalonador.c $(SRC_DIR)/cthread.c -Wall

teste_dispatcher: teste_dispatcher.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste_dispatcher $(BIN_DIR)/support.o teste_dispatcher.c $(SRC_DIR)/cthread.c -Wall

teste_ccreate: teste_ccreate.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste_ccreate $(BIN_DIR)/support.o teste_ccreate.c $(SRC_DIR)/cthread.c -Wall

teste_cjoin: teste_cjoin.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste_cjoin $(BIN_DIR)/support.o teste_cjoin.c $(SRC_DIR)/cthread.c -Wall

teste_cyield: teste_cyield.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste_cyield $(BIN_DIR)/support.o teste_cyield.c $(SRC_DIR)/cthread.c -Wall

teste1: teste1.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste1 $(BIN_DIR)/support.o teste1.c $(SRC_DIR)/cthread.c -Wall

teste2: teste2.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(SRC_DIR)/cthread.c
	$(CC) -g -o teste2 $(BIN_DIR)/support.o teste2.c $(SRC_DIR)/cthread.c -Wall

teste3: teste3.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste3 $(BIN_DIR)/support.o teste3.c $(SRC_DIR)/cthread.c -Wall

teste4: teste4.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste4 $(BIN_DIR)/support.o teste4.c $(SRC_DIR)/cthread.c -Wall

teste5: teste5.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h
	$(CC) -g -o teste5 $(BIN_DIR)/support.o teste5.c $(SRC_DIR)/cthread.c -Wall

prodcons: $(EXE_DIR)/prodcons.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(SRC_DIR)/cthread.c
	$(CC) -g -o prodcons $(BIN_DIR)/support.o $(EXE_DIR)/prodcons.c $(SRC_DIR)/cthread.c -Wall

filosofos: $(EXE_DIR)/filosofos.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(SRC_DIR)/cthread.c
	$(CC) -g -o filosofos $(BIN_DIR)/support.o $(EXE_DIR)/filosofos.c $(SRC_DIR)/cthread.c -Wall

teste_vetor: $(EXE_DIR)/teste_vetor.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(SRC_DIR)/cthread.c
	$(CC) -g -o teste_vetor $(BIN_DIR)/support.o $(EXE_DIR)/teste_vetor.c $(SRC_DIR)/cthread.c -Wall

series: $(EXE_DIR)/series.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(SRC_DIR)/cthread.c
	$(CC) -g -o series $(BIN_DIR)/support.o $(EXE_DIR)/series.c $(SRC_DIR)/cthread.c -Wall

mandel: $(EXE_DIR)/mandel.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(SRC_DIR)/cthread.c
	$(CC) -g -o mandel $(BIN_DIR)/support.o $(EXE_DIR)/mandel.c $(SRC_DIR)/cthread.c -Wall

barbeiro: $(EXE_DIR)/barbeiro.c $(INC_DIR)/support.h $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(SRC_DIR)/cthread.c
	$(CC) -g -o barbeiro $(BIN_DIR)/support.o $(EXE_DIR)/barbeiro.c $(SRC_DIR)/cthread.c -Wall

clean:
	rm -rf $(OBJETOS)
