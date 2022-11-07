
.SUFFIXES:	.o .c .cpp .cc .C .a


RM = -rm

.cpp.o:	
	$(C++) $(CCFLAGS) $*.cpp
.c.o:
	$(CC) $(CCFLAGS) $*.c

#############################################################

C++        = g++
CC         = gcc
CCFLAGS    = -c -I. -w


VPATH   = Obj

OBJECTS1 = emd.o sampleEMD.o

default: sampleEMD

sampleEMD: $(OBJECTS1)
	-mv $(OBJECTS1) Obj
	cd Obj ; $(C++) -o ../sampleEMD $(OBJECTS1)  -framework OpenGL -framework GLUT; cd .

clean:
	$(RM) $(VPATH)/*.o sampleEMD

 

