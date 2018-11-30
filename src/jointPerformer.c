#include <stdio.h>
#include <stdlib.h>
#include <jointPerformer.h>

void workerJ(join *pred, headInter * hq)
{
	bool self = 0;
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
		}
		else
		{
			res = performSelfJoin(n1,pred,&survivors);
			updateInterSelfJoin(n2,res,survivors);
		}
	}
	else
	{
		if (used==2 && n1==n2)
		{
			myint_t *res, survivors;
			res = performSameNodeJoin(n1,pred,&survivors);
			updateInterSelfJoin(n1,res,survivors);
		}
		else
		{
			headResult *res;
			myint_t newRel;
			res = performRHJ(hq,r1,r2,&newRel);
			if (used==0) createInterFromRes(hq,res,rel1,rel2);
			else if (used==1) updateInterFromRes(hq,res,newRel);
			else updateInterAndDelete(hq,n1,n2,res);
		}
	}
	return;
}

myint_t findNextRowId(myint_t *rowIds, myint_t i, myint_t rows)
{
	myint_t current = rowIds[i];
	while (i<rows)
	{
		i++;
		if (rowIds[i]!=current) break;
	}
	return i;
}

relation *forgeRelationsheep(headInter *hi, colRel *r)
{
	nodeInter *node = findNode(hi,r->rel);
	relation *rel = malloc(sizeof(relation));
        tuple *t = rel->tuples;
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
		return rel;
	}

	inter *data = node->data;
	myint_t j, cur=0, rows = data->numberOfRows;
	t = malloc(rows*sizeof(tuple));
	for (i=0;i< numOfCols; i++) if (data->joinedRels[i]==r1->rel) break;
	myint_t rowIds = data->rowIds[i];
	i=0;
        while(i < limit)
        {
                j = trueValids[i];
                t[cur].key = col[j];
		t[cur].payload = j;
		cur++;
                i = findNextRowId(trueValids,i,limit);
        }
	rel->size = cur;
	t = realloc((void *) t, cur * sizeof(tuple));
	return rel;
}

headResult *performRHJ(headInter *hi, colRel *r1, colRel *r2, myint_t *newRel)
{
	relation *relation1 = forgeRelationsheep(hi,r1);
	relation *relation2 = forgeRelationsheep(hi,r2);
	char new1=0, new2=0;
	if (n1==NULL) new1=1;
	if (n2==NULL) new2=1;
	if (new1==1 && new2==0) *newRel = r1->rel;
	if (new1==0 && new2==1) *newRel = r2->rel;
	return radixHashJoin(relation1,relation2);
}

myint_t *performSameNodeJoin(nodeInter *node, join *pred, myint_t *survivors)
{
	colRel *r1 = &(pred->participant1);
	colRel *r2 = &(pred->participant2);
        myint_t *col1 = r1->col;
        myint_t *col2 = r2->col;
	inter * data = node->data;
        myint_t rows = data->numbOfRows, cur=0, i, j1, j2, next1, next2;
        myint_t *temp = malloc(rows*sizeof(myint_t));
        for (i=0; i<numOfCols; i++) if (data->joinedRels[i]==r1->rel) break;
	myint_t *rowIds1 = data->rowIds[i];
	for (i=0; i<numOfCols; i++) if (data->joinedRels[i]==r2->rel) break;
        myint_t *rowIds2 = data->rowIds[i];
        i=0;
        while(i < rows)
        {
                j1 = rowIds1[i];
		j2 = rowIds2[i];
		next1 = findNextRowId(rowIds1,i,rows);
		next2 = findNextRowId(rowIds2,i,rows);
		if (next1>next2) next1 = next2;

		if (col1[j1]==col2[j2]) for ( ; i<next1; i++)
		{
			temp[cur++]=i;
		}
        }
	*survivor = cur;
        temp = realloc((void *) temp, cur * sizeof(myint_t));
        return temp;
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
			i = findNextRowId(trueValids,i,limit);
		}
	}
	else
	{
		myint_t rows = r1->rows, cur = 0;
                myint_t *temp = malloc(rows*sizeof(myint_t));
                for (myint_t i=0; i<rows; i++)
                {
                        if (col1[i]==col2[i]) temp[cur++]=i;
                }
	}
	*survivor = cur;
        temp = realloc((void *) temp, cur * sizeof(myint_t));
        return temp;
}

/*char inInterList(headinter *hi, myint_t rel1, myint_t rel2)
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
}*/

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

