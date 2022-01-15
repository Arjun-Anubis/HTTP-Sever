CC= clang
LIBS= 
HEADERDIR=header
CFLAGS=-Wall -g -DLOG_USE_COLOR -I$(HEADERDIR)

SRCDIR=src
SRCS:=main.c log.c network.c http.c
SRCS:=$(addprefix $(SRCDIR)/,$(SRCS) )

BUILDDIR=build
OBJS:=log.o network.o http.o
OBJS:=$(addprefix $(BUILDDIR)/,$(OBJS) )

EXE=main

all: $(EXE)


$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(SRCDIR)/main.c -o $(EXE) $(LIBS) $(OBJS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $(BUILDDIR)/$(notdir $@) $(LIBS)

clean:
	$(RM) $(EXE) $(OBJS)
