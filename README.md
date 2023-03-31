hex
====================================================================================================
A hexadecimal dump for the Windows command line.


Summary
--------
This is a simple hexadecimal print utility that reads input from the standard input stream or from
files specified on the command line. See doc/hex.html for usage information.


Usage
------
    hex v1.1.0  2020-10-03  https://github.com/hollasch/hex

    hex:    dumps the contents of a file in hex and ASCII
    usage:  hex [--byte|-b] [--word|-w] [--longword|-l] [--quadword|-q]
                [--octword|-o] [--compact|-c]
                [<-s|--start> <start>] [<--end|-e> <end>] [file] ... [file]

        This tool dumps the contents of a file in hexadecimal and ascii. If no
        filenames are supplied, hex reads from the standard input stream.

        --byte,-b      Display output grouped by bytes
        --word,-w      Display output grouped by words (16-bits)
        --longword,-l  Display output grouped by longwords (32-bits)
        --quadword,-q  Display output grouped by quadwords (64-bits)
        --octword,-o   Display output grouped by octwords (128-bits)

        --compact, -c
            Compact duplicate lines. Blocks of identical data are represented by the
            first line of data followed by a single line of \"====\".

        --start <address>, -s <address>
        --end <address>, -e <address>
            Start/end the dump at the given location. To specify octal, prefix with
            '0'. To specify hexadecimal, prefix with '0x'. For example, 200 = 0310 =
            0xc8.

        --version
            Display version information


Description
------------
`hex` is a filter that reads data from either standard input (if no filenames are supplied) or from
the list of files, and dumps the data in hexadecimal and ASCII. Note that options may be prefixed
with either a `'-'` or with a `'/'` character, and that you can put spaces between the switch letter
and the argument (for example, "`-e200`" or "`-e 200`").

To specify hexadecimal values, prefix with "`0x`". to specify octal values, prefix with "`0`"
(zero). for example, `200` = `0310` = `0xc8`.</p>

**Important**: From the command line, DOS does redirection streams as text, rather than binary. This
means that if you want to look at a binary data file, you should probably specify the filename on
the command line (for example, `hex foo.xyz`) rather than as a redirection (as in `hex <foo.xyz`).
In the latter case, a ctrl-z character looks like an end-of-file, and your binary files will likely
appear much shorter than they really are.


Examples
---------

To dump the contents of file `frotz.zzz` to screen:

    hex frotz.abc | more

To dump the words from 0x200 to 0x240 for files _file1_, _file2_ and _file3_:

    hex --start 0x200 --end 0x240 --word file1 file2 file3


Building
---------
This project uses `CMake` to build the hex tool. Among other sources, you can find CMake at
https://cmake.org/.

To configure this project for the default configuration for your machine, go to the root of this
project and run the command

    cmake -B build

This will create a new directory, `build/`, which will contain all of the project output, and the
configured build setup.

To build, run

    cmake --build build

This will build the debug version. To build the release version, run

    cmake --build build --config Release

You will find the built executable in `build/Debug` or `build/Release`.


Installation (Windows)
-----------------------
`hex.exe` is a standalone executable. Copy it to any location on your command path and use as-is.
No other installation steps are required.


----
Steve Hollasch (steve@hollasch.net)<br>
https://github.com/hollasch/hex
