#include <stdio.h>
#include <stdlib.h>
#include "jointPerformer.h"
#include "interListInterface.h"
#include "inputInterface.h"
#include "resultListInterface.h"

void workerF(filter *pred, headInter *hq)
{
	//fprintf(stderr, "%ld\n", pred->participant.rows);
	colRel *r = &(pred->participant);
        myint_t *col = r->col;
	char op = pred->op;
        myint_t rows = r->rows, cur = 0, i, value = pred->value;
        myint_t *temp = malloc(rows*sizeof(myint_t));
	nodeInter *node = findNode(hq,r->rel);

	if (node)
        {
                inter * data = node->data;
                myint_t limit=data->numbOfRows;
                for (i=0; i< data->numOfCols; i++) if (data->joinedRels[i]==r->rel) break;
                myint_t *rowIds = data->rowIds[i];
                i=0;
                for (i=0; i < limit; i++)
                {
			if (ApplyFilter(col[rowIds[i]],op,value)>0)
        	        {
        	                temp[cur++]=i;
  	        	}
                }
		updateInterSelfJoin(node,temp,cur);
		free(temp);
        }
        else
        {
                for (i=0; i<rows; i++)
                {
                        if (ApplyFilter(col[i],op,value)>0) {
							temp[cur++]=i;
						}
                }
		temp = realloc((void *) temp, cur * sizeof(myint_t));
		createInterSelfJoin(hq,r->rel,temp,cur);
		free(temp);
        }
}

char ApplyFilter(myint_t value, char op, myint_t target)
{
	if (op==LESS) return value < target ? 1 : 0;
	if (op==GREATER) return value > target ? 1 : 0;
	return value == target ? 1 : 0;
}

void workerJ(join *pred, headInter * hq)
{
	char self = 0;
	colRel *r1 = &(pred->participant1), *r2 = &(pred->participant2);
	myint_t rel1 = r1->rel, rel2=r2->rel;
	if (rel1 == rel2) self = 1;
	nodeInter *n1 = findNode(hq,rel1);
    	nodeInter *n2 = findNode(hq,rel2);
	char used;
	if (n1 && n2) used=2;
	else if (n1==NULL && n2==NULL) used=0;
	else used = 1;
	if (self==1)
	{
		myint_t *res, survivors;
		if (used==0)
		{
			res = performSelfJoin(NULL,pred,&survivors);
			createInterSelfJoin(hq,rel1,res,survivors);
			free(res);
		}
		else
		{
			res = performSelfJoin(n1,pred,&survivors);
			updateInterSelfJoin(n2,res,survivors);
			free(res);
		}
	}
	else
	{
		if (used==2 && n1==n2)
		{
			myint_t *res, survivors;
			res = performSameNodeJoin(n1,pred,&survivors);
			updateInterSelfJoin(n1,res,survivors);
			free(res);
		}
		else
		{
			headResult *res;
			myint_t newRel, skipped=0;
			char switched;

			if (used==0)
			{
				switched = 0;
				res = performRHJ(hq,r1,r2,&newRel, &switched, &skipped);
				createInterFromRes(hq,res,rel1,rel2, switched);
			}
			else if (used==1)
			{
				res = performRHJ(hq,r1,r2,&newRel, &switched, &skipped);
				if (rel1==newRel) {
					if (skipped==0) updateInterFromRes(n2,res,newRel,switched);
					else updateInterFromRes2(n2,res,newRel,rel2,skipped,switched);
				}
				else{
					if (skipped==0) updateInterFromRes(n1,res,newRel,switched);
					else updateInterFromRes2(n1,res,newRel,rel1,skipped,switched);
				}
			}
			else
			{
				switched = 2;
				res = performRHJ(hq,r1,r2,&newRel, &switched, &skipped);
				if (skipped==0) updateInterAndDelete(hq,n1,n2,res,switched);
				else updateInterAndDelete2(hq,n1,n2,res,rel1,rel2,skipped,switched);
			}
			freeResultList(res);
		}
	}
	return;
}

myint_t findNextRowId(myint_t *rowIds, myint_t i, myint_t rows)
{
	myint_t current = rowIds[i];
	i++;
	while (i<rows)
	{
		if (rowIds[i]!=current) break;
		i++;
	}
	return i;
}

