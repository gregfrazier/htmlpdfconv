/*
    HTML2PDF
    Feb/Mar-2009 Gregory Frazier

    Uses libHARU for PDF drawing functions.
	Uses libTidy for correcting invalid HTML.

	For Debug Mode: Enabled "VERBOSITY" in MAIN.H
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <tchar.h>
#include <windows.h>

#include "main.h"
#include "tidyinc/tidy.h"
#include "tidyinc/buffio.h"

#define BM 0x4D42
#define BUFFER_SIZE 102
HPDF_TextWidth textWidthStruct;
struct imglist *imgroot;
char *jpegpath;

typedef struct RET_TEXT
{
	char* filecontents;
	long filesize;
} RET_TEXT, *RET_TEXT_PTR;

typedef struct node
{
	char *d;
	char entity;
	long clen;
	long w;
	long h;
	long x;
	long y;
	bool isKey;
	int tagType;
	int imgnum;
	struct node *next;
} NODE;

int charSize[95];
long pageHeight;

// Sets the Right Margin
long pagewidth = 555;
// Sets the Top Margin
long pageMargin = 30; //120;
// Sets the Left Margin
long leftMargin = 55;
// Sets the Bottom Margin
long botMargin = 20;

long fontSize;

int pageCount;

struct node *initnode()
{
   struct node *ptr;
   ptr = (struct node *) calloc( 1, sizeof(struct node ) );
   if( ptr == NULL )
       return (struct node *) NULL;
   else {
       return ptr;
   }
}

// Creates the Lookup Table for ASCII Character's Width in the Selected Font.
void setPDFFontMetrics(HPDF_Font font){
	HPDF_TextWidth textWidthStruct;
	for(int x = 0; x < 95; x++){
		// This is stupid, but it only supports character arrays (String)
		char i[2];
		i[0] = x+32;
		i[1] = '\0';
		textWidthStruct = HPDF_Font_TextWidth(font, (HPDF_BYTE *) i, 1);
		charSize[x] = ((textWidthStruct.width*fontSize)/1000);
		if(charSize[x] < 3){charSize[x]++;}
	}
	return;
}

long wordWidth(char *t, long length, bool bold, HPDF_Doc pdf){
	HPDF_TextWidth textWidthStruct;
	HPDF_Font font;
	long total = 0;
	if(bold){font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);}else{font = HPDF_GetFont(pdf, "Helvetica", NULL);}
	textWidthStruct = HPDF_Font_TextWidth(font, (HPDF_BYTE *) t, length);
	total = ((textWidthStruct.width*fontSize)/1000);
	return total;
}

long charWidth(char t){
	if(t >= 32 && t <= 126){
		return charSize[t-32];
	}else{ //Unknown, return 5, seems to be a decent size.
		return 5;
	}
}

void error_handler (HPDF_STATUS   error_no,
               HPDF_STATUS   detail_no,
               void         *user_data)
{
    printf ("\nlibHARU Error: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
    return;
}

X_Y draw_image(HPDF_Doc pdf, const char *filename, float x, float y, float w, float h)
{
    const char* FILE_SEPARATOR = "\\";
//    char filename1[255];
	X_Y j;

    HPDF_Page page = HPDF_GetCurrentPage(pdf);
    HPDF_Image image;
    image = HPDF_LoadJpegImageFromFile(pdf, filename);
	j.y = (long)w; //HPDF_Image_GetHeight(image);
	j.x = (long)h; //HPDF_Image_GetWidth(image);

    /* Draw image to the canvas. */
	HPDF_Page_EndText(page);
    HPDF_Page_DrawImage(page, image, x+2, y-(j.y+2), j.x, j.y);
	HPDF_Page_BeginText (page);
    HPDF_Page_SetTextLeading (page, fontSize);
	return j;
}

char *LoadFile(char *pszFileName, long *lgSize)
{
   	FILE *TARGET;
	char *mem;
	char *ptr2;
	int i;
	long h = 0;

	TARGET = fopen(pszFileName,"rb+");

	if(!TARGET)
	{
		printf("LoadFile(): Unable to open file specified.\n");
		*lgSize = 0;
		return 0;
	}
	
	fseek(TARGET, 0, SEEK_END);
	h = ftell(TARGET);
	
	mem = (char*) malloc(h+1);
	ptr2 = &mem[0];
	if(mem == NULL)
	{
		printf("LoadFile(): Could Not Allocate Memory.\n");
		*lgSize = 0;
		return 0;
	}
	fseek(TARGET,0-h,SEEK_END);
	
	for(i=0;i<h;i++)
	{
		*ptr2 = (char)fgetc(TARGET);
		*ptr2++;
	}
	*ptr2 = (char)'\0';
	*lgSize = h;
	fclose(TARGET);
	return mem;
}

// No Longer in Use
long getParent(struct node *ptr, struct node *ptr2){
	long x;
	while(ptr->next != ptr2){
		ptr = ptr->next;
	}
	x = ptr->tagType;
	return x;
}

void printnode( struct node *ptr )
{
	if(ptr->isKey == false){
		char *p;
		p = (char*)malloc(sizeof(char)*ptr->clen+2);
		strncpy(p,ptr->d,ptr->clen);
		p[ptr->clen] = '\0';
		printf("[User Defined %s]",p);
	}else{
		printf("[Keyword, %i]",ptr->tagType);
	}
	return;
}

