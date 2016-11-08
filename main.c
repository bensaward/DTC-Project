#include <stdio.h>
#include <stdlib.h>
#include <project.h>

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
		fprintf("Could not open image.\n");
		return -1;
	}
	// READ FILE UNTIL WE FIND IDAT
	return 0;
}
