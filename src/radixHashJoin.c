#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basicStructs.h"
#include "inputInterface.h"
#include "indexManip.h"
#include "viceFunctions.h"
#include "queryStructs.h"

headResult * radixHashJoin(relation * rRel, relation * sRel, char * switched) {

        myint_t h1 = FIRST_REORDERED;
        reorderedR * RoR = NULL;
        reorderedR * RoS;
        char freedomFlag;
        indexArray * indArr = NULL;

        //fprintf(stderr,"rel1 is %lu, rel2 is %lu, switched is %d\n",rRel->realRel,sRel->realRel,*switched);
        if (*switched==0)
        {
                freedomFlag=0;
                indArr = FetchFromWarehouse(rRel->realRel, rRel->realCol, &h1);
                if (indArr==NULL)
                {
                        indArr = FetchFromWarehouse(sRel->realRel, sRel->realCol, &h1);
                        if (indArr) *switched=1;
                }
                if (indArr)
                {
                        if (*switched==0) RoS = reorderRelation(sRel, &h1);
                        else RoS = reorderRelation(rRel, &h1);
                }       
                else
                {
                                
                        //Reordering (Choosing best relationship to create the index)
                        if ((rRel->size) < (sRel->size))
                        {
                                RoR = reorderRelation(rRel, &h1);
                                RoS = reorderRelation(sRel, &h1);
                                *switched = 0;
                        }
                        else
                        {
                                RoR = reorderRelation(sRel, &h1);
                                RoS = reorderRelation(rRel, &h1);
                                *switched = 1;
                        }
                } 
        }
        else if (*switched==1)
        {
                freedomFlag=1;
                indArr = FetchFromWarehouse(sRel->realRel, sRel->realCol, &h1);
                if (indArr==NULL) RoR = reorderRelation(sRel, &h1);
                RoS = reorderRelation(rRel, &h1);
        }
        else
        {
                freedomFlag=2;
                double rIdenticality = IdenticalityTest(rRel);
                double sIdenticality = IdenticalityTest(sRel);
                        
                if ((rRel->size)*rIdenticality <= (sRel->size)*sIdenticality)
                {
                        RoR = reorderRelation(rRel, &h1);
                        RoS = reorderRelation(sRel, &h1);
                        *switched = 0;
                }
                else
                {
                        RoR = reorderRelation(sRel, &h1);
                        RoS = reorderRelation(rRel, &h1);
                        *switched = 1;
                }        
        }

        headResult * results;

        //Indexing
        if (indArr==NULL)
        {
                indArr = indexing(RoR, h1);
                if (freedomFlag==1) AddToWarehouse(sRel->realRel, sRel->realCol, h1, indArr);
                else if (freedomFlag==0)
                {
                        if (*switched==0) AddToWarehouse(rRel->realRel, rRel->realCol, h1, indArr);
                        else AddToWarehouse(sRel->realRel, sRel->realCol, h1, indArr);
                }
        }
        results = searchThreadVer(indArr, RoS);

        free(rRel->tuples);
        free(sRel->tuples);
        free(rRel);
        free(sRel);

        //Searching
        
        free(RoS->rel->tuples);
        free(RoS->rel);
        free(RoS->pSumArr.psum);
        free(RoS);
        
        if (freedomFlag==2)
        {
                free(RoR->rel->tuples);
                free(RoR->rel);
                free(RoR->pSumArr.psum);
                free(RoR);
                freeIndexArray(indArr);
        }
        
        return results;
}
