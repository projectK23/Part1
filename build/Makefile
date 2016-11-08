#######################################
#
#   Makefile for K23-a:Part1
#

EXE=run-d
EXETEST=test.$(EXE)
TESTOUTFILE=test.out

ROOTSRC=../src/
ROOTTEST=../test/
ROOTUNIT=../utest/

MKDIR_P=mkdir -p
OUTDIR=../obj/

GCC=gcc
GCCFLAGS=-Wall -O1 -g -o

CIN=-I ../include

#libraries included folders
CINLIB= \
	-I ../libs/include

STANDARD_LIBS= \
	-lpthread

#debug level
ifeq ($(D),error)
	DEBUG_LEVEL=1
	C99=-std=c99
else 
	ifeq ($(D),trace)
		DEBUG_LEVEL=2
		C99=--std=c99
	else
		ifeq ($(D),all)
			DEBUG_LEVEL=3
			C99=--std=c99
		else
			DEBUG_LEVEL=0
		endif
	endif
endif

#precompile variables
DFLAGS=-DDEBUG_LEVEL=$(DEBUG_LEVEL)

#Test parameters
CINUNIT=-I $(ROOTUNIT)
TEST_C := $(wildcard $(ROOTTEST)*.c)
TEST_O := $(patsubst $(ROOTTEST)%.c, %.o,$(TEST_C))
UNI_C= $(ROOTUNIT)uni.c
UNI_O= $(OUTDIR)uni.o

#Files for executable
MAIN_C=$(ROOTSRC)main/main.c
MAIN_O=$(OUTDIR)main.o

SRC_BUFFER_C=$(ROOTSRC)my_src/Buffer.c
SRC_BUFFER_O=$(OUTDIR)Buffer.o

SRC_NODE_I_C=$(ROOTSRC)my_src/NodeIndex.c
SRC_NODE_I_O=$(OUTDIR)NodeIndex.o

SRC_GRAPH_C= $(ROOTSRC)my_src/graph_operations.c
SRC_GRAPH_O= $(OUTDIR)graph_operations.o

SRC_O=$(SRC_BUFFER_O) $(SRC_NODE_I_O) $(SRC_GRAPH_O)

OBJS=$(MAIN_O) $(SRC_O) $(TEST_O)

# Targets 

ifeq ($(SKIP_TEST),true)
all: directories default
else
all: directories default test_all
endif

directories: $(OUTDIR)

default: $(EXE)

run: $(EXE)
	./$(EXE)

memtest: directories test $(EXE) $(EXETEST) 
	rm -f $(TEST_O)
	valgrind --leak-check=yes -v --dsymutil=yes ./$(EXE)
	valgrind --leak-check=yes -v --dsymutil=yes ./$(EXETEST) reg


test_all: test $(EXETEST)
	rm -f $(TEST_O)
	./$(EXETEST) reg > $(TESTOUTFILE)
	
test:
	echo $(DFLAGS)
	$(foreach file,$(TEST_C),($(GCC) -c $(file) $(CINUNIT) $(CIN));)

$(OUTDIR):
	$(MKDIR_P) $(OUTDIR)

$(EXE): $(MAIN_O) $(SRC_O)
	$(GCC) $(GCCFLAGS) $(EXE) $(MAIN_O) $(SRC_O) $(STANDARD_LIBS)

$(EXETEST): $(TEST_O) $(UNI_O) $(SRC_O) 
	$(GCC) $(GCCFLAGS) $(EXETEST) $(TEST_O) $(UNI_O) $(SRC_O) $(STANDARD_LIBS)
	

$(MAIN_O): $(MAIN_C)
	$(GCC) $(C99) -c $(MAIN_C) -o $(MAIN_O) $(CIN) $(CINLIB) $(DFLAGS)

$(SRC_BUFFER_O): $(SRC_BUFFER_C)
	$(GCC) $(C99) -c $(SRC_BUFFER_C) -o $(SRC_BUFFER_O) $(CIN) $(CINLIB) $(DFLAGS)

$(SRC_NODE_I_O): $(SRC_NODE_I_C)
	$(GCC) $(C99) -c $(SRC_NODE_I_C) -o $(SRC_NODE_I_O) $(CIN) $(CINLIB) $(DFLAGS)

$(SRC_GRAPH_O): $(SRC_GRAPH_C)
	$(GCC) $(C99) -c $(SRC_GRAPH_C) -o $(SRC_GRAPH_O) $(CIN) $(CINLIB) $(DFLAGS)

$(UNI_O): $(UNI_C)
	$(GCC) -c $(UNI_C) -o $(UNI_O) $(CIN)
	

clean:
	rm -rf ${OUTDIR}
	$(foreach file,$(TEST_O),(rm -f file );)	
	rm -f $(EXE) $(EXETEST) $(TESTOUTFILE)
	clear