// This dumps the nodes for debug mode
void displayNodes( struct node *ptr ){
	printf("---------------------\n");
	printf("Node Dump:\n");
	printf("---------------------\n");
	while(ptr){
		printnode( ptr );
		ptr = ptr->next;
    }
    printf("\n---------------------\n\n");
}

// Very Slow! Don't Use
void setParentHeight(struct node *ptr, struct node *ptr2, long height, bool recursive){
	struct node *root;
	root = ptr;

	while(ptr->next != ptr2){
		ptr = ptr->next;
	}
	//recurse until parent is type BR
	if(ptr->tagType != 11 && ptr->tagType != -1 && ptr->tagType != 200){
		// ptr is now the parent of ptr2. Set it's height.
		if(ptr->tagType != 4){
			ptr->y = height;
		}else{
			ptr->y = height + ptr->h;
		}
		if(recursive == true){
			setParentHeight(root,ptr,height,true);
		}
	}
	return;
}

// Non-Searching Version of the Above
void setParentHeight2(struct node *ptr, struct node *ptr2, long height, bool recursive){
	struct node *root;
	root = ptr; // This is the "LastBreak" node.

	while(ptr->next != ptr2){
		if(ptr->tagType != 4){
			ptr->y = height;
		}else{
			ptr->y = height + ptr->h;
		}
		ptr = ptr->next;
	}
	return;
}

// No Longer in Use
void insertNewPage2(struct node *ptr, struct node *ptr2){
	struct node *root;
	struct node *newnode = initnode();
	root = ptr;
	int x=0;
//	int last;
	// Find the Parent Node
	while(ptr->next != ptr2){
		ptr = ptr->next;
	}
	// Set the Parent's Node Child to the *new* Line Break Node
	ptr->next = newnode;
	// Set the Line Break Node as the Parent to the Current Node
	newnode->next = ptr2;
	// Setup Line Break Properties
	newnode->isKey = true;
	newnode->tagType = 200;
	//newnode->c = ' ';
	newnode->x = x;
	newnode->y = pageHeight-fontSize;
	pageCount++;
	return;
}

// Fastest Method
struct node *insertNewPage3(struct node *ptr, struct node *ptr2){
	struct node *newnode = initnode();
	ptr->next = newnode;
	newnode->next = ptr2;
	newnode->isKey = true;
	newnode->tagType = 200;
	newnode->x = 0;
	newnode->y = pageHeight-fontSize;
	pageCount++;
	return newnode;
}

// No Longer In Use
struct node *insertNewPage(struct node *ptr, struct node *ptr2, long offsetx, long blah){
	struct node *root;
	struct node *newnode = initnode();
	root = ptr;
	int x=0;
	int last=0;
	while(ptr->next != ptr2){
		ptr = ptr->next;
		if(ptr->tagType == 11 || ptr->tagType == -1 || ptr->tagType == 200){
			last = x;
		}
		x++;
	}
	ptr = root; // Reset it to the beginning
	for(int y = 0; y <= last; y++){
		ptr = ptr->next;
	}
	ptr->next = newnode;
	// Set the Line Break Node as the Parent to the Current Node
	newnode->next = ptr2;
	// Setup Line Break Properties
	newnode->isKey = true;
	newnode->tagType = 200;
	newnode->x = 0;
	newnode->y = pageHeight-fontSize;
	pageCount++;
	return newnode;
}

struct node* insertLineBreak(struct node *ptr, struct node *ptr2, long x, long y){
	struct node *newnode = initnode();
	// Set the Parent's Node Child to the *new* Line Break Node
	ptr->next = newnode;
	// Set the Line Break Node as the Parent to the Current Node
	newnode->next = ptr2;
	// Setup Line Break Properties
	newnode->isKey = true;
	newnode->tagType = 11;
	newnode->x = x;
	newnode->y = y;
	return newnode;
}

// Very Slow, Don't Use
long findHighest(struct node *ptr, struct node *ptr2, long high){
	// Find the highest height in the current line
	long highest, highest2;
	bool higher = false; // True if there is a node with the same height
	struct node *root;
	root = ptr;
	//Say that 0 is the highest so far.
	highest = high;
	while(ptr->next != ptr2){
		ptr = ptr->next;
	}
	// Recurse until parent is type BR
	if(ptr->tagType != 11 && ptr->tagType != -1){
		// ptr is now the parent of ptr2. Get it's height.
		highest2 = ptr->h;
		// Child Versus Parent Height
		//printf("Highest: %i (%i)\n",ptr->h, ptr->tagType);
		if(highest2 >= highest){
			highest = highest2;
		}
		highest = findHighest(root,ptr, highest);
	}
	return highest;
}

// Faster Version of the Above
long findHighest2(struct node *ptr, struct node *ptr2, long high){
	long highest, highest2;
	bool higher = false;
	struct node *root;
	root = ptr;
	highest = high;
	while(ptr->next != ptr2){
		highest2 = ptr->h;
		//printf("Highest: %i (%i)\n",ptr->h, ptr->tagType);
		if(highest2 >= highest){
			highest = highest2;
		}
		ptr = ptr->next;
	}
	return highest;
}

