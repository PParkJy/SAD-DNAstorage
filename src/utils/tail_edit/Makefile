SRC_DIR= src
INC_DIR= src
OBJECT_FILES= trie.o tail.o
SOURCE_FILES= main-tail.c

OBJECTS= $(addprefix $(SRC_DIR)/,$(OBJECT_FILES))
SOURCES= $(addprefix $(SRC_DIR)/,$(SOURCE_FILES))
INCLUDES= $(addprefix -I, $(INC_DIR))

# Defaults.
CC= gcc
CFLAGS= -std=c99 -Wall -Wextra
LDLIBS= -lpthread -lm

# Development and debug flags.
DEV_CFLAGS= -g -O0 -Wunused-parameter -Wredundant-decls \
	-Wreturn-type -Wswitch-default -Wunused-value -Wimplicit \
	-Wimplicit-function-declaration -Wimplicit-int -Wimport \
	-Wunused  -Wunused-function -Wunused-label -Wbad-function-cast \
	-Wno-int-to-pointer-cast -Wpointer-sign -Wnested-externs \
	-Wold-style-definition -Wstrict-prototypes -Wredundant-decls \
	-Wunused -Wunused-function -Wunused-parameter -Wunused-value \
	-Wformat -Wunused-variable -Wformat-nonliteral -Wparentheses \
	-Wundef -Wsequence-point -Wuninitialized -Wbad-function-cast

GPROF_CFLAGS= -pg -O0

# Release flags.
REL_CFLAGS= -O3 -DNDEBUG

# General rules.
all: tail-release

# Compilation environments.
tail-release: CFLAGS += $(REL_CFLAGS)
tail-release: tailored_clustering

# Compilation targets.
tailored_clustering: $(OBJECTS) $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(OBJECTS) $(LDLIBS) -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

