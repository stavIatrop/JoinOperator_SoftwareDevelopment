#include <stdio.h>
#include <stdlib.h>
#include <queryStructs.h>
#include <basicStructs.h>

void workerJ(join *pred, headInter * hq)
{
	bool self = 0;
	colRel *r1 = &(pred->participant1), *r2 = &(pred->participant2);
	myint_t rel1 = r1->rel, rel2=r2->rel;
	if (rel1 == rel2) self = 1;
	char used = inInterList(hq,rel1,rel2);
	if (self==1)
	{
		myint_t *res, survivors;
		if (used==0)
		{
			res = performSelfJoin(NULL,pred,&survivors);
			createInterSelfJoin(hq,rel1,res,survivors);
		}
		else
		{
			nodeInter *temp = findNode(hq,rel1);
			res = performSelfJoin(temp,pred,&survivors);
			updateInterSelfJoin(temp,res,survivors);
		}
	}
	else
	{
		headResult * res;
		myint_t newRel;
		res = performRHJ(hq,r1,r2,&newRel);
		if (used==0) createInterFromRes(hq,res,rel1,rel2);
		else if (used==1) updateInterFromRes(hq,res,newRel);
		else updateInterAndDelete(hq,findNode(hq,rel1),findNode(hq,rel2),res);
	}
	return;
}

headResult *performRHJ(headInter *hi, colRel *r1, colRel *r2, myint_t *newRel)
{
	nodeInter *n1 = findNode(hi,r1->rel), *n2 = findNode(hi,r2->rel);
	char used = inInterList(hq,r1->rel,r2->rel);
	if (used==2)
	{
	}
	else if (used==1)
	{
	}
	else
	{
		myint_t i, rows1 = r1->rows, rows2 = r2->rows;
		myint_t *col1 = r1->col, *col2 = r2->col;
		relation *rel1 = malloc(sizeof(relation));
		relation *rel2 = malloc(sizeof(relation));
		tuple *t1 = rel1->tuples;
		tuple *t2 = rel2->tuples;
		t1 = malloc(rows1*sizeof(tuple));
		t2 = malloc(rows2*sizeof(tuple));
		rel1->size = rows1;
		rel2->size = rows2;
		for (i=0 ; i<rows1 ; i++)
		{
			t1[i].key = col1[i];
			t1[i].payload = i;
		}
		for (i=0 ; i<rows2 ; i++)
                {
                        t2[i].key = col2[i];
                        t2[i].payload = i;
                }
		return radixHashJoin(rel1,rel2);
	}
}

myint_t *performSelfJoin(nodeInter *valids, join *pred, myint_t *survivors)
{
	colRel *r1 = &(pred->participant1);
	myint_t *col1 = r1->col;
	myint_t *col2 = pred->participant2.col;
	if (valids)
	{
		inter * data = valids->data;
		myint_t rows = r1->rows, cur=0, i, j, limit = valids->data->numbOfRows;
		myint_t *temp = malloc(rows*sizeof(myint_t));
		for (i=0;i< numOfCols; i++) if (data->joinedRels[i]==r1->rel) break;
		myint_t trueValids = data->rowIds[i];
		i=0;
		while(i < limit)
		{
			j = trueValids[i];
			if (col1[j]==col2[j]) temp[cur++]=j;
			while (trueValids[i]==j)
			{
				i++;
				if (i>=limit) break;
			}
		}
		*survivor = cur;
		temp = realloc((void *) temp, cur * sizeof(myint_t));
		return temp;
	}
	else
	{
		myint_t rows = r1->rows, cur = 0;
                myint_t *temp = malloc(rows*sizeof(myint_t));
                for (myint_t i=0; i<rows; i++)
                {
                        if (col1[i]==col2[i]) temp[cur++]=i;
                }
		*survivor = cur;
                temp = realloc((void *) temp, cur * sizeof(myint_t));
                return temp;
	}
}

char inInterList(headinter *hi, myint_t rel1, myint_t rel2)
{
	char flag1 = 0, flag2 = 0;
	nodeInter *node = hi->start;
	inter *interp;
	myint_t i;
	while (node!=NULL)
	{
		interp = node->data;
		for (i=0; i < inter->numOfCols; i++)
		{
			if (node->joinedRels[i] == rel1) flag1=1;
			if (node->joinedRels[i] == rel2) flag2=1;
			if (flag1 && flag2) break;
		}
		if (flag1 && flag2) return 2;
		node = node->next;
	}
	return flag1 + flag2;
}

nodeInter *findNode(headinter *hi, myint_t rel)
{
        nodeInter *node = hi->start;
        inter *interp;
        myint_t i;
        while (node!=NULL)
        {
                interp = node->data;
                for (i=0; i < inter->numOfCols; i++)
                {
                        if (node->joinedRels[i] == rel) return node;
                }
                node = node->next;
        }
        return NULL;
}

