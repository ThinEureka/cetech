# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.3

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build

# Utility rule file for NightlySubmit.

# Include the progress variables for this target.
include src/CMakeFiles/NightlySubmit.dir/progress.make

src/CMakeFiles/NightlySubmit:
	cd /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build/src && /usr/bin/ctest -D NightlySubmit

NightlySubmit: src/CMakeFiles/NightlySubmit
NightlySubmit: src/CMakeFiles/NightlySubmit.dir/build.make

.PHONY : NightlySubmit

# Rule to build all files generated by this target.
src/CMakeFiles/NightlySubmit.dir/build: NightlySubmit

.PHONY : src/CMakeFiles/NightlySubmit.dir/build

src/CMakeFiles/NightlySubmit.dir/clean:
	cd /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build/src && $(CMAKE_COMMAND) -P CMakeFiles/NightlySubmit.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/NightlySubmit.dir/clean

src/CMakeFiles/NightlySubmit.dir/depend:
	cd /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++ /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/src /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build/src /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build/src/CMakeFiles/NightlySubmit.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/NightlySubmit.dir/depend

