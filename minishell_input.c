#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minishell_input.h"

/**
 * This function prints a simple string that will be used as the shell prompt.
 */
void print_prompt()
{
   printf("$ ");
   fflush(stdout);
}

