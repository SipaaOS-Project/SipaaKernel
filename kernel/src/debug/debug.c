#include "debug.h"

char *dbglogs = "";

char* get_debuglogs()
{
    return dbglogs;
}

void writedbg(char *ln)
{
    dbglogs = strcat(dbglogs, ln);
}