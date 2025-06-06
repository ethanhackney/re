CFLAGS = -std=c11 		\
	 -Wall  		\
	-Werror                 \
	-Wextra                 \
	-Wconversion            \
	-Wsign-conversion       \
	-Wshadow                \
	-Wstrict-prototypes     \
	-Wpointer-arith         \
	-Wcast-align            \
	-Wuninitialized         \
	-Winit-self             \
	-Wundef                 \
	-Wredundant-decls       \
	-Wwrite-strings         \
	-Wformat=2              \
	-Wswitch-enum           \
	-Wstrict-overflow=5     \
	-Wno-unused-parameter   \
	-pedantic
FFLAGS  = $(CFLAGS) -O3
DFLAGS  = $(CFLAGS) -DDEBUG -fsanitize=address,undefined
SRC     = $(wildcard *.c)
CC      = gcc

safe:
	$(CC) $(DFLAGS) $(SRC)

fast:
	$(CC) $(FFLAGS) $(SRC)
