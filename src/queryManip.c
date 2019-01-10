#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queryStructs.h"

myint_t SubStrOccur(char * str, char * subStr) {

	myint_t count = 0;
	char *tmp = str;
	while((tmp = strstr(tmp, subStr))) {
   	
   		count++;
   		tmp++;
	}
	
	return count;
}

void fillJoinInfo(query * newQuery, char * tempStr, myint_t counterJoins, relationsheepArray relArray) {

	myint_t i = 0, start = 0, end = 0, dots = 0;

	while(tempStr[i] != '\0') {

		if(tempStr[i] == '.') {
			dots++;

			start = end;
			end = i + 1;
			char * part = (char *) malloc((i - start + 1) * sizeof(char));
			memset(part, '\0', i - start + 1 );
			strncpy(part, tempStr + start, i - start);
            strcat(part, "\0");
			
			if( dots == 1) {	//1st participant of join operation

				newQuery->joins[counterJoins].participant1.rel = atoi(part);
				newQuery->joins[counterJoins].participant1.realRel = newQuery->rels[atoi(part)];
				newQuery->joins[counterJoins].participant1.rows = relArray.rels[ newQuery->rels[atoi(part)] ].rows ;

			}else {

				newQuery->joins[counterJoins].participant2.rel = atoi(part);
				newQuery->joins[counterJoins].participant2.realRel = newQuery->rels[atoi(part)];
				newQuery->joins[counterJoins].participant2.rows = relArray.rels[ newQuery->rels[atoi(part)] ].rows ;
			}
			free(part);
			

		} else if( tempStr[i] == '=') {	//column of first participant

			start = end;
			end = i + 1;
			char * part = (char *) malloc((i - start + 1) * sizeof(char));
			memset(part, '\0', i - start + 1 );
			strncpy(part, tempStr + start, i - start);
            strcat(part, "\0");
            myint_t indexRel = newQuery->joins[counterJoins].participant1.rel;
            newQuery->joins[counterJoins].participant1.realCol = atoi(part);
            newQuery->joins[counterJoins].participant1.col = relArray.rels[ newQuery->rels[indexRel] ].pointToCols[atoi(part)];
            free(part);

		}
		i++;
	}

	//column of second participant
	start = end;
	end = i + 1;
	char * part = (char *) malloc((i - start + 1) * sizeof(char));
	memset(part, '\0', i - start + 1 );
	strncpy(part, tempStr + start, i - start);
    strcat(part, "\0");

    myint_t indexRel = newQuery->joins[counterJoins].participant2.rel;
    newQuery->joins[counterJoins].participant2.realCol = atoi(part);
    newQuery->joins[counterJoins].participant2.col = relArray.rels[ newQuery->rels[indexRel] ].pointToCols[atoi(part)];
    free(part);

}

void fillFilterInfo(query * newQuery, char * tempStr, myint_t counterFilters, relationsheepArray relArray) {

	myint_t i = 0, start = 0, end = 0;

	while(tempStr[i] != '\0') {

		if(tempStr[i] == '.') {
	
			start = end;
			end = i + 1;
			char * part = (char *) malloc((i - start + 1) * sizeof(char));
			memset(part, '\0', i - start + 1 );
			strncpy(part, tempStr + start, i - start);
            strcat(part, "\0");
			
			//participant of filter operation

			newQuery->filters[counterFilters].participant.rel = atoi(part);
			newQuery->filters[counterFilters].participant.rows = relArray.rels[ newQuery->rels[atoi(part)] ].rows ;


			free(part);
			

		} else if( tempStr[i] == '=' || tempStr[i] == '>' || tempStr[i] == '<') {	//column of filter participant


			start = end;
			end = i + 1;
			char * part = (char *) malloc((i - start + 1) * sizeof(char));
			memset(part, '\0', i - start + 1 );
			strncpy(part, tempStr + start, i - start);
            strcat(part, "\0");
            myint_t indexRel = newQuery->filters[counterFilters].participant.rel;
            newQuery->filters[counterFilters].participant.col = relArray.rels[ newQuery->rels[indexRel] ].pointToCols[atoi(part)];
            free(part);

            if( tempStr[i] == '=' )
            	newQuery->filters[counterFilters].op = EQUAL;
            else if( tempStr[i] == '>' )
            	newQuery->filters[counterFilters].op = GREATER;
            else
            	newQuery->filters[counterFilters].op = LESS;
           
		}
		i++;
	}

	//value
	start = end;
	end = i + 1;
	char * part = (char *) malloc((i - start + 1) * sizeof(char));
	memset(part, '\0', i - start + 1 );
	strncpy(part, tempStr + start, i - start);
    strcat(part, "\0");

    newQuery->filters[counterFilters].value = atoi(part);
    
    free(part);

}

