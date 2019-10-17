hex
================================================================================
A hexadecimal dump for the Windows command line.


Summary
--------
This is a simple hexadecimal print utility that reads input from the standard
input stream or from files specified on the command line. See doc/hex.html for
usage information.


Usage
------
    hex [-?] [-b|w|l|q|o] [-c] [-s<start>] [-e<end>] [file] ... [file]


Description
------------
`hex` is a filter that reads data from either standard input (if no filenames
are supplied) or from the list of files, and dumps the data in hexadecimal and
ASCII. Note that options may be prefixed with either a `'-'` or with a `'/'`
character, and that you can put spaces between the switch letter and the
argument (for example, "`-e200`" or "`-e 200`").

To specify hexadecimal values, prefix with "`0x`". to specify octal values,
prefix with "`0`" (zero). for example, `200` = `0310` = `0xc8`.</p>

Options include the following:

| Option | Description
|:------:|:--------------------------------------------------------------------
|  `-?`  | Display help information
|  `-b`  | Display output grouped by bytes (1 byte / 8 bits)
|  `-w`  | Display output grouped by words (2 bytes / 16 bits)
|  `-l`  | Display output grouped by longwords \[default\] (4 bytes / 32 bits)
|  `-q`  | Display output grouped by quadwords (8 bytes / 64 bits)
|  `-o`  | Display output grouped by octwords (16 bytes / 128 bits)
|  `-s`  | Start the dump at the given location (octal, decimal or hex)
|  `-e`  | End the dump at the given location (octal, decimal or hex)

**Important**: From the command line, DOS does redirection streams as text,
rather than binary. This means that if you want to look at a binary data file,
you should probably specify the filename on the command line (for example, `hex
foo.xyz`) rather than as a redirection (as in `hex >foo.xyz`). In the latter
case, a ctrl-z character looks like an end-of-file, and your binary files will
likely appear much shorter than they really are.


Examples
---------

To dump the contents of file `frotz.zzz` to screen:

    hex frotz.zzz | more

To dump the words from 0x200 to 0x240 for files _file1_, _file2_ and _file3_:

    hex -s0x200 -e 0x240 -w file1 file2 file3


Building
---------
This project uses `CMake` to build the hex tool. Among other sources, you can
find CMake at https://cmake.org/.

To configure this project for the default configuration for your machine, go to
the root of this project and run the command

    cmake -B build

This will create a new directory, `build/`, which will contain all of the
project output, and the configured build setup.

To build, run

    cmake --build build

This will build the debug version. To build the release version, run

    cmake --build build --config Release

You will find the built executable in `build/Debug` or `build/Release`.


Installation
-------------
`hex.exe` is the single output file for this tool. Copy it to any location on
your command and use as-is. There is no other installation.


----
Steve Hollasch, steve@hollasch.net
https://github.com/hollasch/hex
