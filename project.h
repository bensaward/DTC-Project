#ifndef _PROJECT_H
#define _PROJECT_H


typedef struct // use chars to save memory?
{
	int red;
	int green;
	int blue;
	// include? int alpha;
} pixel;

union Endian //our png image is big endian, perhaps our compiler is little endian
	{
		char asstring[4];
		int asnumber;
	} endian;

//BEGIN FUNCTION DEFS

void getdimensions(FILE *image, int *retvalue); // lets find the IDHR of our png file and read it to get dimensions
void findwell(/* args */); // use circular edge detection to find the well plates
void greyscaleimage(pixel **image); /* for this we need our image in grey scale */
void swaplocations(char *array); // swap our big endian number to a little endian number
//END FUNCTION DEFS

void getdimensions(FILE *image, int *retvalue) //return x, y dimensions if this matches what we expect, NULL otherwise
{
	char *buffer=malloc(sizeof(char)*101); //how many bytes usually before IHDR? +13 bytes for IHDR +4 for "IHDR"
	strcpy(buffer, "00000000000000000000000000");
	char *IHDR="IHDR";
	int bytes_read=0;
	while((strstr(buffer, IHDR) == NULL))
	{
		fgets(buffer, 100, image);
		while (buffer[0]=='\0')
		{
			buffer=buffer+sizeof(char); //our buffer might have a line begining with HEX 00 ie \0
		}
	}
    
	//lets make a string with our header in :D
	int skippedchars=0;
	char *tempheader=strstr(buffer, IHDR);
	tempheader+=4; //move over IHDR into the buffer
	int i=0;
	for (i=0; i<4; i++)
	{
		endian.asstring[i]=tempheader[0]; //read our big endian png dimensions
		tempheader+=sizeof(char);
	}
 	swaplocations(endian.asstring); // turn it into little endian
 	int x,y;
 	x=endian.asnumber; // assign our return value
	for (i=0; i<4; i++)
	{
		endian.asstring[i]=tempheader[0];
		tempheader+=sizeof(char);
	}
	swaplocations(endian.asstring);
	y=endian.asnumber;
    	retvalue[0]=x;
    	retvalue[1]=y;
}

void swaplocations(char *array)
{
	char temp;
	temp=array[0];
	array[0]=array[3];
	array[3]=temp;
	temp=array[1];
	array[1]=array[2];
	array[2]=temp;
}

#endif