// Tokenizes the HTML for the Renderer
struct node* preprocess(char *h, HPDF_Doc pdf){
	/* This pre-processes the file to determine page length, image sizes, font sizes, etc. */
	char *c = h;
	struct node *root;
	struct node *txt;
	
	// Retainer Nodes
	struct node *lastBreak;
	struct node *lastNode;

	struct imglist *imgnext;

	char testbatch[4096];
	struct imgret image;
	long indentModifier = 0;
	long lengBR = leftMargin + indentModifier;
	long heightBR = getY()-(pageMargin+fontSize);
	long heightIMG = 0;
	bool insideB = false;
	bool isClosing = false;
	bool skip = false;
	bool isEM = false;
	int x, imgnumber;
#ifdef VERBOSITY	
	printf("Preprocessing...\n");
	printf("Building Node Tree...\n");
	printf("Page Height: %i\n",pageHeight);
#endif
	root = initnode();
	root->tagType = -1;
	txt = initnode();
	root->next = txt;

	lastBreak = root;
	lastNode = root;

	//Image List
	imgroot = initimg();
	imgnext = initimg();
	imgroot->next = imgnext;
	imgnumber = 1;

	int numfunc;
	while(*c != '\0'){
		numfunc = 0;
		if(*c == '<' && insideB==false){
			// Open tag
			insideB=true;
			*c++;
			skip = true;
			numfunc = 1;
		}else if(*c == '>' && insideB==true){
			// Close out the tag
			insideB=false;
			isClosing=false;
			*c++;
			skip = true;
			numfunc = 2;
		}else if(*c == '/' && insideB==true){
			*c++;
			char *tp = c;
			while(*tp == ' '){*tp++;}
			if(*tp == '>'){c = tp;}
			isClosing = true;
			skip = true;
			numfunc = 3;
		}else if(!isDelim(*c)){
			if(insideB==true)
			{
				numfunc = 4;
				x = 0;
				do{
					testbatch[x] = toupper(*c);
					x++;
					*c++;
				}while(*c != ' ' && *c != '>' && *c != '=' && *c != '"');
				testbatch[x] = '\0';
				if(isKeyword(testbatch) == true){
					#ifdef VERBOSITY
					printf("---> Keyword(%s) found\n", testbatch);
					#endif
					if(isImg(testbatch) == true){
						X_Y u;
						bool printme = false;
						#ifdef VERBOSITY
						printf(" --> isImg(%s) found\n", testbatch);
						#endif
						txt->isKey = true;
						txt->tagType = 4;

						// Get Image Dimensions
						image = getValue(c, false,jpegpath);
						if(strncmp(image.value, "0",strlen(image.value)) == 0){skip = true;}
						if(image.type != 4){skip = true;}
						strcpy(imgnext->filename,image.value);
						if(skip != true){
							u = openJpeg(imgnext->filename);
						}else{
							u.x = 0; 
							u.y = 0;
						}
						if(u.toJPG == true){sprintf(imgnext->filename,"%s.jpg",imgnext->filename);}
						if(u.x != 0){
							if(u.x >= pagewidth){
								// This image is too big to fit on the page
								// Get aspect ratio and resize the image to the largest possible size
								u.y = ((((pagewidth)*1000)/u.x)*u.y)/1000;
								u.x = pagewidth - lengBR;
							}

							// Check if the Height is too large
							if(u.y >= 500){
								u.x = (((500*1000)/u.y)*u.x)/1000;
								u.y = 500;
							}
							txt->h = u.y;

							txt->imgnum = imgnumber;
							imgnumber++;
							
							// Allocate a new Image Node.
							struct imglist *imgnext2 = initimg();
							imgnext->next = imgnext2;
							imgnext2->next = '\0';
							imgnext = imgnext2;

							// Is the image too big to fit on the current line?
							if(pagewidth <= lengBR + u.x){
								// Insert BR Node
								// Line Height and Line Wrapping Done Here.
								// Do a final line wrap at the end of the Document (incase there are no BRs)
								#ifdef VERBOSITY
								printf(" --> Page Width Overflow\n");
								printf("  +-> Adjusting Prev Line Height\n");
								#endif
								// Find Largest Height
#ifdef FALLBACK
								long highest = findHighest(root, txt, 0);
#else
								long highest = findHighest2(lastBreak, txt, 0);
#endif

								if(highest != fontSize){  // There are pictures in the line
									long tempH = heightBR - highest;									
									#ifdef VERBOSITY
									printf("   +-> Setting the Line-Height to: %i (%i)\n",tempH, heightBR);
									#endif
									// Recursively set all other items on this line at the new height.
									setParentHeight2(lastBreak,txt,tempH,true);
									heightBR = tempH;
								}
								lastBreak = insertLineBreak(lastNode,txt,lengBR,heightBR);
								lengBR = leftMargin + indentModifier;
								#ifdef VERBOSITY
								printf("  +--> Inserted Line Break \n");
								#endif
							}

							// Check for Potential Page Overflow
							if(heightBR < u.y + botMargin){
								lastNode = insertNewPage3(lastNode,txt);
								lastBreak = lastNode;
								heightBR = pageHeight-(pageMargin+fontSize);
							}

							//txt->h = u.y;
							txt->w = u.x;

							// Set where the image is going to be displayed
							txt->x = lengBR;
							txt->y = heightBR;
							lengBR += u.x;
						}else{
							*c++; //Not exactly the best method, but it will work
							skip = true;}
						#ifdef VERBOSITY
						printf("   +-> Image has Height: %i\n",u.y);
						printf("   +-> Image has Width: %i\n\n",u.x);
						#endif
					}else if(isBR(testbatch) == true){
						// Line Height and Line Wrapping Done Here.
						// Do a final line wrap at the end of the Document (incase there are no BRs)
						#ifdef VERBOSITY
						printf(" --> isBR(%s) found\n", testbatch);
						printf("  +-> Adjusting Prev Line Height\n");
						#endif
						// Find Largest Height
#ifdef FALLBACK
								long highest = findHighest(root, txt, 0);
#else
								long highest = findHighest2(lastBreak, txt, 0);
#endif
						//printf("Highest: %i | Font: %i\n",highest,fontSize);
						if(highest != fontSize){  // There are pictures in the line
									long tempH = heightBR - highest;									
									#ifdef VERBOSITY
									printf("   +-> Setting the Line-Height to: %i (%i)\n",tempH, heightBR);
									#endif
							// Recursively set all other items on this line at the new height.
							setParentHeight2(lastBreak, txt, tempH, true);
							heightBR = tempH;
						}
						txt->isKey = true;
						txt->tagType = 11;

						lengBR = leftMargin + indentModifier;
						heightBR -= fontSize; // 8 = font size
						// Check Height... Goto a New Page if it's Zero or less
						if(heightBR < fontSize+botMargin){
							lastNode = insertNewPage3(lastNode, txt);
							heightBR = pageHeight-(pageMargin+fontSize);
						}
						txt->x = 0;
						txt->y = heightBR;
						lastBreak = txt;
					}else if(isItalic(testbatch) == true){
						if(isClosing == true){
							#ifdef VERBOSITY
								printf(" --> isItalicClosing(%s) found\n\n", testbatch);
							#endif
							txt->isKey = true;
							txt->tagType = 110;
						}else{
							#ifdef VERBOSITY
							printf(" --> isItalic(%s) found\n\n", testbatch);
							#endif
							txt->isKey = true;
							txt->tagType = 10;
						}
					}else if(isBold(testbatch) == true){
						if(isClosing == true){
							#ifdef VERBOSITY
							printf(" --> isBoldClosing(%s) found\n\n", testbatch);
							#endif
							txt->isKey = true;
							txt->tagType = 101;
							isEM = false;
						}else{
							#ifdef VERBOSITY
							printf(" --> isBold(%s) found\n\n", testbatch);
							#endif
							txt->isKey = true;
							txt->tagType = 1;
							isEM = true;
						}
					}else if(isUL(testbatch) == true){  // Bullets
						//If it's a UL, modify it's Indentation
						if(isClosing == true){
							#ifdef VERBOSITY
							printf(" --> isULClosing(%s) found\n\n", testbatch);
							#endif
							indentModifier -= 20;
#ifdef FALLBACK
								long highest = findHighest(root, txt, 0);
#else
								long highest = findHighest2(lastBreak, txt, 0);
#endif
							if(highest != fontSize){long tempH = heightBR - highest;setParentHeight2(lastBreak, txt, tempH, true); heightBR = tempH;}
							txt->isKey = true; txt->tagType = 11;
							lengBR = leftMargin + indentModifier; heightBR -= fontSize;
							if(heightBR < fontSize+botMargin){
								lastNode = insertNewPage3(lastNode, txt);
								heightBR = pageHeight-(pageMargin+fontSize);
							}
							txt->x = 0;
							txt->y = heightBR;
							lastBreak = txt;
						}else{
							indentModifier += 20;
							#ifdef VERBOSITY
							printf(" --> isUL(%s) found\n\n", testbatch);
							#endif
							txt->isKey = true;
							txt->tagType = 13;
						}
					}else if(isLI(testbatch) == true){  // Bullets
						if(isClosing == false){
							#ifdef VERBOSITY
							printf(" --> isLI(%s) found\n\n", testbatch);
							#endif
#ifdef FALLBACK
								long highest = findHighest(root, txt, 0);
#else
								long highest = findHighest2(lastBreak, txt, 0);
#endif
							if(highest != fontSize){long tempH = heightBR - highest;setParentHeight2(lastBreak, txt, tempH, true); heightBR = tempH;}
							lengBR = leftMargin + indentModifier; heightBR -= fontSize;
							if(heightBR < fontSize+botMargin){
								lastNode = insertNewPage3(lastNode, txt);
								heightBR = pageHeight-(pageMargin+fontSize);
							}
							lastBreak = insertLineBreak(lastNode,txt,lengBR,heightBR);
							txt->isKey = true;
							txt->tagType = 2;
							txt->x = lengBR - 10;
							txt->y = heightBR;
						}else{
							#ifdef VERBOSITY
							printf(" --> isLIClosing(%s) found\n\n", testbatch);
							#endif
							txt->isKey = true;
							txt->tagType = 102;
							txt->x = 0;
							txt->y = heightBR;
						}
					}else{
						#ifdef VERBOSITY
						printf(" --> Not Supported(%s)\n\n", testbatch);
						#endif
						skip = true;
					}
				}else{skip = true;}
			}else{
				numfunc = 5;
				// Put text in buffer for display at CR, BR, or EOF
				txt->d = c;  // Set the Node's Pointer to the Location of the Word in FileContents
				long wordlength = 0;
				int lengthofword = 0;
				char *tempstring;
				// Find word and it's length
				while(!isDelim(*c)){
					lengthofword++;
					*c++;
				}
				txt->clen = lengthofword;
				txt->isKey = false;

				tempstring = (char*)malloc(sizeof(char)*lengthofword+2);
				strncpy(tempstring,txt->d,lengthofword);
				tempstring[lengthofword] = '\0';
				wordlength = wordWidth(tempstring, txt->clen, isEM, pdf);
				free(tempstring);

				//-----------------------------------------------------------------------------
				// Massive Margin-Checking Code
				//-----------------------------------------------------------------------------
				if(pagewidth <= lengBR + wordlength){
					// Insert BR Node
					// Line Height and Line Wrapping Done Here.
					// Do a final line wrap at the end of the Document (incase there are no BRs)
					#ifdef VERBOSITY
					printf(" --> Page Width Overflow %i\n", __LINE__);
					printf("  +-> Adjusting Prev Line Height\n");
					#endif
					// Find Largest Height
#ifdef FALLBACK
					long highest = findHighest(root, txt, 0);
#else
					long highest = findHighest2(lastBreak, txt, 0);
#endif
					if(highest != fontSize){  // There are pictures in the line
						long tempH = heightBR - highest;						
						#ifdef VERBOSITY
						printf("   +-> Setting the Line-Height to: %i (%i)\n",tempH, heightBR);
						#endif
						// Recursively set all other items on this line at the new height.
						setParentHeight2(lastBreak, txt, tempH, true);
						heightBR = tempH - fontSize;
					}else{
						heightBR -= fontSize;
					}
					lastBreak = insertLineBreak(lastNode,txt,lengBR,heightBR);
					if(heightBR < (2*fontSize) + botMargin){
						lastNode = insertNewPage3(lastNode, txt);
						heightBR = pageHeight-(pageMargin+fontSize);
					}
					lengBR = leftMargin + indentModifier;
					#ifdef VERBOSITY
					printf("  +--> Inserted Line Break \n");
					#endif
				}
				//-----------------------------------------------------------------------------
				txt->y = heightBR;
				txt->x = lengBR;
				txt->h = fontSize;

				lengBR += wordlength;
			}
		}else if(*c == '\r' && insideB==false){
			numfunc = 6;
			// add a space for carriage return
			long lastinput = lastNode->tagType;
			/*if(lastinput != 32 && lastinput != 11 && lastinput != -1){
				txt->isKey = true;
				txt->tagType = 32;
				//-----------------------------------------------------------------------------
				// Massive Margin-Checking Code
				//-----------------------------------------------------------------------------
				if(pagewidth <= lengBR + charWidth(32)){
					// Insert BR Node
					// Line Height and Line Wrapping Done Here.
					// Do a final line wrap at the end of the Document (incase there are no BRs)
					#ifdef VERBOSITY
					printf(" --> Page Width Overflow2\n");
					printf("  +-> Adjusting Prev Line Height\n");
					#endif
					// Find Largest Height
#ifdef FALLBACK
					long highest = findHighest(root, txt, 0);
#else
					long highest = findHighest2(lastBreak, txt, 0);
#endif
					printf("Overflow2 Highest: %i | %i\n",highest,fontSize);
					if(highest != fontSize){  // There are pictures in the line
						long tempH = heightBR - highest;									
						#ifdef VERBOSITY
						printf("   +-> Setting the Line-Height to: %i (%i)\n",tempH, heightBR);
						#endif
						// Check if this is going to overflow onto another page
						if(tempH < (fontSize+10)){
							lastNode = insertNewPage3(lastNode, txt);
							heightBR = pageHeight-(pageMargin+fontSize);
							tempH = heightBR - highest;
						}
						// Recursively set all other items on this line at the new height.
						setParentHeight2(lastBreak, txt, tempH, true);
						heightBR = tempH;
					}else{
						heightBR -= fontSize;
						lastBreak = insertLineBreak(lastNode,txt,lengBR,heightBR);
					}
					if(heightBR < (fontSize+botMargin)){
						// Create New Page
						heightBR = pageHeight-(pageMargin+fontSize);
						lastNode = insertNewPage3(lastNode, txt);
					}
					lengBR = leftMargin + indentModifier;
					#ifdef VERBOSITY
					printf("  +--> Inserted Line Break \n");
					#endif
				}
				//-----------------------------------------------------------------------------
				txt->y = heightBR;
				txt->x = lengBR;
				int y = *c;
				lengBR += charWidth(y);
			}else{*/
			skip = true; //}
			*c++;
		}else if(*c == ' ' && insideB==false){
			numfunc = 7;
			// add a space
			long lastinput = lastNode->tagType;
			if(lastinput != 32){
				txt->isKey = true;
				txt->tagType = 32;
				//-----------------------------------------------------------------------------
				// Massive Margin-Checking Code
				//-----------------------------------------------------------------------------
				if(pagewidth <= lengBR + charWidth(32)){
					// Insert BR Node
					// Line Height and Line Wrapping Done Here.
					// Do a final line wrap at the end of the Document (incase there are no BRs)
					#ifdef VERBOSITY
					printf(" --> Page Width Overflow3\n");
					printf("  +-> Adjusting Prev Line Height\n");
					#endif
					// Find Largest Height
#ifdef FALLBACK
					long highest = findHighest(root, txt, 0);
#else
					long highest = findHighest2(lastBreak, txt, 0);
#endif
					if(highest != fontSize){  // There are pictures in the line
						long tempH = heightBR - highest;									
						#ifdef VERBOSITY
						printf("   +-> Setting the Line-Height to: %i (%i)\n",tempH, heightBR);
						#endif
						// Check if this is going to overflow onto another page
						if(tempH < (fontSize+10)){//+botMargin)){
							lastNode = insertNewPage3(lastNode, txt);
							heightBR = pageHeight-(pageMargin+fontSize);
							tempH = heightBR - highest;
						}
						// Recursively set all other items on this line at the new height.
						setParentHeight2(lastBreak, txt, tempH, true);
						heightBR = tempH;
					}else{
						heightBR -= fontSize;
						lastBreak = insertLineBreak(lastNode,txt,lengBR,heightBR);
					}
					if(heightBR < (fontSize+botMargin)){
						// Create New Page
						heightBR = pageHeight-(pageMargin+fontSize);
						lastNode = insertNewPage3(lastNode, txt);
					}
					lengBR = leftMargin + indentModifier;
					#ifdef VERBOSITY
					printf("  +--> Inserted Line Break \n");
					#endif
				}
				//-----------------------------------------------------------------------------
				txt->y = heightBR;
				txt->x = lengBR;
				int y = *c;
				lengBR += charWidth(y);
			}else{skip = true;}
			*c++;
		}else if(*c == ' ' && insideB==true){
			numfunc = 8;
			*c++;
			skip = true;
		}else if(*c == '&'){
			struct charpoint qw;
			//char *tempstring;
			long wordlength=0;
			*c++;
			qw = getEntity(c);
			txt->entity = qw.entity;
			c = qw.address;
			txt->isKey = true;
			txt->tagType = 84;

			/*tempstring = (char*)malloc(sizeof(char)*1+2);
			strncpy(tempstring,txt->entity,1);
			tempstring[1] = '\0';*/
			wordlength = wordWidth(&txt->entity, 1, isEM, pdf);
			//free(tempstring);

			//-----------------------------------------------------------------------------
			// Massive Margin-Checking Code
			//-----------------------------------------------------------------------------
			if(pagewidth <= lengBR + wordlength){
				// Insert BR Node
				// Line Height and Line Wrapping Done Here.
				// Do a final line wrap at the end of the Document (incase there are no BRs)
				#ifdef VERBOSITY
				printf(" --> Page Width Overflow %i\n", __LINE__);
				printf("  +-> Adjusting Prev Line Height\n");
				#endif
				// Find Largest Height
	#ifdef FALLBACK
				long highest = findHighest(root, txt, 0);
	#else
				long highest = findHighest2(lastBreak, txt, 0);
	#endif
				if(highest != fontSize){  // There are pictures in the line
					long tempH = heightBR - highest;						
					#ifdef VERBOSITY
					printf("   +-> Setting the Line-Height to: %i (%i)\n",tempH, heightBR);
					#endif
					// Recursively set all other items on this line at the new height.
					setParentHeight2(lastBreak, txt, tempH, true);
					heightBR = tempH - fontSize;
				}else{
					heightBR -= fontSize;
				}
				lastBreak = insertLineBreak(lastNode,txt,lengBR,heightBR);
				if(heightBR < (2*fontSize) + botMargin){
					lastNode = insertNewPage3(lastNode, txt);
					heightBR = pageHeight-(pageMargin+fontSize);
				}
				lengBR = leftMargin + indentModifier;
				#ifdef VERBOSITY
				printf("  +--> Inserted Line Break \n");
				#endif
			}
			//-----------------------------------------------------------------------------
			txt->y = heightBR;
			txt->x = lengBR;
			txt->h = fontSize;
			lengBR += wordlength;
		}else{
			numfunc = 9;
			// Todo
			c++;
			skip = true;
		}
		if(skip != true && *c != '\0'){
			struct node *txt2 = initnode();
			// Save Old Node First
			lastNode = txt;
			txt->next = txt2;
			txt2->next = '\0';
			txt = txt2;
		}
		skip = false;
	}
	#ifdef VERBOSITY
		displayNodes(root);
	#endif
	return root;

}

