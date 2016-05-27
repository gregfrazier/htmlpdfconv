#include <stdio.h>
#include "hpdf.h"
#include "main.h"

int currCount;

HPDF_Doc initPDF(char *filename){
	HPDF_Doc  pdf;

    fontSize = 9;
	currCount = 0;
	pdf = HPDF_New(error_handler, NULL);
    if (!pdf) {
        //_tprintf(TEXT("Main(): Cannot create PdfDoc object\n"));
        return (HPDF_Doc) 0;
    }
	return pdf;
}

HPDF_Font initFont(HPDF_Doc pdf){
	HPDF_Font font;

	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
    font = HPDF_GetFont(pdf, "Helvetica", NULL);
	return font;
}

HPDF_Page initPage(HPDF_Doc pdf){
	HPDF_Page page;

    page = HPDF_AddPage(pdf);
	currCount++;
	HPDF_Page_SetSize(page,HPDF_PAGE_SIZE_LETTER,HPDF_PAGE_PORTRAIT);
	return page;
}

float setTextTop(HPDF_Page page, HPDF_Font font){
	float pageHeight = (int)HPDF_Page_GetHeight(page);
	//printf("%f\n",pageHeight);
	HPDF_Page_BeginText (page);
	HPDF_Page_SetTextLeading (page, fontSize);
    HPDF_Page_MoveTextPos (page, 0, pageHeight);
    HPDF_Page_SetFontAndSize(page, font, fontSize);
	return pageHeight;
}

void savePDF(HPDF_Page page, HPDF_Doc pdf, char *fname){
	HPDF_Page_EndText (page);
    HPDF_SaveToFile(pdf, fname);
    HPDF_Free(pdf);
}