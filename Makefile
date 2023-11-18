#!/bin/make
OUTDIR	= ./build
TARGET	= $(OUTDIR)/trie
SOURCES	= $(wildcard *.cpp)
OBJECTS	= $(addprefix $(OUTDIR)/, $(SOURCES:.cpp=.o))

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(TARGET)

.PHONY: run
run: $(TARGET)
	./build/trie

$(TARGET): $(OBJECTS) Makefile
	$(CXX) -o $(TARGET) $(OBJECTS)

$(OUTDIR)/%.o: %.cpp Makefile
	mkdir -p build
	$(CXX) -o $@ -c $<

.PHONY: trie-svg
trie-svg: patricia_trie.dot
	dot -Tsvg patricia_trie.dot > output.svg