void drawheader(HPDF_Page page, HPDF_Doc pdf, HPDF_Font font){ //, char *logopath){
	HPDF_Rect rect;
	HPDF_Font font2;
	X_Y j;
	char *logo;
	int sizeofpath=0;
	
	//Outline Rectangle
	rect.left = 25;
    rect.top = (HPDF_REAL)pageHeight-25;
    rect.right = (HPDF_REAL)pagewidth+25;
    rect.bottom = (HPDF_REAL)botMargin;

	HPDF_Page_EndText (page);
    HPDF_Page_Rectangle (page, rect.left, rect.bottom, rect.right - rect.left,
                rect.top - rect.bottom);
    HPDF_Page_Stroke(page);
	
	HPDF_Page_GSave (page);
    HPDF_Page_BeginText (page);
    HPDF_Page_MoveTextPos (page, 270, pageHeight-15);
    HPDF_Page_SetTextLeading (page, 12);
	char *whitetext = (char*)malloc(sizeof(char)*(104));
	sprintf(whitetext,"PAGE %i of %i\0", currCount, pageCount);
	HPDF_Page_TextRect(page, 270, pageHeight-15, 560, pageHeight-35, whitetext, HPDF_TALIGN_RIGHT, NULL);
	free(whitetext);
    HPDF_Page_EndText (page);
    HPDF_Page_GRestore (page);

	HPDF_Page_BeginText (page);
}

