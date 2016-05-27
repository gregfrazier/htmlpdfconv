/*
	Entity/Delimiter
	Author: Greg Frazier
	March 5, 2009
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <tchar.h>
#include <windows.h>
#include "entities.h"
#include "main.h"

bool isDelim(char c){
	for(int x=0; x < 7; x++){
		if(c == delimiters[x])
			return true;
	}
	return false;
}

bool isADelim(char c){
	for(int x=0; x < 6; x++){
		if(c == attdelim[x])
			return true;
	}
	return false;
}

bool notEDelim(char c){
	for(int x=0; x < 5; x++){
		if(c == entdelim[x])
			return false;
	}
	return true;
}

struct charpoint getEntity(char *c){
	char temp[8];
	int x = 0;
	struct charpoint r;
	if(*c == '#'){ // Character Code Entity
		while(notEDelim(*c) && x < 7){
			temp[x] = *c;
			x++;
			*c++;
		}
		//if(*c != ';'){ // This may not be an Entity?
		//	printf("DEBUG: Possible non-entity. If use you want to '&', please type it as '&amp;'\n");
		//}
		*c++;
		temp[x] = '\0';
		x = 0;
		//printf("Entity Found: %s\n",temp);
		while(x < 99){
			if(strncmp(temp, entities_num[x],6) == 0){
				//return entities_trans[x];
				r.entity = *entities_trans[x];
				r.address = c; // Send back the address
				return r;
			}
			x++;
		}
	}else if(notEDelim(*c)){
		while(notEDelim(*c) && x < 7){
			temp[x] = *c;
			x++;
			*c++;
		}
		//if(*c != ';'){ // This may not be an Entity?
		//	printf("DEBUG: Possible non-entity. If use you want to '&', please type it as '&amp;'\n");
		//}
		*c++;
		temp[x] = '\0'; 
		//printf("Entity Found: %s\n",temp);
		x = 0;
		while(x < 99){
			if(strncmp(temp, entities[x],6) == 0){
				//return entities_trans[x];
				r.entity = *entities_trans[x];
				r.address = c; // Send back the address
				return r;
			}
			x++;
		}
	}
	r.entity = '?';
	r.address = c;
	*r.address++;
	return r;
}