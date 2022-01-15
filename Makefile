CC= clang
LIBS= 
CFLAGS=-Wall -g -DLOG_USE_COLOR
SRCS=main.c log.c network.c
OBJS=log.o network.o
EXE=main

all: $(EXE)


$(EXE):  $(OBJS)
	$(CC) $(CFLAGS) main.c -o $(EXE) $(LIBS) $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< $(LIBS) 

clean:
	$(RM) $(EXE) $(OBJS)
