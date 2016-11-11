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
	int idatstreamcount=0;
	int dimensions[4];
	int firstidatlength, array_len;
	getheader(image, dimensions);
	printf("x = %d, y = %d, depth = %d, mode = %d\n", dimensions[0], dimensions[1], dimensions[2], dimensions[3]);
	int image_width=dimensions[0];
	int image_height=dimensions[1];
	// READ FILE UNTIL WE FIND IDAT
    idatread(image, firstidatoverrun, &firstidatlength, &array_len);
    /*printf ("IDATlen = %d\n", firstidatlength);
    printf ("array_len = %d\nIDATOVERRUN =", array_len);
    printf("\n"); */
    char *IDATCHUNK=malloc(sizeof(char)*((firstidatlength))); // dynamically work with our memory
    char *idatstream=malloc(sizeof(char)*(firstidatlength*25));
    char *temp=malloc(sizeof(char)*(firstidatlength-array_len));
    for (i=0; i<array_len; i++) //do this instead of strcpy so we copy any null bytes
    {
        IDATCHUNK[i]=firstidatoverrun[i];
    }

    //idatcount++;
    //read the rest of IDAT chunk leaving off the crc at the end
    fread(temp, 1, (firstidatlength-array_len), image);

    for (i=0; i<(firstidatlength-array_len); i++)
    {
        IDATCHUNK[i+array_len]=temp[i];
    }
    free(temp);

   // int success=inflate_mod(IDATCHUNK, firstidatlength, inflatechunk, firstidatlength);
    //printf("number of bytes inflated = %d\n", success);
   // idatcount++;
    //pixel pixelarray[dimensions[1]][dimensions[2]];
    for (i=0; i<firstidatlength; i++)
    {
        idatstream[i]=IDATCHUNK[i];
        idatstreamcount++;
    }
    pixel **pixelarray=malloc(sizeof(pixel*)*dimensions[1]);
    int columns=0;
    for (columns=0; columns<dimensions[1]; columns++)
    {
        pixelarray[columns]=malloc(sizeof(pixel)*dimensions[0]);
    }
    bytesread=0; // populate our pixel array with the first IDAT chunk
    y=0;
  /*  while (bytesread<firstidatlength)
    {
            for (x=0; x<(dimensions[0]*3); x++)
            {
                switch (bytesread%3)
                {
                case 0: //red
                    {
                        //printf("assigned red pixel after %d bytes, x = %d\n", bytesread, x);
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
    */
    //printf("first IDAT stored\n");
    // grab the rest of the IDAT chunks but skip over the 4 byte crc. Grab the length and compare it against 0x8000
    char discarded;
    int idatcounter=0;
    while (1) //can we be more specific here?
    {
        for (i=0; i<4; i++) //skip over crc
        {
            discarded=fgetc(image);
           // printf("discarded = %c (0x%X)\n", discarded, discarded);
        }
        //printf("discarded the 4 crc bytes\n");
        for (i=0; i<4; i++) //grab the 4 length bytes
        {
            endian.asstring[i]=fgetc(image);
        }
         for (i=0; i<4; i++) //skip over IDAT
        {
            discarded=fgetc(image);
          //  printf("discarded = %c (0x%X)\n", discarded, discarded);
        }
        swaplocations(endian.asstring); //length to little endian format
       // printf("nth chunk is %d long\n", endian.asnumber);
        if (endian.asnumber==firstidatlength)//is the next IDAT 0x8000 long? ie the maximum?
        {
           // printf("IDAT n is the same length as IDAT 1\n");
            bytesread=0;

            fread(IDATCHUNK, 1, firstidatlength, image); // get the next IDAT chunk
            //success=inflate_mod(IDATCHUNK, firstidatlength, inflatechunk, firstidatlength);
            //idatcount++;
            //printf("number of bytes inflated = %d\n", success);
            for (idatcounter=0; idatcounter<firstidatlength; idatcounter++)
            {
                idatstream[idatcounter+idatstreamcount]=IDATCHUNK[idatcounter];
            }
            idatstreamcount+=firstidatlength;

         /*   while (bytesread<firstidatlength) // populate the next part of the array
            {
                while (x<(dimensions[0]*3)) // our x and y is saved from last time
                {
                     switch (bytesread%3)
                    {
                        case 0: //red
                        {
                           // printf("assigned red pixel after %d bytes, x = %d\n", bytesread, x);
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
                    if (bytesread>=firstidatlength) { break;}
                   // if (x/3==(dimensions[0])) {printf("im breaking here!\n"); break;}
                }
                if(bytesread<=firstidatlength) {y+=1; x=0;}
            }
          //  printf("bytesread = %d\nanother IDAT processed\n", bytesread); */
        }
        else // if the IDAT is smaller
        {
            free(IDATCHUNK); //garbage collection
            //free(inflatechunk);
            unsigned char *lastidat=malloc(sizeof(char)*endian.asnumber);
           // unsigned char *inflatechunk=malloc(sizeof(char)*endian.asnumber);
            bytesread=0;
            fread(lastidat, 1, endian.asnumber, image); // get the next IDAT chunk
            //success=inflate_mod(IDATCHUNK, endian.asnumber, inflatechunk, endian.asnumber);
           // idatcount++;
            for (idatcounter=0; idatcounter<endian.asnumber; idatcounter++)
            {
                idatstream[idatcounter+idatstreamcount]=lastidat[idatcounter];
            }
            idatstreamcount+=endian.asnumber;
           /* while (bytesread<endian.asnumber) // populate the next part of the array
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
                if(bytesread<firstidatlength) {y+=1; x=0;}
            }*/
            free(lastidat);
             //done garbage collection
            break;
        }

    }
    idatstream=idatstream+2; // move over the header of the DEFLATE compressed data
    idatstreamcount=idatstreamcount-2;
    char *IDATSTR=malloc(sizeof(char)*idatstreamcount); //dynamic memory assignment for zlib decompression
    for (idatcounter=0; idatcounter<idatstreamcount; idatcounter++)
    {
        IDATSTR[idatcounter]=idatstream[idatcounter];
    }
    printf("IDATSTR[0]= 0x%x\n", IDATSTR[0]);
    printf("IDATSTR[%d]= 0x%x\n", idatstreamcount, IDATSTR[idatstreamcount]);
    idatstream=idatstream-2;
    free(idatstream); // free up some much needed memory
    //printf("idatstream free!\n");
    char *inflatechunk=malloc(sizeof(char)*idatstreamcount);

    //printf("idatstreamcount = %d\nsizeof(inflatechunk) = %d\n", idatstreamcount, firstidatlength*25);
    //int success=inflate_mod(idatstream, idatstreamcount, inflatechunk, idatstreamcount);
	int success=inflate_mod(IDATSTR, idatstreamcount, inflatechunk, idatstreamcount);

	//int max_y_origin, max_x_origin, max_y_corner, max_x_corner;
    int dbgprint;
    //printf("idatcount = %d\n", idatcount);
    FILE *outfile=fopen("hex.dump", "wb");
    fwrite(IDATSTR, 1, idatstreamcount, outfile);
    fclose(outfile);
    for (dbgprint=0; dbgprint<10; dbgprint++)
    {
        printf("inflate chunk byte (%d) = 0x%X\n", dbgprint, inflatechunk[dbgprint]);
      //printf("pixel 1,%d = rgb(%d,%d,%d)\n", dbgprint, pixelarray[0][dbgprint].red, pixelarray[0][dbgprint].green, pixelarray[0][dbgprint].blue);
    }
    /*success=inflate_mod(inflatechunk, idatstreamcount, IDATSTR, idatstreamcount);
    outfile=fopen("hex-2.dump", "wb");
    fwrite(inflatechunk, 1, idatstreamcount, outfile);
    fclose(outfile);*/
	//searchsquare(pixel **array, int x_pos, int width, int y_pos, int height, int *max_x, int *max_y)
   // searchsquare(pixelarray, 5, 50, 5, 50, &max_x_origin, &max_y_origin); //gives approx a 20x20 box around the centre of the top left well
    //printf("found max in top left in %dx%d square at (%d, %d)\n", SCANDIMENSION, SCANDIMENSION, max_x_origin, max_y_origin);
    //searchsquare(pixelarray, image_width-50, 20, 5, 50, &max_x_corner, &max_y_corner);
    //printf("found max in top right in 10x10 square at (%d, %d)\n", max_x_corner, max_y_corner);
	//int darksearch=scoresquare(pixelarray, 21, 31);
	//printf("darkest square by eye has sqrwt = %d\n", darksearch);
	return 0;
}
