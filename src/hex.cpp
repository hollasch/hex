//**************************************************************************************************
//
// hex
//
// This tool reads data from the standard input stream and prints to the standard output stream the
// hexadecimal and ASCII codes for each byte.
//
//**************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale>


static char usage[] {
"\n"
"hex  /  2015.04.27  /  Steve Hollasch\n"
"\n"
"hex:      Dumps the contents of a file in hex and ASCII.\n"
"usage:    hex [-bwlqo] [-c] [-s <start>] [-e <end>] [file] ... [file]\n"
"\n"
"    This tool dumps the contents of a file in hexadecimal and ascii.\n"
"    If no filenames are supplied, hex reads from the standard input stream.\n"
"    To specify hex, prefix with 0x, to specify octal, prefix with 0. For\n"
"    example, 200 = 0310 = 0xc8.\n"
"\n"
"    -b  Display output grouped by bytes.\n"
"    -w  Display output grouped by words (16-bits).\n"
"    -l  Display output grouped by longwords (32-bits).\n"
"    -q  Display output grouped by quadwords (64-bits).\n"
"    -o  Display output grouped by octwords (128-bits).\n"
"\n"
"    -c  Compact duplicate lines. Blocks of identical data are represented\n"
"        by the first line of data followed by a single line of \"====\".\n"
"\n"
"    -s  Start the dump at the given location (octal, decimal or hex).\n"
"    -e  End the dump at the given location (octal, decimal or hex).\n"
"\n"
};


   /***  Type Definitions  ***/

enum GroupType { Group_Byte, Group_Word, Group_Long, Group_Quad, Group_Oct };


   /***  Local Function Declarations  ***/

void   Dump             (FILE*, long, long);
short  ProcessArgs      (int, char*[]);
long   ReadPositiveLong (char *string);


   /***  Data Tables  ***/

