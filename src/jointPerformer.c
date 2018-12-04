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
                myint_t limit=data->numbOfRows, next;
                for (i=0; i< data->numOfCols; i++) if (data->joinedRels[i]==r->rel) break;
                myint_t *rowIds = data->rowIds[i];
                i=0;
                while(i < limit)
                {
			next = findNextRowId(rowIds,i,limit);
        	        if (ApplyFilter(col[rowIds[i]],op,value)>0) for ( ; i<next; i++)
        	        {
        	                temp[cur++]=i;
  	        	}
			else i = next;
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
		//fprintf(stderr, "Inter after filter =  %ld\n", cur);
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
			myint_t newRel;
			char switched;

			res = performRHJ(hq,r1,r2,&newRel, &switched);
			if (used==0) createInterFromRes(hq,res,rel1,rel2);
			else if (used==1)
			{
				if (rel1==newRel) {
					updateInterFromRes(n2,res,newRel,switched);
					//fprintf(stderr, "REL1 NEW\n");
				}
				else{
					fprintf(stderr, "REL2 NEW %ld | new rel: %ld | newRelRows: %ld\n", n1->data->numbOfRows, newRel, r2->rows);
					updateInterFromRes(n1,res,newRel,switched);
				}
			}
			else updateInterAndDelete(hq,n1,n2,res,switched);
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

relation *forgeRelationsheep(headInter *hi, colRel *r)
{
	nodeInter *node = findNode(hi,r->rel);
	relation *rel = malloc(sizeof(relation));
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
		//fprintf(stderr, "size2 is %ld\n", rows);
		return rel;
	}

	inter *data = node->data;
	myint_t j, cur=0, rows = data->numbOfRows, cols = data->numOfCols, next;
	t = malloc(rows*sizeof(tuple));
	for (i=0;i< cols; i++) if (data->joinedRels[i]==r->rel) break;
	myint_t *rowIds = data->rowIds[i];
	i=0;
        while(i < rows)
        {
                next = findNextRowId(rowIds,i,rows);
		for ( ; i < next; i++)
		{
			j = rowIds[i];
			//if(j >= r->rows) fprintf(stderr, "J = %ld, rel = %ld\n", j, r->rel); 
                	t[cur].key = col[j];
        	        t[cur++].payload = i;
		}
        }
	fprintf(stderr, "AAA %ld is already in an inter\n", r->rel);
	rel->size = cur;
	//fprintf(stderr, "size is %ld\n", cur);
	rel->tuples = realloc((void *) t, cur * sizeof(tuple));
	return rel;
}

headResult *performRHJ(headInter *hi, colRel *r1, colRel *r2, myint_t *newRel, char *switched)
{
	relation *relation1 = forgeRelationsheep(hi,r1);

	relation *relation2 = forgeRelationsheep(hi,r2);
	fprintf(stderr, "REL1 ROWS: %ld | REL2 ROWS: %ld\n", relation1->size, relation2->size);

	char new1=0, new2=0;
	if (findNode(hi,r1->rel)==NULL) new1=1;
	if (findNode(hi,r2->rel)==NULL) new2=1;
	if (new1==1 && new2==0)
	{
		*newRel = r1->rel;
		return radixHashJoin(relation2,relation1, switched);
	}
	if (new1==0 && new2==1)
	{
		*newRel = r2->rel;
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
        myint_t rows = data->numbOfRows, cur=0, i, next1, next2, cols = data->numOfCols;
        myint_t *temp = malloc(rows*sizeof(myint_t));
        for (i=0; i< cols; i++) if (data->joinedRels[i]==r1->rel) break;
	myint_t *rowIds1 = data->rowIds[i];
	for (i=0; i< cols; i++) if (data->joinedRels[i]==r2->rel) break;
        myint_t *rowIds2 = data->rowIds[i];
        i=0;
        while(i < rows)
        {
		next1 = findNextRowId(rowIds1,i,rows);
                next2 = findNextRowId(rowIds2,i,rows);
                if (next2<next1) next1 = next2;

		if (col1[rowIds1[i]]==col2[rowIds2[i]]) for (; i<next1; i++)
                {
                        temp[cur++]=i;
                }
		else i = next1;
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
		myint_t i, j, limit=data->numbOfRows, cols = data->numOfCols, next;
		for (i=0;i< cols; i++) if (data->joinedRels[i]==r1->rel) break;
		myint_t *trueValids = data->rowIds[i];
		i=0;
		while(i < limit)
		{
			next = findNextRowId(trueValids,i,limit);
			j = trueValids[i];
			if (col1[j]==col2[j]) for (; i<next; i++) temp[cur++]=i;
			else i = next;
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

