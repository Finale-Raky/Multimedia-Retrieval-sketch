
.SUFFIXES:	.o .c .cpp .cc .C .a


RM = -rm


.cpp.o:	
	$(C++) $(CCFLAGS) $*.cpp



#############################################################

C++        = g++
CCFLAGS    = -c -Iinclude -Woff


VPATH   = Obj

OBJECTS6 = Grid.o UnstructuredGrid.o UnstructuredGrid3D.o plyfile.o sampleSubdivide.o PlyReader.o MeshRenderer.o zpr.o

default: sampleSubdivide

sampleSubdivide: $(OBJECTS6)
	-mv $(OBJECTS6) Obj
	cd Obj ; $(C++) -o ../sampleSubdivide $(OBJECTS6)  -framework OpenGL -framework GLUT; cd .



clean:
	$(RM) $(VPATH)/*.o sampleSubdivide
 