HPDF_Page renderNode(struct node *ptr, HPDF_Page page, HPDF_Doc pdf, HPDF_Font font) //, char *logopath)
{
	/*
		TODO:
			- Bold and Italics
			- Underlined
			- Strikeout - Possibly Un-Supportable for Now
			- Horizontal Line???
			- Page Break
	*/
	if(ptr->isKey == false && ptr->tagType != -1){ // && ptr->c != '0'){
		char *p;
		p = (char*)malloc(sizeof(char)*ptr->clen+2);
		strncpy(p,ptr->d,ptr->clen);
		p[ptr->clen] = '\0';
		HPDF_Page_TextOut(page,ptr->x,ptr->y,p);
		free(p);
	}else if(ptr->isKey == true && ptr->tagType == 84){ // && ptr->c != '0'){
		char *p;
		p = (char*)malloc(sizeof(char)*3);
		strncpy(p,&ptr->entity,1);
		p[1] = '\0';
		HPDF_Page_TextOut(page,ptr->x,ptr->y,p);
		free(p);
	}else if(ptr->isKey == true && ptr->tagType == 32){
		char *p = " ";
		HPDF_Page_TextOut(page,ptr->x,ptr->y,p);
	}else if(ptr->isKey == true && ptr->tagType == 4){
		//Render Image.
		draw_image(pdf,imgName(imgroot,ptr->imgnum),ptr->x,ptr->y,ptr->h,ptr->w);
	}else if(ptr->isKey == true && ptr->tagType == 11){
		//Line Break Ignore
	}else if(ptr->tagType == -1){
		//Start of Document/Page
		
		drawheader(page,pdf, font); //, logopath);
	}else if(ptr->isKey == true && ptr->tagType == 1){
		//Begin Bold Text
		font = HPDF_GetFont (pdf, "Helvetica-Bold", NULL); //Helvetica-BoldOblique
		HPDF_Page_SetFontAndSize (page, font, fontSize);
	}else if(ptr->isKey == true && ptr->tagType == 101){
		//End Bold Text
		font = HPDF_GetFont (pdf, "Helvetica", NULL);
		HPDF_Page_SetFontAndSize (page, font, fontSize);
	}else if(ptr->isKey == true && ptr->tagType == 10){
		//Begin Italic Text
		font = HPDF_GetFont (pdf, "Helvetica-Oblique", NULL);
		HPDF_Page_SetFontAndSize (page, font, fontSize);
	}else if(ptr->isKey == true && ptr->tagType == 110){
		//End Italic Text
		font = HPDF_GetFont (pdf, "Helvetica", NULL);
		HPDF_Page_SetFontAndSize (page, font, fontSize);
	}else if(ptr->isKey == true && ptr->tagType == 2){
		//Begin LI (Bullet)
		HPDF_Font bulletfont;
		bulletfont = HPDF_GetFont (pdf, "Symbol", NULL);
		HPDF_Page_SetFontAndSize (page, bulletfont, fontSize);
		HPDF_Page_TextOut(page,ptr->x,ptr->y,"·");
		HPDF_Page_SetFontAndSize (page, font, fontSize);
	}else if(ptr->isKey == true && ptr->tagType == 102){
		//End LI
		;
	}else if(ptr->isKey == true && ptr->tagType == 200){
		//New Page
		page = initPage(pdf);
		initScreen(0,0,1,1);
		setTextTop(page,font);
		drawheader(page,pdf, font); //,logopath);
	}else{
		// Unsupported Keywords
		#ifdef VERBOSITY
		printf("[Unsupported Keyword, %i]\n",ptr->tagType);
		#endif
	}
	return page;
}

