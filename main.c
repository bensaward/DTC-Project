#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <assert.h>
//#include </auto/dtchome/sawardb/coding/project/project.h>
#include </home/ben/Documents/Oxford/DTP/project/DTC-Project/project.h>


// BEGIN CONSTS
const char *INPUTIMAGE="image-1.png";
const char *OUTFILE="output.txt";


// END CONSTS


// MAIN
int main (int argc, char **argv)
{
    int i,x,y,z;
    char *firstidatoverrun=malloc(100);
	FILE *image = fopen(INPUTIMAGE, "rb");
	if (image == NULL)
	{
		printf("Could not open image.\n");
		return -1;
	}
	int dimensions[4];
	int firstidatlength, array_len;
	getheader(image, dimensions);
	printf("x = %d, y = %d, depth = %d, mode = %d\n", dimensions[0], dimensions[1], dimensions[2], dimensions[3]);
	// READ FILE UNTIL WE FIND IDAT
    idatread(image, firstidatoverrun, &firstidatlength, &array_len);
    printf ("IDATlen = %d\n", firstidatlength);
    printf ("array_len = %d\n", array_len);
    char *IDATCHUNK=malloc(sizeof(char)*((firstidatlength)+1)); // dynamically work with our memory
    char *temp=malloc(sizeof(char)*(firstidatlength-array_len));
    //strcpy(IDATCHUNK, firstidatoverrun);
    for (i=0; i<array_len; i++) //do this instead of strcpy so we copy any null bytes
    {
        IDATCHUNK[i]=firstidatoverrun[i];
    }
    //printf("IDATCHUNK = %s", IDATCHUNK);
    fgets(temp, firstidatlength-array_len-4, image);//read the rest of IDAT chunk leaving off the crc at the end
    //strcpy(IDATCHUNK, temp);
    for (i=0; i<firstidatlength; i++) //do this instead of strcpy so we copy any null bytes
    {
        IDATCHUNK[i+array_len]=temp[i];
    }
    free(temp);
    //printf("IDATCHUNK = %s", IDATCHUNK);
    /* inflate zlib compressed data: stolen from http://www.zlib.net/zlib_how.html */

	return 0;
}
