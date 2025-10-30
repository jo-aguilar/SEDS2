CXX = g++
TARGET = estados
SRC = seds2.cpp
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET)

run:
	clear && ./$(TARGET)

clean: 
	rm -f $(TARGET)
