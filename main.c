#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include </home/ben/Documents/Oxford/DTP/project/DTC-Project/project.h>

// BEGIN CONSTS
const char *INPUTIMAGE="image-1.png";
const char *OUTFILE="output.txt";


// END CONSTS


// MAIN
int main (int argc, char **argv)
{
	FILE *image = fopen(INPUTIMAGE, "r");
	if (image == NULL)
	{
		printf("Could not open image.\n");
		return -1;
	}
	int dimensions[2];
	getdimensions(image, dimensions);
	printf("x = %d, y = %d", dimensions[0], dimensions[1]);
	// READ FILE UNTIL WE FIND IDAT
	return 0;
}
