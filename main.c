#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <assert.h>
#include </auto/dtchome/sawardb/coding/project/project.h>
//#include </home/ben/Documents/Oxford/DTP/project/DTC-Project/project.h>


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

    //read the rest of IDAT chunk leaving off the crc at the end
    fread(temp, 1, (firstidatlength-array_len), image);

    for (i=0; i<(firstidatlength-array_len); i++)
    {
        IDATCHUNK[i+array_len]=temp[i];
    }
    free(temp);

    for (i=0; i<(firstidatlength); i++) //-4 to cut the trailing DEFLATE checksum (apparently separate to the CRC)
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
            for (idatcounter=0; idatcounter<(firstidatlength); idatcounter++)
            {
                idatstream[idatcounter+idatstreamcount]=IDATCHUNK[idatcounter];
            }
            idatstreamcount+=(firstidatlength);

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
            for (idatcounter=0; idatcounter<(endian.asnumber); idatcounter++)
            {
                idatstream[idatcounter+idatstreamcount]=lastidat[idatcounter];
            }
            idatstreamcount+=(endian.asnumber);

            free(lastidat);
             //done garbage collection
            break;
        }

    }
    idatstream=idatstream+2; // move over the header of the DEFLATE compressed data
    idatstreamcount=idatstreamcount-6;
    char *IDATSTR=malloc(sizeof(char)*idatstreamcount); //dynamic memory assignment for zlib decompression
    for (idatcounter=0; idatcounter<(idatstreamcount); idatcounter++)
    {
        IDATSTR[idatcounter]=idatstream[idatcounter];
    }
    printf("IDATSTR[0]= 0x%x\n", IDATSTR[0]);
    printf("IDATSTR[%d]= 0x%x\n", idatstreamcount, IDATSTR[idatstreamcount]);
    idatstream=idatstream-2;
    free(idatstream); // free up some much needed memory
    //printf("idatstream free!\n");
    char *inflatechunk=malloc(sizeof(char)*((image_width*3)+1)*image_height);

    //printf("idatstreamcount = %d\nsizeof(inflatechunk) = %d\n", idatstreamcount, firstidatlength*25);
    //int success=inflate_mod(idatstream, idatstreamcount, inflatechunk, idatstreamcount);
	int success=inflate_mod(IDATSTR, idatstreamcount, inflatechunk, idatstreamcount); // we now have a filtered data stream
    free(IDATSTR);
    bytesread=0;
    char *templine=malloc((image_width*3)+1);
    char *nextscanline=malloc((image_width*3)+1);
    for (i=0; i<(image_width*3)+1; i++)
    {
        nextscanline[i]=inflatechunk[i];
        bytesread++;
    }
    int linelength=bytesread;
    printf("bytesread = %d\n", bytesread);
    int iterations=0;
    while (bytesread < idatstreamcount)
    {
        switch (nextscanline[0])
        {
        case 0: // #nofilter
        {
            break;
        }
        case 1: // sub filtering
            {
             //   printf("sub filtering used for scanline %d\n", iterations);
                for (i=4; i<linelength; i++)
                {
                    nextscanline[i]=(nextscanline[i]+nextscanline[i-3])%256; //all filters are mod 256
                }
                break;
            }
        case 2: //up
            {
                for(i=1; i<linelength; i++)
                {
                    nextscanline[i]=(nextscanline[i]+templine[i])%256;
                }
                break;
            }
        case 3: // average left and above
            {
                for (i=4; i<linelength; i++)
                {
                    nextscanline[i]=(nextscanline[i]+(nextscanline[i-3]+templine[i])/2)%256;
                }
                break;
            }
        case 4: // Paeth (EVIL!) https://www.w3.org/TR/PNG-Filters.html
            {
                //printf("WARN : paeth scanline : %d\n", iterations);
                int predictor, pa, pb, pc;
                for (i=4; i<linelength; i++)
                {
                    predictor=(nextscanline[i-3]+templine[i]-templine[i-3]);
                    pa=abs(predictor-nextscanline[i-3]);
                    pb=abs(predictor-templine[i]);
                    pc=abs(predictor-templine[i-3]);
                    if (pa<=pb && pa<=pc)
                    {
                        nextscanline[i]=(nextscanline[i]+nextscanline[i-3])%256;
                    }
                    else if (pb<=pc)
                    {
                        nextscanline[i]=(nextscanline[i]+templine[i])%256;
                    }
                    else
                    {
                        nextscanline[i]=(nextscanline[i]+templine[i-3])%256;
                    }
                }
                break;
            }
        }
        bytesread+=linelength;
        if (bytesread < idatstreamcount)
        {
            for (i=0; i<linelength; i++)
            {
                inflatechunk[(iterations*linelength)+i]=nextscanline[i];
                templine[i]=nextscanline[i];
                nextscanline[i]=inflatechunk[i+bytesread];
            }
             iterations+=1;
        }
        else
        {
         // printf("bytesread=%d, idatstreamcount=%d\n", bytesread, idatstreamcount);
            for (i=0; i<linelength; i++)
            {
               // printf("bytesread=%d, idatstreamcount=%d\n", (iterations*linelength)+i, idatstreamcount);
                inflatechunk[(iterations*linelength)+i]=nextscanline[i];
            }
            break;
        }

    }

    FILE *outfile=fopen("scanlines.dump.hex", "wb");
    fwrite(inflatechunk, 1, idatstreamcount, outfile);
    fclose(outfile);
	for (y=0; y<image_height; y++)
    {
        bytesread=0;
        //printf("skipping byte 0x%X\n", inflatechunk[bytesread]);
        bytesread=1; //time to populate the array! set to 1 to skip over the filter byte
        for(x=0; x<(image_width*3); x++)
        {
            switch (x%3)
            {
            case 0:
                {
                    pixelarray[y][(x)/3].red=inflatechunk[(y*(image_width*3))+bytesread];
                    break;
                }
            case 1:
                {
                    pixelarray[y][x/3].green=inflatechunk[(y*(image_width*3))+bytesread];
                    break;
                }
            case 2:
                {
                    pixelarray[y][x/3].blue=inflatechunk[(y*(image_width*3))+bytesread];
                    break;
                }
            }
            bytesread+=1;
        }
      //  printf("y = %d, y_max= %d, bytesread = %d\n", y, image_height, bytesread);
    }


    int max_x_origin, max_y_origin;
    searchsquare(pixelarray, 5, 50, 5, 50, &max_x_origin, &max_y_origin); //gives approx a 20x20 box around the centre of the top left well
    printf("found max in top left in %dx%d square at (%d, %d)\n", SCANDIMENSION, SCANDIMENSION, max_x_origin, max_y_origin);
    //searchsquare(pixelarray, image_width-50, 20, 5, 50, &max_x_corner, &max_y_corner);
    //printf("found max in top right in 10x10 square at (%d, %d)\n", max_x_corner, max_y_corner);
	//int darksearch=scoresquare(pixelarray, 21, 31);
	//printf("darkest square by eye has sqrwt = %d\n", darksearch);
	return 0;
}
