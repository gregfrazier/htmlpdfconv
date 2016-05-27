#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <tchar.h>
#include <windows.h>

#include "main.h"

const char *attributes[] = {
							"HREF",
							"SRC",
							"STYLE",
							"HEIGHT",
							"WIDTH",
							"ALT"};

bool isHyperlink(char *wrod){
	char http[8] = "http:\\\\";
	printf("WROD: %s\n",wrod);
	for(int z = 0; z < 7; ++z){
		printf("%c = %c\n",wrod[z], http[z]);
		if(wrod[z] != http[z]){
			return false;
		}
	}
	return true;
}

bool isAttribute(char *wrod){
	for(int z = 0; z < 6; z++){
		if(strcmp(wrod,attributes[z]) == 0){
			return true;
		}
	}
	return false;
}

bool isSrc(char *wrod){
	if(strcmp(wrod,attributes[1]) == 0){
			return true;
	}
	return false;
}

bool isOther(char *wrod){
	for(int z = 0; z < 6; z++){
		if(strcmp(wrod,attributes[z]) == 0){
			return true;
		}
	}
	return false;
}

struct imgret getValue(char *h, bool getval, char *jpegpath){
	int x;
	char *orig_target = h;
	struct imgret image;
	char *testbatch;


	// get rid of whitespace
	while(*h == ' ' || *h == '='){
		*h++;
	}
	while(*h != '>'){
		if(!isADelim(*h)){ //*h >= 'a' && *h <= 'z' || *h >= 'A' && *h <= 'Z' || *h == '_' || *h >= '0' && *h <= '9'){
			//get the entire word
			x = 0;
			testbatch = (char*)malloc(sizeof(char)*4097);
			do{
				testbatch[x] = toupper(*h);
				x++;
				*h++;
				if(*h == '\r' || *h == '\n'){*h++;} // Allows CRLF but cuts it off
			}while(!isADelim(*h)); //*h != ' ' && *h != '>' && *h != '=' && *h != '"');
			testbatch[x] = '\0';
			//printf("%s",testbatch);
			if(isAttribute(testbatch) == true){
				if(isSrc(testbatch) == true){
					#ifdef VERBOSITY
					printf("  --> isSrc(%s) found\n", testbatch);
					#endif
					image.type = 4;
				}else if(isOther(testbatch) == true){
					#ifdef VERBOSITY
					printf("  --> isOther(%s) found\n", testbatch);
					#endif
					image.type = 1;
				}else{image.type = 1;}
			}
			free(testbatch);
		}else if(*h == '='){ // Equality, Something Equals a Value.
			// Since this program is dumb, it doesn't understand anything beyond SRC, it will parse out everything
			// but it won't save any values unless the current struct 'image' has a value of SRC (4)
				//*h == '"' || *h >= 30 && *h <= 39 && getval == true){
				bool instring = false;
				bool singleBlock = false;
				bool doubleBlock = false;
				bool endBlock = false;
				*h++;
				if(*h == '"'){
					instring = true;
					doubleBlock = true;
					*h++;
				}else if(*h == '\''){
					instring = true;
					singleBlock = true;
					*h++;
				}
				x = 0;
				testbatch = (char*)malloc(sizeof(char)*4097);
				do{
					if(*h == ' '){if(instring == false){break;}}
					if(*h == '"'){if(doubleBlock == true){break;}}
					if(*h == '\''){if(singleBlock == true){break;}}
					if(*h == '\0'){break;}
					testbatch[x] = *h;
					
					// Convert / to Windows \ and then remove leading \ if the path has a trailing one.
					if(*h == '/'){ testbatch[x] = '\\';}
					if(*h == '/' || *h == '\\'){if(x == 0){if(jpegpath[strlen(jpegpath)-1] == '\\' || jpegpath[strlen(jpegpath)-1] == '/'){x = -1;}}}					
					x++;
					*h++;
				}while(x < 4094); //*h != '"');
				testbatch[x] = '\0';
				char *imgval;
				imgval = (char *)malloc(sizeof(char)*strlen(jpegpath)+(x*2));
				sprintf(imgval,"%s%s",jpegpath,testbatch);
				if(isHyperlink(testbatch))
				{
					// It's a linked image, so download it.
					printf("IS HYPERLINK\n");
				}
				free(testbatch);
				strncpy(image.value, imgval, strlen(imgval)+1);
				//image.value[strlen(imgval)] = '\0';
				*h++;
				#ifdef VERBOSITY
				printf("   +-> Value Found: %s\n",image.value);
				#endif
				if(image.type == 4){
					return image;
				}
		}else if(*h == ' ' || *h == '\r' || *h == '\n'){ //Skip Whitespace, CR, LF
			*h++;
		}else{
			*h++; // Probably nothing important? So skip it.
		}
	}
	//printf("There is NO SRC\n");
	image.value[0] = '0';
	image.value[1] = '\0';
	return image;
}

