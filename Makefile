CXXFLAGS = -c -std=c++11 -O2
CXXFLAGS_DEBUG = -c -std=c++11 -g -O0
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
OBJ = obj
DBG_OBJ = obj/debug
LIB_OBJ = obj/lib
SRC = src
INC = -I inc

OBJS = $(OBJ)/main.o $(OBJ)/gameboy.o $(OBJ)/cpu.o $(OBJ)/interrupts.o $(OBJ)/lcd_driver.o $(OBJ)/timer.o $(OBJ)/memory.o $(OBJ)/joypad.o $(OBJ)/dma.o $(OBJ)/sfml_interface.o $(OBJ)/cmd_options.o $(OBJ)/audio_controller.o $(OBJ)/wav_header.o $(OBJ)/sfml_audio.o $(OBJ)/serial.o $(OBJ)/rtc.o

DBG_OBJS = $(DBG_OBJ)/main.o $(DBG_OBJ)/gameboy.o $(DBG_OBJ)/cpu.o $(DBG_OBJ)/interrupts.o $(DBG_OBJ)/lcd_driver.o $(DBG_OBJ)/timer.o $(DBG_OBJ)/memory.o $(DBG_OBJ)/joypad.o $(DBG_OBJ)/dma.o $(DBG_OBJ)/sfml_interface.o $(DBG_OBJ)/cmd_options.o $(DBG_OBJ)/audio_controller.o $(DBG_OBJ)/wav_header.o $(DBG_OBJ)/sfml_audio.o $(DBG_OBJ)/serial.o $(DBG_OBJ)/rtc.o

LIB_OBJS = $(LIB_OBJ)/gameboy.o $(LIB_OBJ)/cpu.o $(LIB_OBJ)/interrupts.o $(LIB_OBJ)/lcd_driver.o $(LIB_OBJ)/timer.o $(LIB_OBJ)/memory.o $(LIB_OBJ)/joypad.o $(LIB_OBJ)/dma.o $(LIB_OBJ)/audio_controller.o $(LIB_OBJ)/wav_header.o $(LIB_OBJ)/serial.o $(LIB_OBJ)/rtc.o

LIB_AUDIO = $(LIB_OBJ)/sfml_audio.o


kuszli-gb: pre $(OBJS)
	g++ $(OBJS) $(LIBS) -o kuszli-gb

debug: pre_dbg $(DBG_OBJS)
	g++ $(DBG_OBJS) $(LIBS) -o kuszli-gb
	
install: pre_install $(LIB_OBJS) $(LIB_AUDIO)
	mkdir -p  /usr/local/lib/kuszli-gb
	g++ -shared $(LIB_OBJS) -o /usr/local/lib/kuszli-gb/libkuszli-gb.so
	g++ -shared $(LIB_AUDIO) -o /usr/local/lib/kuszli-gb/libkuszli-gb-audio.so
	mkdir -p /usr/local/include/kuszli-gb
	cp inc/*.hh /usr/local/include/kuszli-gb && cp inc/*.h /usr/local/include/kuszli-gb

$(OBJ)/%.o: $(SRC)/%.cpp 
	g++ $(CXXFLAGS) $(INC) $< -o $@

$(DBG_OBJ)/%.o: $(SRC)/%.cpp 
	g++ $(CXXFLAGS_DEBUG) $(INC) $< -o $@

$(LIB_OBJ)/%.o: $(SRC)/%.cpp
	mkdir -p obj/lib
	g++ $(CXXFLAGS) -fPIC -O2 $(INC) $< -o $@

pre:
	mkdir -p obj

pre_dbg:
	mkdir -p obj/debug

pre_install:
	mkdir -p obj/lib

clean:
	rm -f $(OBJ)/*.o
	rm -f $(DBG_OBJ)/*.o
	rm -f $(LIB_OBJ)/*.o


