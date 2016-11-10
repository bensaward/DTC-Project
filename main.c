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
	int bytesread=0;
	int dimensions[4];
	int firstidatlength, array_len;
	getheader(image, dimensions);
	printf("x = %d, y = %d, depth = %d, mode = %d\n", dimensions[0], dimensions[1], dimensions[2], dimensions[3]);
	// READ FILE UNTIL WE FIND IDAT
    idatread(image, firstidatoverrun, &firstidatlength, &array_len);
    printf ("IDATlen = %d\n", firstidatlength);
    printf ("array_len = %d\nIDATOVERRUN =", array_len);
   for (z=0; z<array_len; z++)
   {
        printf("0x%X\n", firstidatoverrun[z]);
   }

    printf("\n");
    unsigned char *IDATCHUNK=malloc(sizeof(char)*((firstidatlength))); // dynamically work with our memory
    unsigned char *inflatechunk=malloc(sizeof(char)*((firstidatlength)));
    unsigned char *temp=malloc(sizeof(char)*(firstidatlength-array_len));
    for (i=0; i<array_len; i++) //do this instead of strcpy so we copy any null bytes
    {
        IDATCHUNK[i]=firstidatoverrun[i];
    }


    //read the rest of IDAT chunk leaving off the crc at the end
    fread(temp, 1, (firstidatlength-array_len), image);
    printf("first byte of IDAT = 0x%X\n", temp[0]);
    for (i=0; i<(firstidatlength-array_len); i++)
    {
        IDATCHUNK[i+array_len]=temp[i];
    }
    printf("i = %d\n", i);
    free(temp);
    // inflate zlib compressed data: stolen from http://www.zlib.net/zlib_how.html

    int success=inflate_mod(IDATCHUNK, firstidatlength, inflatechunk, firstidatlength);
    pixel pixelarray[dimensions[1]][dimensions[2]];
    bytesread=0; // populate our pixel array with the first IDAT chunk
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
            printf("discarded = %c (0x%X)\n", discarded, discarded);
        }
        //printf("discarded the 4 crc bytes\n");
        for (i=0; i<4; i++) //grab the 4 length bytes
        {
            endian.asstring[i]=fgetc(image);
        }
        swaplocations(endian.asstring); //length to little endian format
        printf("nth chunk is %d long\n", endian.asnumber);
        if (endian.asnumber==firstidatlength)//is the next IDAT 0x8000 long? ie the maximum?
        {
            printf("IDAT 2 is the same length as IDAT 1\n");
            bytesread=0;
            fread(IDATCHUNK, 1, firstidatlength, image); // get the next IDAT chunk
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
            fread(lastidat, 1, endian.asnumber, image); // get the next IDAT chunk
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
