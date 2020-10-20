#
# S-Box Makefile
#

INCDIRS = 
LIBS	=
XFLAGS	= -Wall -static
CFLAGS	= $(INCDIRS) $(XFLAGS)

#Target ALL
all: sbox

.PHONY: makedirs
makedirs:
	@mkdir -p obj 

#compile .c files to .o files
obj/%.o: %.c makedirs
	@echo "  CC     `basename $@`"
	@$(CC) $(CFLAGS) -c $< -ggdb3 -o $@

#Clean the outputs
.PHONY: clean
clean:
	@rm -rf obj/*.o
	@rm sbox

# All dependencies
SBOX_OBJS = obj/sbox.o
SBOX_OBJS += obj/util.o
SBOX_OBJS += obj/linkedlist.o
SBOX_OBJS += obj/memop.o
SBOX_OBJS += obj/memeater.o
SBOX_OBJS += obj/benchmark.o
sbox: $(SBOX_OBJS)
	@echo "  CC [B] `basename $@`"
	@$(CC) $(CFLAGS) $(SBOX_OBJS) $(LIBS) -o sbox

MEM_EATER_OBJS = obj/util.o
MEM_EATER_OBJS += obj/memeater.o
MEM_EATER_OBJS += obj/genmain.o
memeater: CFLAGS += -DUTIL=memeater_main
memeater: $(MEM_EATER_OBJS) obj/genmain.o
	@echo "  CC [B] `basename $@`"
	@$(CC) $(CFLAGS) $(MEM_EATER_OBJS) $(LIBS) -o memeater

LL_OBJS = obj/linkedlist.o
LL_OBJS += obj/util.o
LL_OBJS += obj/genmain.o
lltest: CFLAGS += -DUTIL=lltest_main
lltest: $(LL_OBJS) obj/genmain.o
	@echo "  CC [B] `basename $@`"
	@$(CC) $(CFLAGS) $(LL_OBJS) $(LIBS) -o lltest

UTIL_OBJS = obj/util.o
UTIL_OBJS += obj/genmain.o
util: CFLAGS += -DUTIL=util_main
util: $(UTIL_OBJS) obj/genmain.o
	@echo "  CC [B] `basename $@`"
	@$(CC) $(CFLAGS) $(UTIL_OBJS) $(LIBS) -o utils

BENCHMARK_OBJS = obj/benchmark.o
BENCHMARK_OBJS += obj/util.o
BENCHMARK_OBJS += obj/genmain.o
benchmark: CFLAGS += -DUTIL=benchmark_main
benchmark: $(BENCHMARK_OBJS) obj/genmain.o
	@echo "  CC [B] `basename $@`"
	@$(CC) $(CFLAGS) $(BENCHMARK_OBJS) $(LIBS) -o benchmark
