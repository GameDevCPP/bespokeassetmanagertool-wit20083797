CC := g++
LIBS=-lsfml-graphics -lsfml-window -lsfml-system -ltgui
INC=-I "C:/SFML_MW/include" -I"C:/TGUI_MW/include"
LIBFILES=-L"C:/SFML_MW/lib" -L"C:/TGUI_MW/lib"
SRC_FILES := $(wildcard *.cpp)
HEADER_SRC_FILES := $(wildcard *.hpp)

all: sfmlProg1

sfmlProg1: main.o
	$(CC) $(SRC_FILES) $(HEADER_SRC_FILES) -o main $(LIBFILES) $(LIBS) $(INC)
main.o: main.cpp
	$(CC) -c $(SRC_FILES) $(HEADER_SRC_FILES) $(INC) $(LIBS)
clean:
	del *.o sfmlProg1
	
	