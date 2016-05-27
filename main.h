#include "hpdf.h"

//#pragma warning(4244:disable)

#ifndef VERBOSITY
#define VERBOSITY
#endif

#ifndef SPEEDCHK
#define SPEEDCHK
#endif

// Uses slower but more bug-free methods
#ifndef FALLBACK
#define FALLBACK
#endif

//#define FLOAT_TO_INT(in,out) __asm__ __volatile__ ("fistpl %0" : "=m" (out) : "t" (in) : "st") ;

typedef struct SCREEN
{
	bool bold;       // ****
	bool underline;  //    *
	bool italic;     //    * Set these to false on init
	bool hyperlink;  //    *
	bool anchor;     // ****
	float x;		 // This is the current X position
	float y;		 // This is the current Y position
	int posLine;	 // This is the position in the current line (0-100 Characters on the Screen) (X linepos)
	long posPage;    // This is the current line on the page out (Y linepos)
	char href[4096]; // This is the address of the Hyperlink
	long numPage;    // Page Number (set to 1)
	char color[6];   // The HEX value of the text color
	char* line;		 // Contents of the Line
	char imgstack[500][256]; //Stack of IMAGES to display
} SCREEN, *PSCREEN;

typedef struct X_Y
{
	long x;
	long y;
	bool toJPG;
} X_Y, *X_Y_PTR;

typedef struct imglist
{
	char filename[4096];
	struct imglist *next;
} IMGLIST;

typedef struct imgret
{
	int type;
	char value[4096];
} IMGRET;

typedef struct charpoint
{
	char entity;
	char *address;
} CHARPOINT;

// Keywords
bool isKeyword(char *wrod);
bool isBR(char *wrod);
bool isImg(char *wrod);
bool isBold(char *wrod);
bool isItalic(char *wrod);
bool isUL(char *wrod);
bool isLI(char *wrod);

// Attribute
bool isAttribute(char *wrod);
bool isSrc(char *wrod);
struct imgret getValue(char *h, bool getval, char *jpegpath);

// Screen Functions
void initScreen(float x, float y, int line, long page);
void setLine(long c);
void setLinePos(int c);
void setAnchor(bool c);
void setBold(bool c);
void setItalic(bool c);
void setUnderline(bool c);
void setX(float x);
void setY(float y);
bool getItalic();
bool getUnderline();
bool getAnchor();
bool getBold();
float getY();
float getX();
long getPage();
long getLine();
int getLinePos();
void setHref(char *c);

// Main Functions
HPDF_Doc initPDF(char *filename);
HPDF_Font initFont(HPDF_Doc pdf);
HPDF_Page initPage(HPDF_Doc pdf);
float setTextTop(HPDF_Page page, HPDF_Font font);
void savePDF(HPDF_Page page, HPDF_Doc pdf, char *fname);
void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data);
long charWidth(char t);

// Image Handling Functions
X_Y get_jpeg_size(unsigned char* data, unsigned int data_size);
X_Y openJpeg(char* filename);

//Image List Functions
struct imglist *initimg();
char *imgName(struct imglist *ptr, int x);
char* FindExtension(const char *name);

//Entity
bool isDelim(char c);
bool isADelim(char c);
struct charpoint getEntity(char *c);

//hyperlinks
bool downURL(char *NDomain, char *NAddress, char *Filename);

extern const char *keywords[];
extern const char *attributes[];
extern SCREEN screen;
extern long fontSize;
extern int pageCount;
extern int currCount;