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
    unsigned char *IDATCHUNK=malloc(sizeof(char)*((firstidatlength))); // dynamically work with our memory
    unsigned char *inflatechunk=malloc(sizeof(char)*((firstidatlength)));
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
    // inflate zlib compressed data: stolen from http://www.zlib.net/zlib_how.html
    //int success=inf(IDATCHUNK, inflatechunk, firstidatlength);
    int success=inflate_mod(IDATCHUNK, firstidatlength, inflatechunk, firstidatlength);
    pixel pixelarray[dimensions[1]][dimensions[2]];
    int bytesread=0; // populate our pixel array with the first IDAT chunk
    y=0;
    do
    {
            for (x=0; x<(dimensions[0]*3); x++)
            {
                switch (bytesread%3)
                {
                case 0: //red
                    {
                        pixelarray[y][x/3].red=inflatechunk[bytesread];
                        bytesread+=1;
                        break;
                    }
                case 1: //green
                    {
                        pixelarray[y][x/3].green=inflatechunk[bytesread];
                        bytesread+=1;
                        break;
                    }
                case 2: //blue
                    {
                        pixelarray[y][x/3].blue=inflatechunk[bytesread];
                        bytesread+=1;
                        break;
                    }
                }
                if (bytesread>=firstidatlength) {break;}
            }
           if(bytesread<firstidatlength) {y+=1;}
    }
    while (bytesread<firstidatlength);
    printf("first IDAT stored\n");
    // grab the rest of the IDAT chunks but skip over the 4 byte crc. Grab the length and compare it against 0x8000
    char discarded;
    while (1) //can we be more specific here?
    {
        for (i=0; i<4; i++) //skip over crc
        {
            discarded=fgetc(image);
        }
        for (i=0; i<4; i++) //grab the 4 length bytes
        {
            endian.asstring[i]=fgetc(image);
        }
        swaplocations(endian.asstring); //length to little endian format
        if (endian.asnumber==firstidatlength)//is the next IDAT 0x8000 long? ie the maximum?
        {
            bytesread=0;
            fgets(IDATCHUNK, firstidatlength, image); // get the next IDAT chunk
            success=inflate_mod(IDATCHUNK, firstidatlength, inflatechunk, firstidatlength);
            while (bytesread<firstidatlength) // populate the next part of the array
            {
                while (x<(dimensions[0]*3)) // our x and y is saved from last time
                {
                     switch (bytesread%3)
                    {
                        case 0: //red
                        {
                            pixelarray[y][x/3].red=inflatechunk[bytesread];
                            bytesread+=1;
                            x+=1;
                            break;
                        }
                        case 1: //green
                        {
                            pixelarray[y][x/3].green=inflatechunk[bytesread];
                            bytesread+=1;
                            x+=1;
                            break;
                        }
                        case 2: //blue
                        {
                            pixelarray[y][x/3].blue=inflatechunk[bytesread];
                            bytesread+=1;
                            x+=1;
                            break;
                        }
                    }
                    if (bytesread>=firstidatlength) {break;}
                }
                if(bytesread<firstidatlength) {y+=1;}
            }
            printf("another IDAT processed\n");
        }
        else // if the IDAT is smaller
        {
            free(IDATCHUNK); //garbage collection
            free(inflatechunk);
            unsigned char *lastidat=malloc(sizeof(char)*endian.asnumber);
            unsigned char *inflatechunk=malloc(sizeof(char)*endian.asnumber);
            bytesread=0;
            fgets(lastidat, endian.asnumber, image); // get the next IDAT chunk
            success=inflate_mod(IDATCHUNK, firstidatlength, inflatechunk, firstidatlength);
            while (bytesread<firstidatlength) // populate the next part of the array
            {
                while (x<(dimensions[0]*3)) // our x and y is saved from last time
                {
                     switch (bytesread%3)
                    {
                        case 0: //red
                        {
                            pixelarray[y][x/3].red=inflatechunk[bytesread];
                            bytesread+=1;
                            x+=1;
                            break;
                        }
                        case 1: //green
                        {
                            pixelarray[y][x/3].green=inflatechunk[bytesread];
                            bytesread+=1;
                            x+=1;
                            break;
                        }
                        case 2: //blue
                        {
                            pixelarray[y][x/3].blue=inflatechunk[bytesread];
                            bytesread+=1;
                            x+=1;
                            break;
                        }
                    }
                    if (bytesread>=firstidatlength) {break;}
                }
                if(bytesread<firstidatlength) {y+=1;}
            }
            free(lastidat);
            free(inflatechunk); //done garbage collection
            break;
        }

    }
	return 0;
}
