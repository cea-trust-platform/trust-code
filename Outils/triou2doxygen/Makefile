INCLUDES	= 
OBJS		= triou2doxygen.o
PROG		= triou2doxygen

LIBDIR        = 
INCDIR        = 

CFLAGS		= -O3 -g
LFLAGS		= 
CC		= $(TRUST_CC)

all: $(PROG)
	#strip $?

$(PROG): $(OBJS)
	$(CC) -o $@ $(LFLAGS) $(OBJS)

clean:
	rm -f $(OBJS) $(PROG) *~ core

.SUFFIXES : .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
