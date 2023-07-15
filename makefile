CC			= gcc
CFLAGS 		= -ansi -pedantic -Wall
LDFLAGS		= -lm
PROG_NAME 	= mmn14
ZIP_NAME	= $(PROG_NAME).zip
ARGS		= test2

BUILD_DIR 	= build
OBJ_DIR		= $(BUILD_DIR)/obj
BIN_DIR		= $(BUILD_DIR)/bin



ifdef debug
CFLAGS += -g
endif

ifdef cml
CFLAGS += -fsanitize=address
endif

.PHONY: clean build_env all run

all: build_env $(PROG_NAME)

$(PROG_NAME): main.o lexer.o preprocessor.o assembler.o trie.o vector.o
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o -o $(BIN_DIR)/$@ $(LDFLAGS)

main.o: main.c assembler/assembler.h global/defines.h
preprocessor.o: preprocessor/preprocessor.c \
 preprocessor/../data_structures/vector/vector.h \
 preprocessor/../data_structures/trie/trie.h \
 preprocessor/../global/defines.h preprocessor/../global/dir_ins_names.h \
 preprocessor/../global/defines.h preprocessor/preprocessor.h
assembler.o: assembler/assembler.c assembler/assembler.h \
 assembler/../lexer/lexer.h assembler/../lexer/../global/defines.h \
 assembler/../data_structures/vector/vector.h \
 assembler/../data_structures/trie/trie.h assembler/../global/defines.h
trie.o: data_structures/trie/trie.c data_structures/trie/trie.h
vector.o: data_structures/vector/vector.c data_structures/vector/vector.h
lexer.o: lexer/lexer.c lexer/lexer.h lexer/../global/defines.h

%.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)

build_env:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

zip: clean 
	rm -f $(ZIP_NAME)
	zip -r $(ZIP_NAME) *

run:
	build/bin/mmn14 $(ARGS)
	






