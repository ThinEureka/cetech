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

# Utility rule file for ContinuousBuild.

# Include the progress variables for this target.
include src/CMakeFiles/ContinuousBuild.dir/progress.make

src/CMakeFiles/ContinuousBuild:
	cd /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build/src && /usr/bin/ctest -D ContinuousBuild

ContinuousBuild: src/CMakeFiles/ContinuousBuild
ContinuousBuild: src/CMakeFiles/ContinuousBuild.dir/build.make

.PHONY : ContinuousBuild

# Rule to build all files generated by this target.
src/CMakeFiles/ContinuousBuild.dir/build: ContinuousBuild

.PHONY : src/CMakeFiles/ContinuousBuild.dir/build

src/CMakeFiles/ContinuousBuild.dir/clean:
	cd /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build/src && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousBuild.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/ContinuousBuild.dir/clean

src/CMakeFiles/ContinuousBuild.dir/depend:
	cd /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++ /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/src /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build/src /home/ondra/Projects/cyberego/cetech1/3rdparty/capnproto/c++/build/src/CMakeFiles/ContinuousBuild.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/ContinuousBuild.dir/depend