relation *forgeRelationsheep(headInter *hi, colRel *r, myint_t *skipped)
{
	nodeInter *node = findNode(hi,r->rel);
	relation *rel = malloc(sizeof(relation));
	rel->realRel = r->realRel;
	rel->realCol = r->realCol;
        tuple *t;
	myint_t *col = r->col, i;
	if (node==NULL)
        {
		myint_t rows = r->rows;
        	t = malloc(rows*sizeof(tuple));
        	rel->size = rows;
        	for (i=0 ; i<rows ; i++)
        	{
        		t[i].key = col[i];
        	        t[i].payload = i;
        	}
		rel->tuples = t;
		return rel;
	}

	inter *data = node->data;
	myint_t rows = data->numbOfRows, cols = data->numOfCols, cur=0;
	t = malloc(rows*sizeof(tuple));
	for (i=0;i< cols; i++) if (data->joinedRels[i]==r->rel) break;
	myint_t *rowIds = data->rowIds[i]; //, value;
	//if (rowIds[0]==0) value = rowIds[0] +1;
	//else value = rowIds[0] -1;
	for (i=0 ; i<rows ; i++)
	{
		/*if (rowIds[i]==value)  uncomment all these to activate packages
		{
			(*skipped)++;
			continue;
		}
		value = rowIds[i];*/
		t[cur].key = col[rowIds[i]];
	    	t[cur].payload = i;
		cur++;
	}
	rel->size = cur;
	rel->tuples = realloc((void *) t, cur * sizeof(tuple));
	return rel;
}

headResult *performRHJ(headInter *hi, colRel *r1, colRel *r2, myint_t *newRel, char *switched, myint_t *skipped)
{
	relation *relation1 = forgeRelationsheep(hi,r1,skipped);

	relation *relation2 = forgeRelationsheep(hi,r2,skipped);

	//fprintf(stderr,"skipped is %lu\n", *skipped);

	char new1=0, new2=0;
	if (findNode(hi,r1->rel)==NULL) new1=1;
	if (findNode(hi,r2->rel)==NULL) new2=1;
	if (new1==1 && new2==0)
	{
		*newRel = r1->rel;
		*switched = 1;
		return radixHashJoin(relation2,relation1, switched);
	}
	if (new1==0 && new2==1)
	{
		*newRel = r2->rel;
		*switched = 1;
		return radixHashJoin(relation1,relation2, switched);
	}
	else return radixHashJoin(relation1,relation2, switched);
}

myint_t *performSameNodeJoin(nodeInter *node, join *pred, myint_t *survivors)
{
	colRel *r1 = &(pred->participant1);
	colRel *r2 = &(pred->participant2);
        myint_t *col1 = r1->col;
        myint_t *col2 = r2->col;
	inter * data = node->data;
        myint_t rows = data->numbOfRows, cur=0, i, cols = data->numOfCols;
        myint_t *temp = malloc(rows*sizeof(myint_t));
        for (i=0; i< cols; i++) if (data->joinedRels[i]==r1->rel) break;
	myint_t *rowIds1 = data->rowIds[i];
	for (i=0; i< cols; i++) if (data->joinedRels[i]==r2->rel) break;
        myint_t *rowIds2 = data->rowIds[i];
        for (i=0; i<rows; i++)
        {
			if (col1[rowIds1[i]]==col2[rowIds2[i]])
            {
                    temp[cur++]=i;
            }
        }
	*survivors = cur;
        temp = realloc((void *) temp, cur * sizeof(myint_t));
        return temp;
}

myint_t *performSelfJoin(nodeInter *valids, join *pred, myint_t *survivors)
{
	colRel *r1 = &(pred->participant1);
	myint_t *col1 = r1->col;
	myint_t *col2 = pred->participant2.col;
	myint_t rows = r1->rows, cur = 0;
        myint_t *temp = malloc(rows*sizeof(myint_t));

	if (valids)
	{
		inter * data = valids->data;
		myint_t i, j, limit=data->numbOfRows, cols = data->numOfCols;
		for (i=0;i< cols; i++) if (data->joinedRels[i]==r1->rel) break;
		myint_t *trueValids = data->rowIds[i];
		i=0;
		for (i=0; i < limit; i++)
		{
			j = trueValids[i];
			if (col1[j]==col2[j]) temp[cur++]=i;
		}
	}
	else
	{
                for (myint_t i=0; i<rows; i++)
                {
                        if (col1[i]==col2[i]) temp[cur++]=i;
                }
	}
	*survivors = cur;
        temp = realloc((void *) temp, cur * sizeof(myint_t));
        return temp;
}

nodeInter *findNode(headInter *hi, myint_t rel)
{
        nodeInter *node = hi->start;
        inter *interp;
        myint_t i;
        while (node!=NULL)
        {
                interp = node->data;
                for (i=0; i < interp->numOfCols; i++)
                {
                        if (interp->joinedRels[i] == rel) return node;
                }
                node = node->next;
        }
        return NULL;
}

