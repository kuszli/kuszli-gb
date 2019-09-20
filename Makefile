CXXFLAGS = -fPIC -c -std=c++11 
LIBS = -lsfml-graphics -lsfml-window -lsfml-system
OBJ = obj
SRC = src
INC = -I inc

OBJS = $(OBJ)/main.o $(OBJ)/gameboy.o $(OBJ)/cpu.o $(OBJ)/interrupts.o $(OBJ)/lcd_driver.o $(OBJ)/timer.o $(OBJ)/memory.o $(OBJ)/joypad.o $(OBJ)/dma.o $(OBJ)/sfml_interface.o $(OBJ)/cmd_options.o

LIB_OBJS = $(OBJ)/gameboy.o $(OBJ)/cpu.o $(OBJ)/interrupts.o $(OBJ)/lcd_driver.o $(OBJ)/timer.o $(OBJ)/memory.o $(OBJ)/joypad.o $(OBJ)/dma.o 

kuszli-gb: $(OBJS)
	g++ $(OBJS) $(LIBS) -o kuszli-gb

install: $(LIB_OBJS)
	mkdir -p  /usr/local/lib/kuszli-gb
	g++ -shared $(LIB_OBJS) -o /usr/local/lib/kuszli-gb/libkuszli-gb.so
	mkdir -p /usr/local/include/kuszli-gb
	cp inc/*.hh /usr/local/include/kuszli-gb && cp inc/*.h /usr/local/include/kuszli-gb

$(OBJ)/main.o: $(SRC)/main.cpp
	g++ $(CXXFLAGS) $(INC) $(SRC)/main.cpp -o $(OBJ)/main.o

$(OBJ)/gameboy.o: $(SRC)/gameboy.cpp
	g++ $(CXXFLAGS) $(INC) $(SRC)/gameboy.cpp -o $(OBJ)/gameboy.o

$(OBJ)/cpu.o: $(SRC)/cpu.cpp 
	g++ $(CXXFLAGS) $(INC) $(SRC)/cpu.cpp -o $(OBJ)/cpu.o

$(OBJ)/interrupts.o: $(SRC)/interrupts.cpp 
	g++ $(CXXFLAGS) $(INC) $(SRC)/interrupts.cpp -o $(OBJ)/interrupts.o

$(OBJ)/lcd_driver.o: $(SRC)/lcd_driver.cpp 
	g++ $(CXXFLAGS) $(INC) $(SRC)/lcd_driver.cpp -o $(OBJ)/lcd_driver.o

$(OBJ)/timer.o: $(SRC)/timer.cpp 
	g++ $(CXXFLAGS) $(INC) $(SRC)/timer.cpp -o $(OBJ)/timer.o

$(OBJ)/memory.o: $(SRC)/memory.cpp
	g++ $(CXXFLAGS) $(INC) $(SRC)/memory.cpp -o $(OBJ)/memory.o

$(OBJ)/joypad.o: $(SRC)/joypad.cpp
	g++ $(CXXFLAGS) $(INC) $(SRC)/joypad.cpp -o $(OBJ)/joypad.o

$(OBJ)/dma.o: $(SRC)/dma.cpp
	g++ $(CXXFLAGS) $(INC) $(SRC)/dma.cpp -o $(OBJ)/dma.o

$(OBJ)/sfml_interface.o: $(SRC)/sfml_interface.cpp
	g++ $(CXXFLAGS) $(INC) $(SRC)/sfml_interface.cpp -o $(OBJ)/sfml_interface.o

$(OBJ)/cmd_options.o: $(SRC)/cmd_options.cpp
	g++ $(CXXFLAGS) $(INC) $(SRC)/cmd_options.cpp -o $(OBJ)/cmd_options.o

clean:
	rm -f $(OBJ)/*.o


