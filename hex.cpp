//**************************************************************************************************
// hex
//
// This tool reads data from the standard input stream and prints to the standard output stream the
// hexadecimal and ASCII codes for each byte.
//**************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale>


static auto programVersion = "hex v1.1.0  2020-10-03  https://github.com/hollasch/hex\n";

static auto usage = R"(
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
)";


   /***  Type Definitions  ***/

enum class GroupType { Byte, Word, Long, Quad, Oct };

struct ProgramParams {
    GroupType grouping  { GroupType::Long };  // Grouping (Byte, Word or Long)
    bool      compact   { false };            // Compact Duplicate Lines
    long      dataStart { -1L };              // Input Stream Start
    long      dataEnd   { -1L };              // Input Stream End
    int       fileCount { 0 };                // Number of Files to Dump
};


//----------------------------------------------------------------------------------------------------------------------

inline int print (const char *string) {
    return fputs (string, stdout);
}


inline int fprint (FILE *stream, const char *string) {
    return fputs (string, stream);
}


void PrintHelp() {
    print ("");
    print (programVersion);
    print (usage);
}

void PrintVersion() {
    print (programVersion);
}


bool ProcessArgs (ProgramParams& params, int argc, char *argv[]) {

    // This routine processes the command-line arguments. This function returns true if all
    // arguments were successfully processed.

    params.fileCount = 0;

    for (auto argi = 1;  argi < argc;  ++argi) {
        char *swptr;    // Switch Pointer

        // First check to see if the user is prompting for information. Note that I also check
        // forward-slash options for PC folks.

        if (  (0 == strcmp(argv[argi], "/?")) || (0 == strcmp(argv[argi], "-?"))
           || (0 == strcmp(argv[argi], "-h")) || (0 == strcmp(argv[argi], "--help"))
           )
        {
            PrintHelp();
            return false;
        }

        // If the option does not start with a dash, we assume it's a filename, so add that to the
        // list of files.

        if (argv[argi][0] != '-') {
            ++params.fileCount;
            continue;
        }

        swptr = argv[argi] + 1;

        if (*swptr == '-') {
            ++swptr;

            auto gotStart = false;
            auto gotEnd = false;

            if (0 == strcmp(swptr, "byte"))
                params.grouping = GroupType::Byte;
            else if (0 == strcmp(swptr, "word"))
                params.grouping = GroupType::Word;
            else if (0 == strcmp(swptr, "longword"))
                params.grouping = GroupType::Long;
            else if (0 == strcmp(swptr, "quadword"))
                params.grouping = GroupType::Quad;
            else if (0 == strcmp(swptr, "octword"))
                params.grouping = GroupType::Oct;
            else if (0 == strcmp(swptr, "compact"))
                params.compact = true;
            else if (0 == strcmp(swptr, "start"))
                gotStart = true;
            else if (0 == strcmp(swptr, "end"))
                gotEnd = true;
            else if (0 == strcmp(swptr, "version")) {
                PrintVersion();
                exit(0);
            } else {
                fprintf (stderr, "hex: Unknown option (%s).\n", argv[argi]);
                return false;
            }

            if (gotStart || gotEnd) {
                if (argc <= argi+1) {
                    fprintf (stderr, "hex: No argument given to %s option.\n", argv[argi]);
                    return false;
                }

                argv[argi++][0] = 0; // Zap the switch argument.

                auto val = strtoul (argv[argi], nullptr, 0);
                if (gotStart)
                    params.dataStart = val;
                else
                    params.dataEnd = val;
            }

            argv[argi][0] = 0; // Zap the switch argument.

        } else {

            // Handle single-character switch type
            do {
                switch (*swptr) {
                    case 'b':   params.grouping = GroupType::Byte;  break;
                    case 'w':   params.grouping = GroupType::Word;  break;
                    case 'l':   params.grouping = GroupType::Long;  break;
                    case 'q':   params.grouping = GroupType::Quad;  break;
                    case 'o':   params.grouping = GroupType::Oct;   break;

                    case 'c': {
                        params.compact  = true;
                        break;
                    }

                    case 'e': {
                        auto *ptr = swptr+1;

                        if (!*ptr) {
                            argv[argi][0] = 0;
                            ptr = argv[++argi];
                        }

                        if (argc <= argi) {
                            fprint (stderr, "hex: No argument given to -e option.\n");
                            return false;
                        }

                        params.dataEnd = strtoul (ptr, nullptr, 0);
                        swptr = 0;
                        break;
                    }

                    case 's': {
                        auto *ptr = swptr+1;

                        if (!*ptr) {
                            argv[argi][0] = 0;
                            ptr = argv[++argi];
                        }

                        if (argc <= argi) {
                            fprint (stderr, "hex: No argument given to -s option.\n");
                            return false;
                        }

                        params.dataStart = strtoul (ptr, nullptr, 0);
                        swptr = 0;
                        break;
                    }

                    default:
                        fprintf (stderr, "hex: Unknown option (%c).\n", *swptr);
                        return false;
                }

                if (swptr)
                    ++swptr;

            } while (swptr && *swptr);

            argv[argi][0] = 0;
        }

    }

    return true;
}


