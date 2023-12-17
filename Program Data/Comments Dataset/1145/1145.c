/*
This function is responsible for parsing command-line options in a C program.

Here's the detail:
- The function takes the standard arguments argc, argv[], and opts, where argc is the argument count, argv[] is an array of pointers to the arguments, and opts is a string containing the valid option characters.
- It uses static variable sp to keep track of the current position within the argument string.
- It processes the command-line arguments one by one, identifying options preceded by '-' and handling both short and long options.
- It checks for illegal options, missing arguments for options that require them, and updates optind to point to the next argument to be processed.
- It also sets the global variables optopt and optarg based on the processed options, which can be used by the calling program to determine the specific options and their arguments.

Need's to notice:
- The function assumes that the command-line arguments conform to the standard Unix-style option syntax (e.g., '-a', '-b value', '--option', etc.).
- The function provides basic error handling for illegal options and missing arguments, but additional validation and error reporting may be needed depending on the specific requirements of the program using this function.
- This function is a basic implementation of option parsing and may not handle more advanced scenarios such as long options with values, option grouping, or subcommands. Additional features may need to be added depending on the complexity of the command-line interface.
*/
static int getopt(int argc, char *argv[], char *opts)

{

    static int sp = 1;

    int c;

    char *cp;



    if (sp == 1)

        if (optind >= argc ||

            argv[optind][0] != '-' || argv[optind][1] == '\0')

            return EOF;

        else if (!strcmp(argv[optind], "--")) {

            optind++;

            return EOF;

        }

    optopt = c = argv[optind][sp];

    if (c == ':' || (cp = strchr(opts, c)) == NULL) {

        fprintf(stderr, ": illegal option -- %c\n", c);

        if (argv[optind][++sp] == '\0') {

            optind++;

            sp = 1;

        }

        return '?';

    }

    if (*++cp == ':') {

        if (argv[optind][sp+1] != '\0')

            optarg = &argv[optind++][sp+1];

        else if(++optind >= argc) {

            fprintf(stderr, ": option requires an argument -- %c\n", c);

            sp = 1;

            return '?';

        } else

            optarg = argv[optind++];

        sp = 1;

    } else {

        if (argv[optind][++sp] == '\0') {

            sp = 1;

            optind++;

        }

        optarg = NULL;

    }



    return c;

}
