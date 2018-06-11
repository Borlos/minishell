#ifndef __INTERNALS_H__
#define __INTERNALS_H__

#include "parser.h"

int is_internal_command(const command *);
void execute_internal_command(const command *);

#endif // __INTERNALS_H__
