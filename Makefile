#
# S-Box Makefile
#

INCDIRS = 
LIBS	=
XFLAGS	= -Wall
CFLAGS	= $(INCDIRS) $(XFLAGS)

#Target ALL
all: sbox

.PHONY: makedirs
makedirs:
	@mkdir -p obj 

#compile .c files to .o files
obj/%.o: %.c makedirs
	@echo "  CC     `basename $@`"
	@$(CC) $(CFLAGS) -c $< -o $@

#Clean the outputs
.PHONY: clean
clean:
	@rm -rf obj/*.o
	@rm sbox

# All dependencies
SBOX_OBJS = obj/sbox.o
SBOX_OBJS += obj/util.o
SBOX_OBJS += obj/memop.o
SBOX_OBJS += obj/memeater.o

sbox: $(SBOX_OBJS)
	@echo "  CC [B] `basename $@`"
	@$(CC) $(CFLAGS) $(SBOX_OBJS) $(LIBS) -o sbox
