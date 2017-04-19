INCLUDES	= 
SRCS		= triou2doxygen.cpp
PROG		= $(TRUST_ROOT)/exec/triou2doxygen

LIBDIR        = 
INCDIR        = 

CFLAGS		= -O3 -g
LFLAGS		= 
CC		= $(TRUST_CC)

all: $(PROG)
	#strip $?

$(PROG): $(SRCS)
	$(CC) -o $@ $(CFLAGS) $(LFLAGS) $(SRCS)

clean:
	rm -f $(PROG) *~ core

.SUFFIXES : .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
