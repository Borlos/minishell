#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>

// Each command will be parsed and stored in one of these structures:

typedef struct
{
    char * raw_command;    // Complete command as it was read
    char * chunks_buffer;  // Chunked command (same as raw_command but with \0s)

    // NOTE: Every one of the following pointers will point to a location
    // within chunks_buffer instead of pointing to individual blocks

    char * input;          // Standard input redirection
    char * output;         // Standard output redirection
    char * output_err;     // Standard error output redirection

    int argc;              // Number (count) of arguments
    char ** argv;          // Vector of arguments

    char background;       // 1: background execution ('&')
}                          // 0: regular execution
command;

// Function that reads a full line from a stream. If no stream is specified
// (stream == NULL), then the stardard input (stdin) stream will be used

int read_line (char ** pp_line, FILE * stream);

// Function that parses a line read with read_line() into an array of command
// structures.  The function will generate one element for each of the commands
// in the line separated by the pipe character (|). The function shall return
// the number of commands read or a negative integer in case an error has
// occurred

int parse_commands (command ** pp_cmds, char *raw_command);

// Function that releases the resources used by the parsed commands

void free_commands (command* p_cmds, int n_cmds);


#endif // __PARSER_H__
