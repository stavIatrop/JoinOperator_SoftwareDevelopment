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

int main(int argc, char const *argv[])
{
        char file[500] = "testTables/file3_50000";

        table * t = readTable(file);

        printTable(t);

        relation * r =extractRelation(t, 0);

        printRelation(r);

        return 0;
}
