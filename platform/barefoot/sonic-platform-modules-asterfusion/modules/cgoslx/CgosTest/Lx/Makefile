PROJECT_INC = -I. -I.. -I../.. -I../../CgosLib
PROJECT_LIB = -lcgos -L../../CgosLib/Lx
C_source = ../cgostest.c ../generic.c
OPT = -Wall

default:
	gcc $(C_source) -o cgostest $(OPT) $(DEF) $(PROJECT_INC) $(PROJECT_LIB)

clean:
	rm -f cgostest

cleanall: clean

