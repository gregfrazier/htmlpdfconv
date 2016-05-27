#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <tchar.h>
#include <windows.h>

const char *keywords[] = {
							"A",
							"B",
							"STRONG", 
							"EM", 
							"IMG",
							"BODY",
							"TITLE",
							"HEAD",
							"UL",
							"LI",
							"I",
							"BR",
							"P",
							"DIV",
							"DIV"};

bool isKeyword(char *wrod){
	for(int z = 0; z < 14; z++){
		if(strcmp(wrod,keywords[z]) == 0){
			return true;
		}
	}
	return false;
}

bool isImg(char *wrod){
	if(strcmp(wrod,keywords[4]) == 0){
			return true;
	}
	return false;
}

// UL is just like a BR, so just let it do a BR.
bool isUL(char *wrod){
	if(strcmp(wrod,keywords[8]) == 0){
			return true;
	}
	return false;
}

bool isLI(char *wrod){
	if(strcmp(wrod,keywords[9]) == 0){
			return true;
	}
	return false;
}

bool isBR(char *wrod){
	if(strcmp(wrod,keywords[11]) == 0 || strcmp(wrod,keywords[13]) == 0 || strcmp(wrod,keywords[12]) == 0){
			return true;
	}
	return false;
}

bool isBold(char *wrod){
	if(strcmp(wrod,keywords[1]) == 0 || strcmp(wrod,keywords[2]) == 0){
		return true;
	}
	return false;
}

bool isItalic(char *wrod){
	if(strcmp(wrod,keywords[10]) == 0 || strcmp(wrod,keywords[3]) == 0){
		return true;
	}
	return false;
}