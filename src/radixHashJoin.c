#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basicStructs.h"
#include "inputInterface.h"
#include "indexManip.h"
#include "viceFunctions.h"
#include "queryStructs.h"

double totalRadixTime = 0;
int radixes = 0;

headResult * radixHashJoin(relation * rRel, relation * sRel, char * switched) {

        myint_t h1 = FIRST_REORDERED;
        reorderedR * RoR = NULL;
        reorderedR * RoS = NULL;
        char freedomFlag;
        indexArray * indArr = NULL;
        clock_t begin = clock();
        //fprintf(stderr,"NNN\n");

        //fprintf(stderr,"rel1 is %lu, rel2 is %lu, switched is %d\n",rRel->realRel,sRel->realRel,*switched);
        if (*switched==0)
        {
                freedomFlag=0;
                indArr = FetchFromIndexWarehouse(rRel->realRel, rRel->realCol, &h1);
                if (indArr==NULL)
                {
                        indArr = FetchFromIndexWarehouse(sRel->realRel, sRel->realCol, &h1);
                        if (indArr) *switched=1;
                }
                //fprintf(stderr,"indArr is %lu\n\n",indArr);
                if (indArr)
                {
                        if (*switched==0)
                        {
                                RoS = FetchFromReWarehouse(sRel->realRel, sRel->realCol, &h1);
                                if (RoS==NULL)
                                {
                                        RoS = reorderRelation(sRel, &h1);
                                        AddToReWarehouse(sRel->realRel, sRel->realCol, h1, RoS, 0);
                                }
                        }
                        else
                        {
                                RoS = FetchFromReWarehouse(rRel->realRel, rRel->realCol, &h1);
                                if (RoS==NULL)
                                {
                                        RoS = reorderRelation(rRel, &h1);
                                        AddToReWarehouse(rRel->realRel, rRel->realCol, h1, RoS, 0);
                                }
                        }
                }       
                else
                {
                        RoR = FetchFromReWarehouse(rRel->realRel, rRel->realCol, &h1);
                        if (RoR==NULL)
                        {
                                RoR = FetchFromReWarehouse(sRel->realRel, sRel->realCol, &h1);
                                if (RoR==NULL)
                                {
                                       if ((rRel->size) < (sRel->size))
                                        {
                                                RoR = reorderRelation(rRel, &h1);
                                                AddToReWarehouse(rRel->realRel, rRel->realCol, h1, RoR, 1);
                                                RoS = reorderRelation(sRel, &h1);
                                                AddToReWarehouse(sRel->realRel, sRel->realCol, h1, RoS, 0);
                                                *switched = 0;
                                        }
                                        else
                                        {
                                                RoR = reorderRelation(sRel, &h1);
                                                AddToReWarehouse(sRel->realRel, sRel->realCol, h1, RoR, 1);
                                                RoS = reorderRelation(rRel, &h1);
                                                AddToReWarehouse(rRel->realRel, rRel->realCol, h1, RoS, 0);
                                                *switched = 1;
                                        } 
                                }
                                else
                                {
                                        RoS = reorderRelation(rRel, &h1);
                                        AddToReWarehouse(rRel->realRel, rRel->realCol, h1, RoS, 0);
                                }
                        }
                        else
                        {
                                RoS = FetchFromReWarehouse(sRel->realRel, sRel->realCol, &h1);
                                if (RoS==NULL)
                                {
                                        RoS = reorderRelation(sRel, &h1);
                                        AddToReWarehouse(sRel->realRel, sRel->realCol, h1, RoS, 0);
                                }
                        }
                } 
        }
        else if (*switched==1)
        {
                freedomFlag=1;
                indArr = FetchFromIndexWarehouse(sRel->realRel, sRel->realCol, &h1);
                if (indArr==NULL)
                {
                        RoR = FetchFromReWarehouse(sRel->realRel, sRel->realCol, &h1);
                        if (RoR==NULL)
                        {
                                RoR = reorderRelation(sRel, &h1);
                                AddToReWarehouse(sRel->realRel, sRel->realCol, h1, RoR, 1);
                        }
                }
                RoS = reorderRelation(rRel, &h1);
        }
        else
        {
                freedomFlag=2;
                        
                if (rRel->size <= sRel->size)
                {
                        //fprintf(stderr,"CCC\n");
                        RoR = reorderRelation(rRel, &h1);
                        RoS = reorderRelation(sRel, &h1);
                        *switched = 0;
                }
                else
                {
                        //fprintf(stderr,"DDD\n");
                        RoR = reorderRelation(sRel, &h1);
                        RoS = reorderRelation(rRel, &h1);
                        *switched = 1;
                }        
        }

        headResult * results;

        //Indexing
        if (indArr==NULL)
        {
                //fprintf(stderr,"BBB\n");
                indArr = indexing(RoR, h1);
                if (freedomFlag==1) AddToIndexWarehouse(sRel->realRel, sRel->realCol, h1, indArr);
                else if (freedomFlag==0)
                {
                        if (*switched==0) AddToIndexWarehouse(rRel->realRel, rRel->realCol, h1, indArr);
                        else AddToIndexWarehouse(sRel->realRel, sRel->realCol, h1, indArr);
                }
        }
        //fprintf(stderr,"AAA\n");
        results = searchThreadVer(indArr, RoS);
        //fprintf(stderr,"EEE\n");

        free(rRel->tuples);
        free(sRel->tuples);
        free(rRel);
        free(sRel);

        //fprintf(stderr,"HHH\n");

        //Searching
        
        if (freedomFlag>0)
        {
                free(RoS->rel->tuples);
                free(RoS->rel);
                free(RoS->pSumArr.psum);
                free(RoS);
        }

        //fprintf(stderr,"GGG\n");
        
        if (freedomFlag==2)
        {
                free(RoR->rel->tuples);
                free(RoR->rel);
                free(RoR->pSumArr.psum);
                free(RoR);
                freeIndexArray(indArr);
        }
        
        //fprintf(stderr,"FFF\n");
        clock_t end = clock();
        totalRadixTime += (double)(end - begin) / CLOCKS_PER_SEC;
        radixes += 1;
        
        return results;
}
