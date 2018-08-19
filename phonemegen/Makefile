CXX      = g++
LDFLAGS  = -lpulse -lpulse-simple -lespeak
INCLUDE  =
CXXFLAGS = -std=gnu++11 -Wall -Wextra -Wfatal-errors -g
CXXFLAGS += $(INCLUDE)
TARGET   = ipa2chip
SRCFILES = main.cpp
OBJECTS  = $(patsubst %.cpp, %.o, $(SRCFILES))

all: $(TARGET)

.c.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

clean:
	rm -f *.o $(TARGET)
