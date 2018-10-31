#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basicStructs.h"
#include "viceFunctions.h"
#include "resultListInterface.h"
#include "chainFollowerInterface.h"
#include "sideFunctions.h"
#include "hashing.h"
#include "indexManip.h"
#include "inputInterface.h"

int main(int argc, char *argv[])
{
        //char file[500] = "testTables/r3_10000000";

        arguments * args = readArguments(argc, argv);
        if(args == NULL) {
                perror("Wrong arguments");
                return -1;
        }


        table * r = readTable(args->rPath, args->type);
        table * s = readTable(args->sPath, args->type);

        printTable(r);
        printTable(s);

        relation * rRel =extractRelation(r, args->colR);
        relation * sRel =extractRelation(s, args->colS);

        printRelation(rRel);
        printRelation(sRel);

        return 0;
}
