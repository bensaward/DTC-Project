#ifndef _PROJECT_H
#define _PROJECT_H

#define CHUNK 16384
#define SCANDIMENSION 10
#define SCANSQRD SCANDIMENSION*SCANDIMENSION
#define magic_constant 0.25
/*fiddled with this number to get closer to middle of well, poor results with 20x20 and 30x30
    0.95 forces bottom left of well, light area+shadow, 0.25 gives pretty good results */
#define magic_constant2 0.1
typedef struct // use chars to save memory?
{
	int red;
	int green;
	int blue;
	int alpha;
} pixel_a;

typedef struct // use chars to save memory?
{
	int red;
	int green;
	int blue;;
} pixel;

union Endian //our png image is big endian, perhaps our compiler is little endian
	{
		char asstring[4];
		int asnumber;
	} endian;

//BEGIN FUNCTION DEFS
int getmagic (pixel ** arr, int x_pos, int width, int y_pos, int height, int mode); //get magic filter values
void searchsquare(pixel ** arr, int x_pos, int width, int y_pos, int height, int *max_x, int *max_y);
int scoresquare(pixel ** arr, int x_pos, int y_pos);
void getheader(FILE *image, int *retvalue); // lets find the IDHR of our png file and read it to get dimensions
void findwell(/* args */); // use circular edge detection to find the well plates
void greyscaleimage(pixel **image); /* for this we need our image in grey scale */
void swaplocations(char *array); // swap our big endian number to a little endian number
void idatread(FILE *image, char *array, int *length, int *array_len); //read a block of the idat
int inflate_mod(const void *src, int srcLen, void *dst, int dstLen);
//END FUNCTION DEFS