char  b_template[] { "XX XX XX XX  XX XX XX XX  XX XX XX XX  XX XX XX XX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
short b_locs[]     { 0,3,6,9, 13,16,19,22, 26,29,32,35, 39,42,45,48, 54, 64 };

char  w_template[] { "XXXX XXXX  XXXX XXXX  XXXX XXXX  XXXX XXXX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
short w_locs[]     { 0,2,5,7, 11,13,16,18, 22,24,27,29, 33,35,38,40, 46, 56 };

char  l_template[] { "XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
short l_locs[]     { 0,2,4,6, 9,11,13,15, 18,20,22,24, 27,29,31,33, 39, 49};

char  q_template[] { "XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
short q_locs[]     { 0,2,4,6,8,10,12,14, 17,19,21,23,25,27,29,31, 37, 47 };

char  o_template[] { "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  # AAAAAAAA  CCCCCCCCCCCCCCCC\n" };
short o_locs[]     { 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30, 36, 46 };

char hexdig[]      { "0123456789abcdef" };


   /***  Global Variable Definitions  ***/

long      dataend   { -1L };         // Input Stream End
short     fcount;                    // Number of Files to Dump
GroupType grouping  { Group_Long };  // Grouping (Byte, Word or Long)
short*    locs      { l_locs };      // Byte Output Locations
long      datastart { -1L };         // Input Stream Start
char*     ptemplate { l_template };  // Line Template
bool      compact   { false };       // Compact Duplicate Lines



/***************************************************************************************************
Helper Functions
***************************************************************************************************/

inline int print (char *string)
{
    return fputs (string, stdout);
}


inline int fprint (FILE *stream, char *string)
{
    return fputs (string, stream);
}


//**************************************************************************************************

int main (int argc, char *argv[])
{
    long argi;  // Command-Line Argument Index

    // Process the command-line arguments.

    if (!ProcessArgs (argc, argv))
    {   fprint (stderr, usage);
        exit (0);
    }

    // Set up the output buffer according to the grouping type.

    switch (grouping)
    {
        case Group_Byte:  ptemplate = b_template;  locs = b_locs;  break;
        case Group_Word:  ptemplate = w_template;  locs = w_locs;  break;

        default:
        case Group_Long:  ptemplate = l_template;  locs = l_locs;  break;

        case Group_Quad:  ptemplate = q_template;  locs = q_locs;  break;
        case Group_Oct:   ptemplate = o_template;  locs = o_locs;  break;
    }

    // If no filenames were given, dump the standard input stream, otherwise
    // dump each of the named files.

    if (fcount == 0)
        Dump (stdin, datastart, dataend);
    else
    {
        for (argi=1;  argi < argc;  ++argi)
        {
            auto fname = argv[argi];  // File Name

            // Skip over command-line switches.

            if (!*fname) continue;

            // Dump the file.

            FILE *file;     // File Handle

            if (0 != fopen_s(&file, fname,"rb"))
                fprintf (stderr, "hex:  Couldn't open \"%s\".\n", fname);
            else
            {   if (fcount > 1) printf ("\n%s:\n", fname);
                Dump (file, datastart, dataend);
                fclose (file);
            }
        }
    }

    return 0;
}


//**************************************************************************************************

short ProcessArgs (int argc, char *argv[])
{
    // This routine processes the command-line arguments. If all goes well, the function returns 1,
    // else it returns 0.

    int argi;         // Command-Line Argument Index

    for (fcount=0, argi=1;  argi < argc;  ++argi)
    {
        char *swptr;    // Switch Pointer

        // First check to see if the user is prompting for information. Note that I also check
        // forward-slash options for PC folks.

        if (  (0 == strcmp(argv[argi], "-?"))
           || (0 == strcmp(argv[argi], "/?"))
           || (0 == strcmp(argv[argi], "-h"))
           || (0 == strcmp(argv[argi], "/h"))
           || (0 == strcmp(argv[argi], "-help"))
           || (0 == strcmp(argv[argi], "/help"))
           )
        {
            return 0;
        }

        // If the option does not start with a dash, we assume it's a filename, so add that to the
        // list of files.

        if (argv[argi][0] != '-')
        {   ++fcount;
            continue;
        }

        swptr = argv[argi] + 1;

        do
        {   switch (*swptr)
            {
                case 'b':   grouping = Group_Byte;  break;
                case 'l':   grouping = Group_Long;  break;
                case 'w':   grouping = Group_Word;  break;
                case 'q':   grouping = Group_Quad;  break;
                case 'o':   grouping = Group_Oct;   break;

                case 'c':   compact  = true;        break;

                case 'e':
                {   char *ptr = swptr+1;

                    if (!*ptr)
                    {   argv[argi][0] = 0;
                        ptr = argv[++argi];
                    }

                    if (argc <= argi)
                    {   fprint (stderr,
                                "hex:  No argument given to -e option.\n");
                        return 0;
                    }

                    dataend = ReadPositiveLong (ptr);

                    swptr = 0;
                    break;
                }

                case 's':
                {   auto ptr = swptr+1;

                    if (!*ptr)
                    {   argv[argi][0] = 0;
                        ptr = argv[++argi];
                    }

                    if (argc <= argi)
                    {   fprint (stderr,
                                "hex:  No argument given to -s option.\n");
                        return 0;
                    }

                    datastart = ReadPositiveLong (ptr);

                    swptr = 0;
                    break;
                }

                default:
                    fprintf (stderr, "hex:  Unknown option (%c).\n", *swptr);
                    return 0;
            }

            if (swptr)
                ++swptr;

        } while (swptr && *swptr);

        argv[argi][0] = 0;
    }

    return 1;
}


//**************************************************************************************************

void Dump (FILE *file, long datastart, long dataend)
{
    // This procedure dumps a file to standard output.

    if ((dataend > 0) && (datastart > 0) && (dataend <= datastart))
        return;

    size_t addr = (datastart > 0) ? datastart : 0;
    bool   redblock { false };

    // If the user specified a start address, then seek to that location.

    if (datastart < 0)
        datastart = 0;
    else
    {   if (0 != fseek (file, datastart, 0))
        {   fprint (stderr, "hex:  fseek to start position failed.\n");
            return;
        }
    }

    // While a non-zero number of bytes are read in...

    char   buff[16];        // Input Buffer
    char   priorBuff[16];   // Prior Input Buffer
    size_t nbytes;          // Number of Bytes Read In

    while ((0 != (nbytes = fread (buff, 1, 0x10, file))) || redblock)
    {
        if (dataend > 0)
        {   if ((dataend <= addr) && !redblock) break;
            if (dataend < (addr+0x10))
                nbytes = dataend - addr + 1;
        }

        // If we're in compact print mode, and we're not at the first line, and we have a full line
        // of data, and this data line is that same as the prior one, then represent subsequent
        // duplicate lines with a single line of "====".

        if (  compact && (addr != datastart) && (nbytes == 0x10)
           && (0 == memcmp (priorBuff, buff, sizeof(buff)))
           )
        {
            // Print the redundant line marker, but only once per block.

            if (!redblock)
            {   fputs ("====\n", stdout);
                redblock = true;
            }
            addr += nbytes;
            continue;
        }

        // The following conditional is true when the input ends in the middle of a redundant block.
        // In this situation, we force the output of the last line of the input. Note that for
        // redblock to be true, the previous number of bytes read in had to be 0x10, so we know
        // that the number of bytes in the last buffer is 0x10. We also decrement the address to
        // adjust for having skipped past the last block.

        if (!nbytes && redblock)
        {   nbytes  = 0x10;
            addr   -= 0x10;
        }

        // If we get this far, then we're not in a redundant block.

        redblock = false;

        // Write the current address to the output buffer.

        auto ptr = ptemplate + locs[16] + 7;
        auto jj = addr;

        for (int i=8;  i != 0;  --i, jj>>=4, --ptr)
            *ptr = hexdig[jj & 0xf];

        // Write the hexadecimal value of each byte.

        auto t = 0;    // Template Character Index

        for (t=0;  t < nbytes;  ++t)
        {   ptemplate [locs[t]  ] = hexdig [ (unsigned char)(buff[t]) >> 4  ];
            ptemplate [locs[t]+1] = hexdig [ (unsigned char)(buff[t]) & 0xF ];
        }

        // If we didn't read a full line, then pad to the ASCII section with blank spaces (we need
        // to overwrite the previous charaters).

        for (; t < 0x10;  ++t)
            ptemplate[locs[t]] = ptemplate[locs[t]+1] = ' ';

        // Write out the ASCII values of the input buffer.

        for (t=0;  t < nbytes;  ++t)
            ptemplate [locs[17]+t]
                = ((buff[t] < 0x20) || (0x7E < buff[t]) ? '.' : buff[t]);

        // If we didn't read a full line, then pad the remainder of the ASCII section with
        // blank spaces.

        for (; t < 0x10;  ++t)
            ptemplate [locs[17]+t] = ' ';

        fputs (ptemplate, stdout);
        memcpy (priorBuff, buff, sizeof(buff));
        addr += nbytes;
    }
}


//**************************************************************************************************

long ReadPositiveLong (char *string)
{
    // This function reads in a decimal, hexadecimal, or octal number from the given string.

    auto base  = 10L;    // Base of the Input Number (8, 10, or 16).
    auto value = 0L;     // Value of the Number
    int  digit;          // Current Digit

    // Set the base up differently if we're getting an octal or a hexadecimal number.

    if (*string == '0')
    {   if (tolower(string[1]) == 'x')
        {   string += 2;
            base = 16;
        }
        else
        {   string ++;
            base = 8;
        }
    }

    for (;;)
    {
        digit = *string++;

        switch (digit)
        {   case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                digit -= '0';
                break;

            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                digit = 10 + tolower(digit) - 'a';
                break;

            default:
                return value;
        }

        if (digit >= base)
            return value;

        value = (value * base) + digit;
    }
}
