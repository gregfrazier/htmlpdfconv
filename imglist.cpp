#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <tchar.h>
#include <windows.h>
#include "main.h"

struct imglist *initimg(){
   struct imglist *ptr;
   ptr = (struct imglist *) calloc( 1, sizeof(struct imglist ) );
   if( ptr == NULL )
       return (struct imglist *) NULL;
   else {
       return ptr;
   }
}

char *imgName(struct imglist *ptr, int x){
	for(int y = 0; y < x; y++){
		ptr = ptr->next;
		if(ptr == NULL){
			break;}
	}
	return ptr->filename;
}