void render(HPDF_Page page, HPDF_Doc pdf, HPDF_Font font, struct node *ptr){ //, char *logopath){
	HPDF_Page page2;
	page2 = page;
	while(ptr){
		page2 = renderNode(ptr, page2, pdf, font); //, logopath);
		ptr = ptr->next;
    }
}

// Frees the Linked Lists
void DestroyLink(struct node *root){
	struct node *temp = root;
	while(temp->next){
		root = temp->next;
		free(temp);
		temp=root;
	}
	return;
}

int _tmain(int argc, _TCHAR* argv[])
{
	LPSTR filename;
//	LPSTR filecontents;
#ifdef SPEEDCHK
	LARGE_INTEGER m_qwStart, m_qwStop, m_qwFreq;
	LARGE_INTEGER t_qwStart, t_qwStop, t_qwFreq;
	LARGE_INTEGER p_qwStart, p_qwStop, p_qwFreq;
	float fx;
#endif
//	RET_TEXT txtFile;
	HANDLE hFile = NULL;
	DWORD dwSize;
//	BOOL bSuccess;
    char ReadBuffer[BUFFER_SIZE] = {0};
	RET_TEXT rtFile;
	char fname[MAX_PATH+3];
	int status = -1;
	TidyBuffer errbuf = {0};
	
	// TidyLib
	TidyDoc tdoc = tidyCreate();

	HPDF_Doc pdf;
	HPDF_Font font;
	HPDF_Page page;
	struct node *root;
    

	if (argc<1) {
        //_ftprintf(stderr, CxImage::GetVersion());
        printf("HTML->PDF\n");
		printf("Usage: %s <htmlfile>\n", argv[0]);
        return 1;
    }
	filename = (LPSTR)LocalAlloc(LPTR, MAX_PATH);
	jpegpath = (char*)malloc(sizeof(char)*MAX_PATH);
	sprintf(filename,"%s",argv[1]);
	GetCurrentDirectory(MAX_PATH, jpegpath);
#ifdef VERBOSITY
	printf("Opening/Tidying %s\n",filename);
#endif
	dwSize = 0;
	pageCount = 1;

	//---------------------
	// Tidy the HTML First
	//---------------------
//----------------------------
#ifdef SPEEDCHK
	QueryPerformanceFrequency(&t_qwFreq);
	QueryPerformanceCounter(&t_qwStart);
#endif
//----------------------------
	tidyOptSetBool( tdoc, TidyUpperCaseTags, yes );
	tidyOptSetBool( tdoc, TidyQuiet, yes );
	tidyOptSetInt( tdoc, TidyWrapLen, 0 );
	status = tidySetErrorBuffer( tdoc, &errbuf );
	status = tidyParseFile( tdoc, filename );
	if ( status >= 0 )
            status = tidyCleanAndRepair( tdoc );
	if ( status >= 0 )
            status = tidyRunDiagnostics( tdoc );
	if ( status > 1 )
		status = ( tidyOptSetBool(tdoc, TidyForceOutput, yes) ? status : -1 );
	if ( status >= 0 )
		status = tidySaveFile( tdoc, filename );
	tidyRelease(tdoc);
	#ifdef VERBOSITY
		printf("Tidy Diagnostics:\n\n%s",errbuf.bp);
	#endif
//----------------------------
#ifdef SPEEDCHK
	QueryPerformanceCounter(&t_qwStop);
    fx = (float)((t_qwStop.QuadPart - t_qwStart.QuadPart) / (float)t_qwFreq.QuadPart);
    printf("Tidy Seconds: %f\n",fx);
#endif
//----------------------------
	//---------------------

	dwSize = 0;
	pageCount = 1;	
	rtFile.filecontents = LoadFile(filename, &rtFile.filesize);	
	if(rtFile.filesize == 0)
    {
        printf("Main(): Could not read from file (error %d)\n", GetLastError());
        CloseHandle(hFile);
        return 1;
    }
	LocalFree(filename);
#ifdef VERBOSITY
	printf("Size: %i\n", rtFile.filesize);
#endif
	
	//-------------------------------------------------------------
	// PDF Initialization
	//-------------------------------------------------------------
	sprintf(fname,"%s.pdf",argv[1]);	
	pdf = initPDF(fname);
	if(pdf == 0){
		_tprintf(TEXT("Main(): Cannot create PDF Document Object\n"));
		return 1;
	}

	font = initFont(pdf);
	page = initPage(pdf);
	initScreen(0, 0, 1, 1);
	setY(setTextTop(page,font));
	pageHeight = (long)HPDF_Page_GetHeight(page);
	//-------------------------------------------------------------
	
	// Set the metrics of the font for preprocessing
	setPDFFontMetrics(font);

	// Initialize the Prime Root Node
	root = initnode();

#ifdef VERBOSITY	
	printf("Processing...\n");
#endif

//----------------------------
#ifdef SPEEDCHK
	QueryPerformanceFrequency(&m_qwFreq);
	QueryPerformanceCounter(&m_qwStart);
#endif
//----------------------------

	root = preprocess(rtFile.filecontents, pdf);

//----------------------------
#ifdef SPEEDCHK
	QueryPerformanceCounter(&m_qwStop);
    fx = (float)((m_qwStop.QuadPart - m_qwStart.QuadPart) / (float)m_qwFreq.QuadPart);
    printf("Processing Seconds: %f\n",fx);
#endif
//----------------------------

#ifdef VERBOSITY
	printf("Rendering...\n");
#endif
	render(page,pdf,font,root); //,logopath);
	savePDF(page, pdf, fname);

	// Garbage Collection
	DestroyLink(root);

	//GlobalFree(rtFile.filecontents);
	if(hFile != NULL) CloseHandle(hFile);
#ifdef VERBOSITY
	printf("PDF Outputed to: %s\n", fname);
#endif
	printf("Success\n");
	return 0;
}
