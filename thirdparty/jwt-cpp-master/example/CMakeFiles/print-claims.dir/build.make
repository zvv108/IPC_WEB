# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/firefly/Documents/software/jwt-cpp-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/firefly/Documents/software/jwt-cpp-master

# Include any dependencies generated for this target.
include example/CMakeFiles/print-claims.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include example/CMakeFiles/print-claims.dir/compiler_depend.make

# Include the progress variables for this target.
include example/CMakeFiles/print-claims.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/print-claims.dir/flags.make

example/CMakeFiles/print-claims.dir/print-claims.cpp.o: example/CMakeFiles/print-claims.dir/flags.make
example/CMakeFiles/print-claims.dir/print-claims.cpp.o: example/print-claims.cpp
example/CMakeFiles/print-claims.dir/print-claims.cpp.o: example/CMakeFiles/print-claims.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/firefly/Documents/software/jwt-cpp-master/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/CMakeFiles/print-claims.dir/print-claims.cpp.o"
	cd /home/firefly/Documents/software/jwt-cpp-master/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT example/CMakeFiles/print-claims.dir/print-claims.cpp.o -MF CMakeFiles/print-claims.dir/print-claims.cpp.o.d -o CMakeFiles/print-claims.dir/print-claims.cpp.o -c /home/firefly/Documents/software/jwt-cpp-master/example/print-claims.cpp

example/CMakeFiles/print-claims.dir/print-claims.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/print-claims.dir/print-claims.cpp.i"
	cd /home/firefly/Documents/software/jwt-cpp-master/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/firefly/Documents/software/jwt-cpp-master/example/print-claims.cpp > CMakeFiles/print-claims.dir/print-claims.cpp.i

example/CMakeFiles/print-claims.dir/print-claims.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/print-claims.dir/print-claims.cpp.s"
	cd /home/firefly/Documents/software/jwt-cpp-master/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/firefly/Documents/software/jwt-cpp-master/example/print-claims.cpp -o CMakeFiles/print-claims.dir/print-claims.cpp.s

# Object files for target print-claims
print__claims_OBJECTS = \
"CMakeFiles/print-claims.dir/print-claims.cpp.o"

# External object files for target print-claims
print__claims_EXTERNAL_OBJECTS =

example/print-claims: example/CMakeFiles/print-claims.dir/print-claims.cpp.o
example/print-claims: example/CMakeFiles/print-claims.dir/build.make
example/print-claims: /usr/lib/aarch64-linux-gnu/libssl.so
example/print-claims: /usr/lib/aarch64-linux-gnu/libcrypto.so
example/print-claims: example/CMakeFiles/print-claims.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/firefly/Documents/software/jwt-cpp-master/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable print-claims"
	cd /home/firefly/Documents/software/jwt-cpp-master/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/print-claims.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/print-claims.dir/build: example/print-claims
.PHONY : example/CMakeFiles/print-claims.dir/build

example/CMakeFiles/print-claims.dir/clean:
	cd /home/firefly/Documents/software/jwt-cpp-master/example && $(CMAKE_COMMAND) -P CMakeFiles/print-claims.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/print-claims.dir/clean

example/CMakeFiles/print-claims.dir/depend:
	cd /home/firefly/Documents/software/jwt-cpp-master && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/firefly/Documents/software/jwt-cpp-master /home/firefly/Documents/software/jwt-cpp-master/example /home/firefly/Documents/software/jwt-cpp-master /home/firefly/Documents/software/jwt-cpp-master/example /home/firefly/Documents/software/jwt-cpp-master/example/CMakeFiles/print-claims.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/print-claims.dir/depend

