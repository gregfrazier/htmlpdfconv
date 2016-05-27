#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "ximage.h"

/*
	This file uses CxImage Library to convert gif,bmp,tif,tga,pcx,png,etc... into JPEG for PDF embedding.
*/

char* FindExtension(const char *name)
{
	int len = _tcslen(name);
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return (char*)(name+i+1);
		}
	}
	return (char*)(name+len);
}

int check4jpg(char *filename){
	CxImage image;
	char *newfilename;
	char* extin = FindExtension(filename);
	//printf("Extension: %s\n",extin);
	_tcslwr(extin);
	int typein = CxImage::GetTypeIdFromName(extin);
	//printf("%i | %i\n", typein, CXIMAGE_SUPPORT_JPG);
	if(typein == 3){
		// Do nothing!
		//printf("Doing nothing\n");
		return 2;
	}else if (typein == CXIMAGE_FORMAT_UNKNOWN) {
		printf("\nCxImage Error: Unknown filetype for %s\n", filename);
		return 1;
	}
	return 0;
}

int convert2jpg(char *filename){
	CxImage image;
	char *newfilename;
	char* extin = FindExtension(filename);

	_tcslwr(extin);
	int typein = CxImage::GetTypeIdFromName(extin);
	if(typein == 3){
		// Do nothing!
		//printf("Whazah!\n");
		return 2;
	}else if (typein == CXIMAGE_FORMAT_UNKNOWN) {
		printf("\nCxImage Error: Unknown filetype for %s\n", filename);
		return 1;
	}

	newfilename = (char *)malloc(sizeof(char)*strlen(filename)*2);
	sprintf(newfilename, "%s.jpg",filename);

	image.Load(filename,typein);
	if (image.IsValid()){
		if(!image.IsGrayScale()) image.IncreaseBpp(24);
		image.SetJpegQuality(80);
		image.Save(newfilename,3);
		free(newfilename);
		return 0;
	}else{
		free(newfilename);
		return 1;
	}
	free(newfilename);
	return 1;
}

X_Y openJpeg(char* filename){
	FILE *hFile;
	long size;
	int blah;
	unsigned char *data;
	char *filejpg;
//unsigned short x, y;
	size_t result;
	X_Y re;

	re.x = 0;
	re.y = 0;
	re.toJPG = false;

	//printf("Image Filename: %s\n\n",filename);
	blah = check4jpg(filename);
	if(blah == 2){
		hFile = fopen(filename,"rb");
		printf("%s\n",filename);
		//fopen_s(hFile,filename,"rb");
		if (hFile==NULL){
			printf("Can not find\n");
			return re;
		}else{
			fseek(hFile, 0, SEEK_END);
			size = ftell(hFile);}
		if(size > 0){
			//Read the file into a character array
			fseek(hFile,0,SEEK_SET);
			data = (unsigned char*)malloc(sizeof(char)*size);
			result = fread (data,1,size,hFile);
			if(result != size){
				fclose(hFile);
				return re;
			}
			fclose(hFile);
			re = get_jpeg_size(data,size);
			printf("Size of JPEG: W:%i, H:%i\n\n", re.x,re.y);
		}else{
			return re;}
	}else{
		// Need to Convert it...
		blah = 0;
		//printf("Converting to JPG\n");
		blah = convert2jpg(filename);
		if(blah != 0){ return re; }
		filejpg = (char*)malloc(sizeof(char)*(strlen(filename)*2));
		sprintf(filejpg, "%s.jpg",filename);
		//printf("\n\n%s\n",filejpg);
		hFile = fopen(filejpg,"rb");
		free(filejpg);
		//fopen_s(hFile,filename,"rb");
		if (hFile==NULL){
			return re;
		}else{
			fseek(hFile, 0, SEEK_END);
			size = ftell(hFile);}
		if(size > 0){
			//Read the file into a character array
			fseek(hFile,0,SEEK_SET);
			data = (unsigned char*)malloc(sizeof(char)*size);
			result = fread (data,1,size,hFile);
			if(result != size){
				fclose(hFile);
				return re;
			}
			fclose(hFile);
			re = get_jpeg_size(data,size);
			re.toJPG = true;
			//printf("Size of JPEG: W:%i, H:%i\n\n", re.x,re.y);
		}else{
			return re;}
	}
	return re;
}

// Code from http://www.64lines.com/
X_Y get_jpeg_size(unsigned char* data, unsigned int data_size) {
   //Check for valid JPEG image
	X_Y ry;
	ry.x = 0;
	ry.y = 0;
   int i=0;   // Keeps track of the position within the file
   if(data[i] == 0xFF && data[i+1] == 0xD8 && data[i+2] == 0xFF && data[i+3] == 0xE0) {
      i += 4;
      // Check for valid JPEG header (null terminated JFIF)
      if(data[i+2] == 'J' && data[i+3] == 'F' && data[i+4] == 'I' && data[i+5] == 'F' && data[i+6] == 0x00) {
         //Retrieve the block length of the first block since the first block will not contain the size of file
         unsigned short block_length = data[i] * 256 + data[i+1];
         while((unsigned int)i<data_size) {
            i+=block_length;               //Increase the file index to get to the next block
            if((unsigned int)i >= data_size) return ry;   //Check to protect against segmentation faults
            if(data[i] != 0xFF) return ry;   //Check that we are truly at the start of another block
            if(data[i+1] == 0xC0) {            //0xFFC0 is the "Start of frame" marker which contains the file size
               //The structure of the 0xFFC0 block is quite simple [0xFFC0][ushort length][uchar precision][ushort x][ushort y]
               ry.y = data[i+5]*256 + data[i+6];
			   ry.x = data[i+7]*256 + data[i+8];
               return ry;
            }
            else
            {
               i+=2;                              //Skip the block marker
               block_length = data[i] * 256 + data[i+1];   //Go to the next block
            }
         }
         return ry;                     //If this point is reached then no size was found
      }else{ return ry; }                  //Not a valid JFIF string
         
   }else{ return ry; }                     //Not a valid SOI header
}