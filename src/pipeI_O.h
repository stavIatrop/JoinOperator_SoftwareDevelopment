#ifndef PIPEI_O_H
#define PIPEI_O_H

#include "queryStructs.h"
#include "checksumInterface.h"

#define MAX_BUFFER 4096

char * readFromPipe(const char * );
int numDigits(myint_t );
void writePipe(checksum * );
#endif