query * ConstructQuery( char * queryStr, myint_t rels, myint_t joins, myint_t sums, myint_t filters, relationsheepArray relArray) {

	query * newQuery = (query *) malloc(sizeof(query));
	newQuery->numOfFilters = filters;
	newQuery->numOfJoins = joins;
	newQuery->numOfSums = sums;
	newQuery->numOfRels = rels;
	newQuery->rels = (myint_t *) malloc(rels * sizeof(myint_t));
	newQuery->filters = (filter *) malloc(filters * sizeof(filter));
	newQuery->joins = (join *) malloc(joins * sizeof(join));
	newQuery->sums = (colRel *) malloc(sums * sizeof(colRel));

	myint_t i = 0, slice = 0, start = 0, end = 0;
	myint_t counterRels = 0, counterSums = 0, counterFilters = 0, counterJoins = 0;
	while (queryStr[i] != '\0') {


	 	if( slice == 0) {


			if( queryStr[i] != ' ') {
				
				if( queryStr[i] != '|')  {
					i++;
					
					continue;
				}
				else
					slice++;
				
			}
			start = end;
			end = i + 1;
			char * tempStr = (char *) malloc((i - start + 1) * sizeof(char));
			memset(tempStr, '\0', i - start + 1);
            strncpy(tempStr, queryStr + start, i - start);
            strcat(tempStr, "\0");

            newQuery->rels[counterRels] = atoi(tempStr);


            free(tempStr);
            counterRels++;

		} 
        else if (slice == 1) {
			
			if(queryStr[i] == '&' || queryStr[i] == '|'){		//predicate str
				
				if ( queryStr[i] == '|')
					slice++;

				start = end;
				end = i + 1;
				char * tempStr = (char *) malloc((i - start + 1) * sizeof(char));
				memset(tempStr, '\0', i - start + 1);
	            strncpy(tempStr, queryStr + start, i - start);
	            strcat(tempStr, "\0");

	            if ( strstr(tempStr, "=") != NULL ) {

	            	if (SubStrOccur(tempStr, ".") == 1){ 			//filter predicate
	            		
	            		fillFilterInfo(newQuery, tempStr, counterFilters, relArray);
	            		free(tempStr);
	            		
                		counterFilters++;

	            	} else if( SubStrOccur(tempStr, ".") == 2 ) {	//join predicate

	            		fillJoinInfo(newQuery, tempStr, counterJoins, relArray);
	            		
	            		free(tempStr);
                		counterJoins++; 

	            	} else {

	            		perror("Wrong query input format");
	            		return NULL;
	            	}

	            } else if ( strstr(tempStr, ">") != NULL ) {		//filter predicate

	            	fillFilterInfo(newQuery, tempStr, counterFilters, relArray);
	            	free(tempStr);
                	counterFilters++;

	            }

	            else if ( strstr(tempStr, "<") != NULL) {			//filter predicate

	            	fillFilterInfo(newQuery, tempStr, counterFilters, relArray);
	            	free(tempStr);
                	counterFilters++;
	            }else {

	            	perror("Wrong query input format");
	            	return NULL;
	            } 
			}
	 	}
	 	else if (slice == 2){

			if( queryStr[i] == ' ') {			//col participant
				
				start = end;
				end = i + 1;
				char * tempStr = (char *) malloc((i - start + 1) * sizeof(char));
				memset(tempStr, '\0', i - start + 1 );
	            strncpy(tempStr, queryStr + start, i - start);
	            strcat(tempStr, "\0");

	            myint_t indexRel = newQuery->sums[counterSums].rel;
          
            	newQuery->sums[counterSums].col = relArray.rels[ newQuery->rels[indexRel] ].pointToCols[atoi(tempStr)];
          
	            newQuery->sums[counterSums].rows = relArray.rels[ newQuery->rels[indexRel] ].rows;
	
	            free(tempStr);
				counterSums++;

			}else if( queryStr[i] == '.') {		//rel participant

				start = end;
				end = i + 1;
				char * tempStr = (char *) malloc((i - start + 1) * sizeof(char));
				memset(tempStr, '\0', i - start + 1);
	            strncpy(tempStr, queryStr + start, i - start);
	            strcat(tempStr, "\0");

	            newQuery->sums[counterSums].rel = atoi(tempStr);
	            
	            free(tempStr);
	            
	        }

		} else {
			perror("Wrong query input format");
			return NULL;
		}
		
	 	i++;
	}

	// //last col participant
	start = end;
	end = i + 1;
	char * tempStr = (char *) malloc((i - start + 1) * sizeof(char));
	memset(tempStr, '\0', i - start + 1);
    strncpy(tempStr, queryStr + start, i - start);
    strcat(tempStr, "\0");

    myint_t indexRel = newQuery->sums[counterSums].rel;
	newQuery->sums[counterSums].col = relArray.rels[ newQuery->rels[indexRel] ].pointToCols[atoi(tempStr)];
 	newQuery->sums[counterSums].rows = relArray.rels[ newQuery->rels[indexRel] ].rows;
    
    free(tempStr);
	counterSums++;

	return newQuery;
}


void FreeQuery(query * newQuery) {

	 free(newQuery->rels);
   	 free(newQuery->filters);
   	 free(newQuery->joins);
     free(newQuery->sums);
     free(newQuery);
     return;
}