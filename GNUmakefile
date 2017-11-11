# This file was automatically generated by bakefile.
#
# Any manual changes will be lost if it is regenerated,
# modify the source .bkl file instead if possible.

# You may define standard make variables such as CFLAGS or
# CXXFLAGS to affect the build. For example, you could use:
#
#      make CXXFLAGS=-g
#
# to build with debug information. The full list of variables
# that can be used by this makefile is:
# AR, CC, CFLAGS, CPPFLAGS, CXX, CXXFLAGS, LD, LDFLAGS, MAKE, RANLIB.

# You may also specify config=Debug|Release
# or their corresponding lower case variants on make command line to select
# the corresponding default flags values.
ifeq ($(config),debug)
override config := Debug
endif
ifeq ($(config),release)
override config := Release
endif
ifeq ($(config),Debug)
CPPFLAGS ?= -DDEBUG
CFLAGS ?= -g -O0
CXXFLAGS ?= -g -O0
LDFLAGS ?= -g
else ifeq ($(config),Release)
CPPFLAGS ?= -DNDEBUG 
CFLAGS ?= -O2
CXXFLAGS ?= -O2
else ifneq (,$(config))
$(warning Unknown configuration "$(config)")
endif

# Use "make RANLIB=''" for platforms without ranlib.
RANLIB ?= ranlib

CXX := g++

CPPFLAGS = -std=c++14

# The directory for the build files, may be overridden on make command line.
builddir = .

all: $(builddir)/test

$(builddir)/test: $(builddir)/test_BridgeControl.o $(builddir)/test_LightsControl.o $(builddir)/test_HueApp.o $(builddir)/test_Main.o $(builddir)/test_Session.o $(builddir)/test_User.o
	$(CXX) -o $@ $(LDFLAGS) $(builddir)/test_BridgeControl.o $(builddir)/test_LightsControl.o $(builddir)/test_HueApp.o $(builddir)/test_Main.o $(builddir)/test_Session.o $(builddir)/test_User.o -lwt -lwthttp -lboost_system -lwtdbo -lwtdbosqlite3 -lcrypt -pthread

$(builddir)/test_HueApp.o: HueApp.C
	$(CXX) -c -o $@ $(CPPFLAGS) $(CFLAGS) -MD -MP -pthread HueApp.C

$(builddir)/test_Main.o: Main.C
	$(CXX) -c -o $@ $(CPPFLAGS) $(CFLAGS) -MD -MP -pthread Main.C

$(builddir)/test_Session.o: Session.C
	$(CXX) -c -o $@ $(CPPFLAGS) $(CFLAGS) -MD -MP -pthread Session.C

$(builddir)/test_User.o: User.C
	$(CXX) -c -o $@ $(CPPFLAGS) $(CFLAGS) -MD -MP -pthread User.C

$(builddir)/test_LightsControl.o: LightsControl.C
	$(CXX) -c -o $@ $(CPPFLAGS) $(CFLAGS) -MD -MP -pthread LightsControl.C 

$(builddir)/test_BridgeControl.o: BridgeControl.C
	$(CXX) -c -o $@ $(CPPFLAGS) $(CFLAGS) -MD -MP -pthread BridgeControl.C 

clean:
	rm -f *.o
	rm -f *.d
	rm -f $(builddir)/test

start:
	./test --docroot ./ --http-address 127.0.0.1 --http-port 8080

.PHONY: all clean

# Dependencies tracking:
-include *.d
