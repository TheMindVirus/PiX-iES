# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/alastair/sample/cube

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alastair/sample/cube/build

# Include any dependencies generated for this target.
include CMakeFiles/cube.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cube.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cube.dir/flags.make

CMakeFiles/cube.dir/main.c.obj: CMakeFiles/cube.dir/flags.make
CMakeFiles/cube.dir/main.c.obj: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alastair/sample/cube/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/cube.dir/main.c.obj"
	/home/alastair/sample/pspdev/bin/psp-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cube.dir/main.c.obj   -c /home/alastair/sample/cube/main.c

CMakeFiles/cube.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cube.dir/main.c.i"
	/home/alastair/sample/pspdev/bin/psp-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alastair/sample/cube/main.c > CMakeFiles/cube.dir/main.c.i

CMakeFiles/cube.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cube.dir/main.c.s"
	/home/alastair/sample/pspdev/bin/psp-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alastair/sample/cube/main.c -o CMakeFiles/cube.dir/main.c.s

# Object files for target cube
cube_OBJECTS = \
"CMakeFiles/cube.dir/main.c.obj"

# External object files for target cube
cube_EXTERNAL_OBJECTS = \
"/home/alastair/sample/cube/logo.o"

cube: CMakeFiles/cube.dir/main.c.obj
cube: ../logo.o
cube: CMakeFiles/cube.dir/build.make
cube: CMakeFiles/cube.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alastair/sample/cube/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable cube"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cube.dir/link.txt --verbose=$(VERBOSE)
	/usr/bin/cmake -E cmake_echo_color --cyan Not\ stripping\ binary,\ build\ type\ is\ .
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Calling psp-fixup-imports"
	/home/alastair/sample/pspdev/bin/psp-fixup-imports /home/alastair/sample/cube/build/cube
	/usr/bin/cmake -E cmake_echo_color --cyan Not\ building\ PRX
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Calling mksfoex"
	/home/alastair/sample/pspdev/bin/mksfoex -d MEMSIZE=1 cube PARAM.SFO
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Calling pack-pbp with ELF file"
	/home/alastair/sample/pspdev/bin/pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL /home/alastair/sample/cube/build/cube NULL
	/usr/bin/cmake -E cmake_echo_color --cyan EBOOT.PBP\ file\ created.

# Rule to build all files generated by this target.
CMakeFiles/cube.dir/build: cube

.PHONY : CMakeFiles/cube.dir/build

CMakeFiles/cube.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cube.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cube.dir/clean

CMakeFiles/cube.dir/depend:
	cd /home/alastair/sample/cube/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alastair/sample/cube /home/alastair/sample/cube /home/alastair/sample/cube/build /home/alastair/sample/cube/build /home/alastair/sample/cube/build/CMakeFiles/cube.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cube.dir/depend

