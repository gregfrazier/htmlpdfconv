#include "main.h"

SCREEN screen;

void setHref(char *c){
	int i = 0;
	while(*c != '\0'){
		screen.href[i] = *c;
		*c++;
		i++;
	}
	return;
}

int getLinePos(){
	return screen.posLine;
}

long getLine(){
	return screen.posPage;
}

long getPage(){
	return screen.numPage;
}

float getX(){
	return screen.x;
}

float getY(){
	return screen.y;
}

bool getBold(){
	return screen.bold;
}

bool getAnchor(){
	return screen.anchor;
}

bool getUnderline(){
	return screen.underline;
}

bool getItalic(){
	return screen.italic;
}

void setX(float x){
	screen.x = x;
	return;
}

void setY(float y){
	screen.y = y;
	return;
}

void setBold(bool c){
	screen.bold = c;
	return;
}

void setUnderline(bool c){
	screen.underline = c;
	return;
}

void setItalic(bool c){
	screen.italic = c;
	return;
}

void setHyperlink(bool c){
	screen.hyperlink = c;
	return;
}

void setAnchor(bool c){
	screen.anchor = c;
	return;
}

void setLinePos(int c){
	screen.posLine = c;
	return;
}

void setLine(long c){
	screen.posPage = c;
	return;
}

void initScreen(float x, float y, int line, long page){
	//screen.color = "000000";
	screen.bold = false;
	screen.anchor = false;
	screen.hyperlink = false;
	screen.italic = false;
	screen.underline = false;
	screen.x = x;
	screen.y = y;
	screen.posLine = line;
	screen.posPage = page;
	screen.numPage = 0;
	return;
}