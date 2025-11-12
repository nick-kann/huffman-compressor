CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2

TARGET = huffman
SOURCES = main.cpp compressor.cpp huffman.cpp
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: clean

