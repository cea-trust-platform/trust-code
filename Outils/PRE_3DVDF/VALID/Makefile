CXX=g++ -g

OCC_INCLUDES=-I$(CASROOT)/inc
OCC_LDFLAGS=-L$(CASROOT)/Linux/lib
LIB_OCC=   -lTKTopAlgo

CXXFLAGS=$(OCC_INCLUDES) 
LDFLAGS=$(OCC_LDFLAGS)
LIBS=  $(LIB_OCC)  -ldl

SOURCES = postverif.cxx
OBJECTS = postverif.o
TARGET = postverif

all:  $(TARGET) test_occ

$(TARGET): $(OBJECTS)  postverif.cxx Makefile
	$(CXX)   $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS) 
postverif.o: postverif.cxx
	$(CXX) postverif.cxx -c $(CXXFLAGS) -o postverif.o

test_occ:test.cxx
	$(CXX) $(CXXFLAGS)  test.cxx -o  test_occ $(LDFLAGS) $(LIBS) 
clean:
	rm -f *.o $(TARGET) test_occ
	rm -rf model prepro
