CC 		  := gcc
CFLAGS  := -Wall -Wno-deprecated-declarations -g -I include

# static libraries package .o files 
# @ compile time, linker lazy fetches symbols when linking tests
# useful for self contained EXEs that dont change
# larger exe but faster runtime execution
AR := ar -rc 		 # archive tool: bundles OBJs into .a file 
RANLIB := ranlib # indexes archive for quicker symbol lookups

SRCDIR   := src
TESTDIR  := test
INCDIR 	 := include
BENCHDIR := benchmarks
## OBJECT FILES ##
SRC_OBJS := $(SRCDIR)/queue.o \
						$(SRCDIR)/scheduler.o \
						$(SRCDIR)/timer.o \
						$(SRCDIR)/tracker.o \
						$(SRCDIR)/thread-worker.o 

TEST_HELPERS_OBJS := $(TESTDIR)/test_helpers.o
## OBJECT FILES ##

## EXE FILES ##
TEST_EXES := $(TESTDIR)/test_queue \
						 $(TESTDIR)/test_timer \
						 $(TESTDIR)/test_tracker \
						 $(TESTDIR)/test_worker

BENCH_EXES := $(BENCHDIR)/parallel_cal
## EXE FILES ## 


# mark `all` and `test` as always-run targets
.PHONY: all test bench

## RULES ##

# make rule format: 
	# target : dependencies (deps) 
	# 	optional build steps
	
	####
	
	# $@: target
	# $^: all deps 
	# $<: 1st dep 

all: clean thread-worker.a $(TEST_EXES)


thread-worker.a: $(SRC_OBJS)
	$(AR) $@ $^	 # archives all deps (src OBJ files) into ,a file
	$(RANLIB) $@ # indexes archive for quicker symbol lookups 

#TODO: default to PSJF for now 
SCHED ?= PSJF
${SRCDIR}/scheduler.o: ${SRCDIR}/scheduler.c ${INCDIR}/scheduler.h 
ifeq ($(SCHED), PSJF)
	$(CC) -pthread $(CFLAGS) -c -DPSJF $< -o $@
else ifeq ($(SCHED), MLFQ)
	$(CC) -pthread $(CFLAGS) -c -DMLFQ $< -o $@ 
else ifeq ($(SCHED), CFS)
	$(CC) -pthread $(CFLAGS) -c -DCFS  $< -o $@
#else
	#$(CC) -pthread $(CFLAGS) -c -DPSJF $< -o $@
endif

# create any target OBJ file from src 
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	# %: filters deps to those with matching pattern in target
	# -c $<: passes the 1st matching dep to be compiled (not linked)
	# -o $@: passes the name of the target object file 
	$(CC) $(CFLAGS) -pthread -c $< -o $@ -I$(INCDIR)
																	  		
# compile test helper to OBJ
$(TESTDIR)/test_helpers.o: $(TESTDIR)/test_helpers.c
	$(CC) $(CFLAGS) -pthread -c $^ -o $@

#links (i.e. builds EXE) with library and helper objects
# implicit steps:
	# compiles the corresponding .c file
	# links it with symbols from OBJ files in .a and test_helpers.o
	# outputs a single EXE file
$(TESTDIR)/%: $(TESTDIR)/%.c thread-worker.a $(TEST_HELPERS_OBJS)
	# $^: all dependencies are passed to stitch the test EXE together
	# without the -c file, the linker is invoked
	$(CC) $(CFLAGS) -pthread $^ -o $@

# build all test EXEs and run them
# before running, make all must be satisfied 
	# remove old build files
	# build static library (archive file) thread-worker.a
	# build (compile + link) all test EXEs
	
# to test: sequentially loop over each EXE and invoke it (./{EXE})
	# $$t: designates a shell variable ($t is a make variable)
	# make escapes the first $ yielding shell cmd $t (./{EXE} in shell)

test: all
	@for t in $(TEST_EXES); do \
		echo "\nrunning $$t"; \
		$$t || exit 1; \
	done

bench: all $(BENCH_EXES)

$(BENCHDIR)/%: $(BENCHDIR)/%.c thread-worker.a
	$(CC)	$(CFLAGS) -pthread $^ -o $@

clean: 
	rm -f thread-worker.a $(SRC_OBJS) $(TEST_HELPERS_OBJS) $(TEST_EXES) $(BENCH_EXES)

## RULES ## 