void getheader(FILE *image, int *retvalue) //return x, y dimensions if this matches what we expect, NULL otherwise
{
	char *buffer=malloc(sizeof(char)*41); //how many bytes usually before IHDR? +13 bytes for IHDR +4 for "IHDR"
	strcpy(buffer, "00000000000000000000000000");
	char *IHDR="IHDR";
	int bytes_read=0;
	fgets(buffer, 40, image);
	buffer[5]='\0';
	unsigned char check=0x89;
	if (!(strstr(buffer, "PNG") != NULL && (unsigned char)buffer[0]==check)) // is the file a valid png?
    {
        printf("Not a valid PNG image\n");
    }
	while((strstr(buffer, IHDR) == NULL))
	{
		fgets(buffer, 40, image);
		while (buffer[0]=='\0')
		{
			buffer=buffer+sizeof(char); //our buffer might have a line begining with HEX 00 ie \0
		}
	}

	//lets make a string with our header in :D
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
    int color_depth=tempheader[0];
    tempheader+=sizeof(char);
    int color_mode=tempheader[0];
    retvalue[0]=x;
    retvalue[1]=y;
    retvalue[2]=color_depth;
    retvalue[3]=color_mode;
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

void idatread(FILE *image, char *array, int *length, int *array_len) //pass back some of the chunk as an array
{
    int i=0; int x=0;
    char *IDAT="IDAT";
    char *buff=malloc(sizeof(char)*40);
    char *tempholder=malloc(sizeof(char)*4);
    int overruncounter=0; //count how far into the IDAT buffer we ran and hold those bytes
    int movedforward=0;
    fread(buff, 1, 40, image);
    int endofstring=0;
   // printf("Passing into idatread\n");
    for (i=0; i<40; i++)
    {
       // printf("byte %d is 0x%x (%c)\n", i, buff[i], buff[i]);
        if (buff[i]=='I')
        {
            printf("found 'I' at pos %d", i);


            if ((i+1)<40)
            {
                if (buff[i+1]=='D') //just match ID of IDAT
                {
                    switch (i)
                    {
                        case 0:
                        {
                            for (x=0; x<4; x++)
                            {
                                endian.asstring[x]=tempholder[x];
                            }
                            break;
                        }
                        case 1:
                        {
                            for (x=0; x<3; x++)
                            {
                                endian.asstring[x]=tempholder[x];
                            }
                            endian.asstring[3]=buff[0];
                            break;
                        }
                        case 2:
                        {
                            endian.asstring[0]=tempholder[2];
                            endian.asstring[1]=tempholder[3];
                            endian.asstring[2]=buff[0];
                            endian.asstring[3]=buff[1];
                            break;
                        }
                        case 3:
                        {
                            endian.asstring[0]=tempholder[3];
                            endian.asstring[1]=buff[0];
                            endian.asstring[2]=buff[1];
                            endian.asstring[3]=buff[2];
                            break;
                        }
                        default:
                        {
                      //      printf("default behaviour\n");
                            endian.asstring[0]=buff[i-4];
                            endian.asstring[1]=buff[i-3];
                            endian.asstring[2]=buff[i-2];
                            endian.asstring[3]=buff[i-1];
                      //       printf("default behaviour complete\n");
                            break;
                        }
                    }
                    overruncounter=36-i;
                    if (overruncounter>0)
                    {
                      //  printf("overruncounter = %d\n", overruncounter);
                        //char *IDATCHUNK=malloc(sizeof(char)*overruncounter);
                       *array_len=overruncounter;
           //            printf("array_len in header = %d\n  ", *array_len);
                        for (x=0; x<overruncounter; x++)
                        {
                      //      printf("putting data into IDAT overrun\n");
                            array[x]=buff[i+4+x];
                        }
                        //strcpy(array, IDATCHUNK);
                    }

                }
            }
            else
            {
                char nextchar=fgetc(image);
                if (nextchar=='D')
                {
                    endian.asstring[0]=buff[i-4];
                    endian.asstring[1]=buff[i-3];
                    endian.asstring[2]=buff[i-2];
                    endian.asstring[3]=buff[i-1];
                }
            }
            swaplocations(endian.asstring);
            *length=endian.asnumber;
            //printf("Length of IDAT CHUNK is %d\n", *length);
            break;
        }

        if (i==39 && buff[39]!='I') //save the last 4 bytes in case the first 4 characters are the start of IDAT
        {
            tempholder[0]=buff[36];
            tempholder[1]=buff[37];
            tempholder[2]=buff[38];
            tempholder[3]=buff[39];
            i=0;
            fread(buff, 1, 40, image);
        }
    }
}


int inflate_mod(const void *src, int srcLen, void *dst, int dstLen) { //stolen from stackoverflow
    //URL = http://stackoverflow.com/questions/4901842/in-memory-decompression-with-zlib
    z_stream strm  = {0};
    strm.total_in  = strm.avail_in  = srcLen;
    strm.total_out = strm.avail_out = dstLen;
    strm.next_in   = (Bytef *) src;
    strm.next_out  = (Bytef *) dst;

    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;

    int err = -1;
    int ret = -1;

    err = inflateInit2(&strm, -MAX_WBITS/*15 + 32*/); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
    printf("err after inflateinit2 = %d\n", err);
    if (err == Z_OK) {
        err = inflate(&strm, Z_FINISH);
        printf("err after inflate = %d\n", err);
        if (err == Z_STREAM_END) {
            ret = strm.total_out;
        }
        else {
             inflateEnd(&strm);
             printf("err after inflateEnd = %d\n", err);
             return err;
        }
    }
    else {
        inflateEnd(&strm);
        return err;
    }

    inflateEnd(&strm);
    return ret;
}

int scoresquare(pixel ** arr, int x_pos, int y_pos) //score a SCANDIMENSION x SCANDIMENSION square
{
    float pixelweight=0;
    int y, x;
    for (y=0; y<SCANDIMENSION; y++)
    {
        for (x=0; x<SCANDIMENSION; x++) //greyscale weighting = 0.21 R + 0.72 G + 0.07 B (use equal weighting since pc is interpreting values)
        {
            pixelweight+=((arr[y_pos+y][x_pos+x].red+arr[y_pos+y][x_pos+x].green+arr[y_pos+y][x_pos+x].blue)/3);
            //pixelweight+=(0.21*arr[y_pos+y][x_pos+x].red)+(0.72*arr[y_pos+y][x_pos+x].green)+(0.07*arr[y_pos+y][x_pos+x].blue);
        }
    }
    return (int)pixelweight; // truncate
}

void searchsquare(pixel ** arr, int x_pos, int width, int y_pos, int height, int *max_x, int *max_y) //self written "random" (logical) search
{
    int x=0, y=0, hi_x=x_pos, hi_y=y_pos, holder=100000000, sqrwt;
    int magic_threshold=getmagic(arr, x_pos, width, y_pos, height,0);
    int magic_pass=getmagic(arr, x_pos, width, y_pos, height,1);
    for (y=0; y<height; y++)
    {
        for(x=0; x<width; x++)
        {
            sqrwt=scoresquare(arr, x_pos+x, y_pos+y);
           //
            if ((sqrwt<holder && sqrwt>magic_threshold) /*&& sqrwt<magic_pass /*do i need?*/) //threshold out darkest values to avoid matching shadows and cutoff lightest squares
            {
                holder=sqrwt;
                hi_x=x_pos+x;
                hi_y=y_pos+y;
            }
        }
    }
    printf("lowest sqrwt = %d\nmagic_threshold = %d\n", sqrwt, magic_threshold);
    for (y=0; y<5; y++) //print 4x4 of RGB bytes in hex for debug vs GIMP
    {
        for(x=0; x<5; x++)
        {
            printf("byte at %d,%d = RGB(%d, %d, %d)\n", (hi_x+x), (hi_y+y), arr[hi_y+y][hi_x+y].red, arr[hi_y+y][hi_x+y].green, arr[hi_y+y][hi_x+y].blue);
        }
    }
    *max_x=hi_x;
    *max_y=hi_y;
}

int getmagic (pixel ** arr, int x_pos, int width, int y_pos, int height, int mode) /*get our threshold and cutoff for filtering
mode 0: return threshold magic, mode 1: return cutoff magic */
{
    int x,y,numpixels=0;
    float total=0;

    float minval=255; //seed min and max as max and min respectively
    float maxval=0;
    float temp;
    for (y=0; y<height; y++)
    {
        for (x=0; x<width; x++)
        {
            temp=(arr[y_pos+y][x_pos+x].red+arr[y_pos+y][x_pos+x].green+arr[y_pos+y][x_pos+x].blue)/3;
           // temp=(0.21*arr[y_pos+y][x_pos+x].red)+(0.72*arr[y_pos+y][x_pos+x].green)+(0.07*arr[y_pos+y][x_pos+x].blue); //weight average?
            total+=temp;
            numpixels++;
            if (temp < minval && temp > 50) //avoid picking up the odd completely black pixel, only the really grey ones (fudge factor)
            {
                minval=temp; // this is 0 if there is a black pixel
            }
            if (temp > maxval && temp < 240) //avoid picking up the really light pixels
            {
                maxval=temp;
            }
        }
    }
    int average=(total/numpixels);
   /* printf("darkest possible sqrwt = %f\n", minval*SCANSQRD);
    printf("average sqrwt = %d\n", average*SCANSQRD);
    printf("lightest sqrwt = %d\n", maxval*SCANSQRD); */
    int ret;
    if (mode==0)
    {
        ret=(int)SCANSQRD*(minval+magic_constant*(average-minval));
      //  printf("sqrwt > %d\n", ret);
    }
    else
    {
        ret=(int)SCANSQRD*(average-(magic_constant2*(average-minval)));
    //    printf("sqrwt < %d\n", ret);
    }

    return ret;
}
#endif