void Dump (FILE *file, ProgramParams& params) {
    static char templateByte[] { "XX XX XX XX  XX XX XX XX  XX XX XX XX  XX XX XX XX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
    static short locsByte[] { 0,3,6,9, 13,16,19,22, 26,29,32,35, 39,42,45,48, 54, 64 };

    static char templateWord[] { "XXXX XXXX  XXXX XXXX  XXXX XXXX  XXXX XXXX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
    static short locsWord[] { 0,2,5,7, 11,13,16,18, 22,24,27,29, 33,35,38,40, 46, 56 };

    static char templateLong[] { "XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
    static short locsLong[] { 0,2,4,6, 9,11,13,15, 18,20,22,24, 27,29,31,33, 39, 49};

    static char templateQuad[] { "XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
    static short locsQuad[] { 0,2,4,6,8,10,12,14, 17,19,21,23,25,27,29,31, 37, 47 };

    static char templateOct[] { "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
    static short locsOct[] { 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30, 36, 46 };

    static char hexDigits[] { "0123456789abcdef" };

    // Set up according to the grouping type.

    char  *lineTemplate;
    short *locs;

    switch (params.grouping) {
        case GroupType::Byte: lineTemplate = templateByte; locs = locsByte; break;
        case GroupType::Word: lineTemplate = templateWord; locs = locsWord; break;
        case GroupType::Long: lineTemplate = templateLong; locs = locsLong; break;
        case GroupType::Quad: lineTemplate = templateQuad; locs = locsQuad; break;
        case GroupType::Oct:  lineTemplate = templateOct;  locs = locsOct;  break;
    }

    // This procedure dumps a file to standard output.

    if ((params.dataEnd > 0) && (params.dataStart > 0) && (params.dataEnd <= params.dataStart))
        return;

    size_t addr = (params.dataStart > 0) ? params.dataStart : 0;
    auto   redblock = false;

    // If the user specified a start address, then seek to that location.

    if (params.dataStart < 0)
        params.dataStart = 0;
    else if (0 != fseek (file, params.dataStart, 0)) {
        fprint (stderr, "hex: fseek to start position failed.\n");
        return;
    }

    // While a non-zero number of bytes are read in...

    char   buff[16];        // Input Buffer
    char   priorBuff[16];   // Prior Input Buffer
    size_t nbytes;          // Number of Bytes Read In

    while ((0 != (nbytes = fread (buff, 1, 0x10, file))) || redblock) {
        if (params.dataEnd > 0) {
            if ((params.dataEnd <= addr) && !redblock) break;
            if (params.dataEnd < (addr+0x10))
                nbytes = params.dataEnd - addr + 1;
        }

        // If we're in compact print mode, and we're not at the first line, and we have a full line
        // of data, and this data line is that same as the prior one, then represent subsequent
        // duplicate lines with a single line of "====".

        if (  params.compact && (addr != params.dataStart) && (nbytes == 0x10)
           && (0 == memcmp (priorBuff, buff, sizeof(buff)))
           )
        {
            // Print the redundant line marker, but only once per block.

            if (!redblock) {
                fputs ("====\n", stdout);
                redblock = true;
            }
            addr += nbytes;
            continue;
        }

        // The following conditional is true when the input ends in the middle of a redundant block.
        // In this situation, we force the output of the last line of the input. Note that for
        // redblock to be true, the previous number of bytes read in had to be 0x10, so we know that
        // the number of bytes in the last buffer is 0x10. We also decrement the address to adjust
        // for having skipped past the last block.

        if (!nbytes && redblock) {
            nbytes  = 0x10;
            addr   -= 0x10;
        }

        // If we get this far, then we're not in a redundant block.

        redblock = false;

        // Write the current address to the output buffer.

        auto ptr = lineTemplate + locs[16] + 7;
        auto jj = addr;

        for (int i=8;  i != 0;  --i, jj>>=4, --ptr)
            *ptr = hexDigits[jj & 0xf];

        // Write the hexadecimal value of each byte.

        auto t = 0;    // lineTemplate Character Index

        for (t=0;  t < nbytes;  ++t) {
            lineTemplate [locs[t]  ] = hexDigits [ (unsigned char)(buff[t]) >> 4  ];
            lineTemplate [locs[t]+1] = hexDigits [ (unsigned char)(buff[t]) & 0xF ];
        }

        // If we didn't read a full line, then pad to the ASCII section with blank spaces (we need
        // to overwrite the previous charaters).

        for (; t < 0x10;  ++t)
            lineTemplate[locs[t]] = lineTemplate[locs[t]+1] = ' ';

        // Write out the ASCII values of the input buffer.

        for (t=0;  t < nbytes;  ++t)
            lineTemplate [locs[17]+t]
                = ((buff[t] < 0x20) || (0x7E < buff[t]) ? '.' : buff[t]);

        // If we didn't read a full line, then pad the remainder of the ASCII section with blank
        // spaces.

        for (; t < 0x10;  ++t)
            lineTemplate [locs[17]+t] = ' ';

        fputs (lineTemplate, stdout);
        memcpy (priorBuff, buff, sizeof(buff));
        addr += nbytes;
    }
}


int main (int argc, char *argv[]) {

    long argi;  // Command-Line Argument Index

    // Process the command-line arguments.

    ProgramParams params;
    if (!ProcessArgs (params, argc, argv))
        return 1;

    // If no filenames were given, dump the standard input stream.
    if (params.fileCount == 0) {
        Dump (stdin, params);
        return 0;
    }

    for (argi=1;  argi < argc;  ++argi) {

        auto fname = argv[argi];  // File Name

        // Skip over command-line switches.

        if (!*fname) continue;

        // Dump the file.

        FILE *file;     // File Handle

        if (0 != fopen_s(&file, fname,"rb"))
            fprintf (stderr, "hex: Couldn't open \"%s\".\n", fname);
        else {
            if (params.fileCount > 1) printf ("\n%s:\n", fname);
            Dump (file, params);
            fclose (file);
        }
    }

    return 0;
}
