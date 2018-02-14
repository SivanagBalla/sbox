#
# S-Box Makefile
#

INCDIRS = 
LIBS	=
XFLAGS	= -Wall
CFLAGS	= $(INCDIRS) $(XFLAGS)

#Target ALL
all: sbox

#compile .c files to .o files
%.o: %.c
	@echo "  CC     `basename $@`"
	@$(CC) $(CFLAGS) -c $< -o $@

#Clean the outputs
.PHONY: clean
clean:
	@rm -rf *.o

# All dependencies
SBOX_OBJS = sbox.o
SBOX_OBJS += util.o
SBOX_OBJS += memop.o
SBOX_OBJS += memeater.o

sbox: $(SBOX_OBJS)
	@echo "  CC [B] `basename $@`"
	@$(CC) $(CFLAGS) $(SBOX_OBJS) $(LIBS) -o sbox
