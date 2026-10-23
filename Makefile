CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
CPPFLAGS += -Iinclude
CORE = src/Image.cpp src/Filters.cpp
.PHONY: all test run clean
all: bin/app bin/run_tests
bin:
	mkdir -p bin
bin/app: $(CORE) src/Source.cpp include/Image.h include/Filters.h | bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CORE) src/Source.cpp -o $@
bin/run_tests: $(CORE) tests/tests_image.cpp tests/tests_filters.cpp include/Image.h include/Filters.h | bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CORE) tests/tests_image.cpp tests/tests_filters.cpp -o $@
test: bin/run_tests
	mkdir -p test_output
	./bin/run_tests
run: bin/app
	./bin/app
clean:
	rm -rf bin
