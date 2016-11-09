#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include </auto/dtchome/sawardb/coding/project/project.h>
#include </home/ben/Documents/Oxford/DTP/project/DTC-Project/project.h>

// BEGIN CONSTS
const char *INPUTIMAGE="image-1.png";
const char *OUTFILE="output.txt";


// END CONSTS


// MAIN
int main (int argc, char **argv)
{
    char *array=malloc(100);
	FILE *image = fopen(INPUTIMAGE, "rb");
	if (image == NULL)
	{
		printf("Could not open image.\n");
		return -1;
	}
	int dimensions[4];
	getheader(image, dimensions);
	printf("x = %d, y = %d, depth = %d, mode = %d\n", dimensions[0], dimensions[1], dimensions[2], dimensions[3]);
	// READ FILE UNTIL WE FIND IDAT
    idatread(image, array);
	return 0;
}