// Old Parser
// Just here for reference. Don't use.
/*void parser(char *h, HPDF_Page page, HPDF_Doc pdf, HPDF_Font font){
	//Dump Text to PDF
	char *c = h;
	char *d;
	char *buffer;
	long length;
	long filelength = 0;
	long linepos = 0;
	bool insideB=false;
	bool isClosing=false;
	bool lineIMG=false;
	int nLine = 100;
	int LastInput = 0;
	long lengBR = 0;
	long heightBR = (long)HPDF_Page_GetHeight(page)-8;
	long heightIMG = 0;
	X_Y j;
	//
	//  0 = nothing
	//  1 = Character
	// *  2 = Whitespace
	// *  3 = <BR>
	// *  4 = Other tag
    // *******************
	char testbatch[4096]; //a tag shouldn't be more than 4kb big
	int x, n;
	
	//Allocate Buffer
	//buffer = (char *)malloc(nLine);
	screen.line = (char *)malloc(nLine);
	n = 0; //character count
	
	while(*c != '\0'){
		if(*c == '<' && insideB==false){
			// Open tag
			insideB=true;
			*c++;
		}else if(*c == '>' && insideB==true){
			// Close out the tag
			insideB=false;
			isClosing=false;
			*c++;
		}else if(*c == '/' && insideB==true){
			*c++;
			char *tp = c;
			while(*tp == ' '){*tp++;}
			if(*tp == '>'){c = tp;}
			isClosing = true;
		}else if(*c >= 'a' && *c <= 'z' || *c >= 'A' && *c <= 'Z' || *c == '_' || *c == '^' || *c >= '0' && *c <= '9' || *c == '-' || *c == '.'){
			if(insideB==true)
			{
				x = 0;
				do{
					testbatch[x] = *c;
					x++;
					*c++;
				}while(*c != ' ' && *c != '>' && *c != '=' && *c != '"');
				testbatch[x] = '\0';
				//printf("T: '%s'\n", testbatch);
				if(isKeyword(testbatch) == true){
					printf("---> Keyword(%s) found\n", testbatch);
					if(isImg(testbatch) == true){
						printf(" --> isImg(%s) found\n", testbatch);
						LastInput = 4;
						// Display the current buffer before displaying the picture
						// --------------------------------------------------------
							//buffer[n] = '\0';
							//n = 0;
							//printf("BUFFER: %s\n",buffer);
							//HPDF_Page_ShowTextNextLine(page, buffer);
							//currentStart += -8.0;
							//setY(getY()-8.0);
							
						// --------------------------------------------------------
						//draw_image(pdf, getValue(c), 0, getY());
						// need to return the X and the Y!!!!
						j = draw_image(pdf,getValue(c),lengBR,heightBR+8);
						heightIMG = j.y;
						lengBR += j.x;
						lineIMG = true;
						printf("   +-> Image has Height: %i\n",j.y);
						printf("   +-> Image has Width: %i\n\n",j.x);
						//HPDF_Page_MoveTextPos (page, 0, getY());
					}else if(isBR(testbatch) == true){
						printf(" --> isBR(%s) found\n", testbatch);
						LastInput = 3;
						lengBR = 0;
						n = 0;
						printf("  +-> Setting Line Row from: %i, %i\n\n", heightBR, heightIMG);
						if(lineIMG == true){
							heightBR = heightBR - heightIMG;
							lineIMG = false;
						}else{
							heightBR -= 8;
						}
						//check if you need to goto the next page
						if(heightBR < 1){
							//Create New Page
								page = initPage(pdf);
								initScreen(0, 0, 1, 1);
								setY(setTextTop(page,font));
								heightBR = (long)HPDF_Page_GetHeight(page)-8;
						}
						printf("  +-> Setting Line Row to: %i\n\n", heightBR);
						//
					}else if(isItalic(testbatch) == true){
						if(isClosing == true){
							printf(" --> isItalicClosing(%s) found\n\n", testbatch);
							font = HPDF_GetFont (pdf, "Helvetica", NULL);
							HPDF_Page_SetFontAndSize (page, font, 8);
						}else{
							printf(" --> isItalic(%s) found\n\n", testbatch);
							font = HPDF_GetFont (pdf, "Helvetica-Oblique", NULL);
							HPDF_Page_SetFontAndSize (page, font, 8);
						}
					}else if(isBold(testbatch) == true){
						if(isClosing == true){
							printf(" --> isBoldClosing(%s) found\n\n", testbatch);
							font = HPDF_GetFont (pdf, "Helvetica", NULL);
							HPDF_Page_SetFontAndSize (page, font, 8);
						}else{
							printf(" --> isBold(%s) found\n\n", testbatch);
							font = HPDF_GetFont (pdf, "Helvetica-Bold", NULL);
							HPDF_Page_SetFontAndSize (page, font, 8);
						}
					}else{
						printf(" --> Not Supported(%s)\n\n", testbatch);
					}
				}
			}else{
				// Put text in buffer for display at CR, BR, or EOF
				//if( n+100>nLine ){
				//	nLine = nLine*2 + 100;
				//	screen.line = (char *)realloc(screen.line, nLine);
				//}
				//screen.line[n] = *c;
				LastInput = 1;
				if(n > 200){
					n=0;
					//Goto next line
					//HPDF_Page_MoveTextPos (page, 0-lengBR, -8);
					lengBR = 0;
					heightBR -= 8;
				}
				//char *i = c;
				char i[2];
				i[0] = c[0];
				i[1] = '\0';
				//HPDF_Page_MoveTextPos (page, ((textWidthStruct.width*8)/1000), 0);
				HPDF_Page_TextOut(page, lengBR,heightBR,i);
				textWidthStruct = HPDF_Font_TextWidth(font, (HPDF_BYTE *) i, 1);
				lengBR += ((textWidthStruct.width*8)/1000);
				//HPDF_Page_ShowText(page, i);
				//printf("%i, ",lengBR);
				//lengBR += ((textWidthStruct.width*8)/1000);
				*c++;
				n++;
			}
		}else if(*c == '\r'){
			// add a space for carriage return
			if(LastInput != 2 &&  LastInput != 3){
				if(n > 200){
					n=0;
					//Goto next line
					//HPDF_Page_MoveTextPos (page, 0-lengBR, -8);
					lengBR = 0;
					heightBR -= 8;
				}
				//HPDF_Page_MoveTextPos (page, ((textWidthStruct.width*8)/1000), 0);
				//textWidthStruct = HPDF_Font_TextWidth(font, (HPDF_BYTE *) " ", 1);
				//HPDF_Page_ShowText(page, " ");
				//lengBR += ((textWidthStruct.width*8)/1000)+1;
				HPDF_Page_TextOut(page, lengBR,heightBR," ");
				textWidthStruct = HPDF_Font_TextWidth(font, (HPDF_BYTE *) " ", 1);
				lengBR += ((textWidthStruct.width*8)/1000);
				//HPDF_Page_ShowText(page, " ");
				LastInput = 2;
				n++;
			}
			*c++;
		}else if(*c == ' ' && insideB==false){
			if(LastInput != 2 &&  LastInput != 3){
				if(n > 200){
					n=0;
					//Goto next line
					//HPDF_Page_MoveTextPos (page, 0-lengBR, -8);
					lengBR = 0;
					heightBR -= 8;
				}
				//HPDF_Page_MoveTextPos (page, ((textWidthStruct.width*8)/1000), 0);
				//textWidthStruct = HPDF_Font_TextWidth(font, (HPDF_BYTE *) " ", 1);
				//HPDF_Page_ShowText(page, " ");
				HPDF_Page_TextOut(page, lengBR,heightBR," ");
				textWidthStruct = HPDF_Font_TextWidth(font, (HPDF_BYTE *) " ", 1);
				lengBR += ((textWidthStruct.width*8)/1000);
				//HPDF_Page_ShowText(page, " ");
				//printf("%i, ",((textWidthStruct.width*8)/1000));
				//lengBR += ((textWidthStruct.width*8)/1000)+1;
				LastInput = 2;
				n++;
			}
			*c++;
		}else if(*c == ' ' && insideB==true){
			*c++;
		}else{
			// Todo
			c++;
		}
	}
	if(*c == '\0'){
		//Last Read
		//screen.line[n] = '\0';
		//n = 0;
		//HPDF_Page_ShowTextNextLine(page, buffer);
		//setY(getY()-8.0);
		//c++;
	}
	free(screen.line);
	return;
}*/