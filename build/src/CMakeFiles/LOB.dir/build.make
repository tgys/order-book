# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /nix/store/5cl0n3viqbgfgh0zaizaag6p3s5444x6-cmake-3.19.7/bin/cmake

# The command to remove a file.
RM = /nix/store/5cl0n3viqbgfgh0zaizaag6p3s5444x6-cmake-3.19.7/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tei/projects/limit_order_book

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tei/projects/limit_order_book/build

# Include any dependencies generated for this target.
include src/CMakeFiles/LOB.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/LOB.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/LOB.dir/flags.make

src/CMakeFiles/LOB.dir/order_book.cc.o: src/CMakeFiles/LOB.dir/flags.make
src/CMakeFiles/LOB.dir/order_book.cc.o: ../src/order_book.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tei/projects/limit_order_book/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/LOB.dir/order_book.cc.o"
	cd /home/tei/projects/limit_order_book/build/src && /nix/store/88ghxafjpqp5sqpd75r51qqg4q5d95ss-gcc-wrapper-10.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/LOB.dir/order_book.cc.o -c /home/tei/projects/limit_order_book/src/order_book.cc

src/CMakeFiles/LOB.dir/order_book.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LOB.dir/order_book.cc.i"
	cd /home/tei/projects/limit_order_book/build/src && /nix/store/88ghxafjpqp5sqpd75r51qqg4q5d95ss-gcc-wrapper-10.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tei/projects/limit_order_book/src/order_book.cc > CMakeFiles/LOB.dir/order_book.cc.i

src/CMakeFiles/LOB.dir/order_book.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LOB.dir/order_book.cc.s"
	cd /home/tei/projects/limit_order_book/build/src && /nix/store/88ghxafjpqp5sqpd75r51qqg4q5d95ss-gcc-wrapper-10.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tei/projects/limit_order_book/src/order_book.cc -o CMakeFiles/LOB.dir/order_book.cc.s

# Object files for target LOB
LOB_OBJECTS = \
"CMakeFiles/LOB.dir/order_book.cc.o"

# External object files for target LOB
LOB_EXTERNAL_OBJECTS =

src/libLOB.so: src/CMakeFiles/LOB.dir/order_book.cc.o
src/libLOB.so: src/CMakeFiles/LOB.dir/build.make
src/libLOB.so: src/CMakeFiles/LOB.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tei/projects/limit_order_book/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libLOB.so"
	cd /home/tei/projects/limit_order_book/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LOB.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/LOB.dir/build: src/libLOB.so

.PHONY : src/CMakeFiles/LOB.dir/build

src/CMakeFiles/LOB.dir/clean:
	cd /home/tei/projects/limit_order_book/build/src && $(CMAKE_COMMAND) -P CMakeFiles/LOB.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/LOB.dir/clean

src/CMakeFiles/LOB.dir/depend:
	cd /home/tei/projects/limit_order_book/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tei/projects/limit_order_book /home/tei/projects/limit_order_book/src /home/tei/projects/limit_order_book/build /home/tei/projects/limit_order_book/build/src /home/tei/projects/limit_order_book/build/src/CMakeFiles/LOB.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/LOB.dir/depend

