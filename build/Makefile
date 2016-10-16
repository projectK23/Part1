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
GCCFLAGS=-Wall -O3 -o

CIN=-I ../include

#libraries included folders
CINLIB= \
	-I ../libs/log/include

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

SRC_C=$(ROOTSRC)my_src/src.c
SRC_O=$(OUTDIR)src.o

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
	valgrind --leak-check=yes ./$(EXE)
	valgrind --leak-check=yes ./$(EXETEST) reg


test_all: test $(EXETEST)
	rm -f $(TEST_O)
	./$(EXETEST) reg > $(TESTOUTFILE)
	
test:
	echo $(DFLAGS)
	$(foreach file,$(TEST_C),($(GCC) -c $(file) $(CINUNIT) $(CIN));)

$(OUTDIR):
	$(MKDIR_P) $(OUTDIR)

$(EXE): $(MAIN_O) $(SRC_O)
	$(GCC) $(GCCFLAGS) $(EXE) $(MAIN_O) $(SRC_O)

$(EXETEST): $(TEST_O) $(UNI_O) $(SRC_O) 
	$(GCC) $(GCCFLAGS) $(EXETEST) $(TEST_O) $(UNI_O) $(SRC_O)
	

$(MAIN_O): $(MAIN_C)
	$(GCC) $(C99) -c $(MAIN_C) -o $(MAIN_O) $(CIN) $(CINLIB) $(DFLAGS)

$(SRC_O): $(SRC_C)
	$(GCC) $(C99) -c $(SRC_C) -o $(SRC_O) $(CIN) $(CINLIB) $(DFLAGS)

$(UNI_O): $(UNI_C)
	$(GCC) -c $(UNI_C) -o $(UNI_O) $(CIN)
	

clean:
	rm -rf ${OUTDIR}
	$(foreach file,$(TEST_O),(rm -f file );)	
	rm -f $(EXE) $(EXETEST) $(TESTOUTFILE)
	clear





