
.SUFFIXES:	.o .c .cpp .cc .C .a


RM = -rm


.cpp.o:	
	$(C++) $(CCFLAGS) $*.cpp



#############################################################

C++        = g++
CCFLAGS    = -c -Iinclude


VPATH   = Obj

OBJECTS = Grid.o UnstructuredGrid.o UnstructuredGrid3D.o plyfile.o sampleDecimation.o PlyReader.o MeshDecimator.o MeshRenderer.o zpr.o

default: sampleDecimation

sampleDecimation: $(OBJECTS) 
	-mv $(OBJECTS) Obj
	cd Obj ; $(C++) -o ../sampleDecimation $(OBJECTS)  -framework OpenGL -framework GLUT; cd .


clean:
	$(RM) $(VPATH)/*.o sampleDecimation
 
