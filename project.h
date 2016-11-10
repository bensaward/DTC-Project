#ifndef _PROJECT_H
#define _PROJECT_H
/*
Bit depth:          1 byte
   Color type:         1 byte
   Compression method: 1 byte
   Filter method:      1 byte
   Interlace method:   1 byte
*/
#define CHUNK 16384
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
int inf(FILE *source, FILE *dest); // 11 December 2005  Mark Adler (http://www.zlib.net/zpipe.c)
void getheader(FILE *image, int *retvalue); // lets find the IDHR of our png file and read it to get dimensions
void findwell(/* args */); // use circular edge detection to find the well plates
void greyscaleimage(pixel **image); /* for this we need our image in grey scale */
void swaplocations(char *array); // swap our big endian number to a little endian number
void idatread(FILE *image, char *array, int *length, int *array_len); //read a block of the idat
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
    char *buff=malloc(sizeof(char)*41);
    char *tempholder=malloc(sizeof(char)*4);
    int overruncounter=0; //count how far into the IDAT buffer we ran and hold those bytes
    int movedforward=0;
    fgets(buff, 40, image);
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
                    overruncounter=40-i;
                    if (overruncounter>0)
                    {
                      //  printf("overruncounter = %d\n", overruncounter);
                        char *IDATCHUNK=malloc(sizeof(char)*overruncounter);
                       *array_len=overruncounter;
           //            printf("array_len in header = %d\n  ", *array_len);
                        for (x=0; x<overruncounter; x++)
                        {
                      //      printf("putting data into IDAT overrun\n");
                            IDATCHUNK[x]=buff[i+4+x];
                        }
                        strcpy(array, IDATCHUNK);
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
            fgets(buff, 40, image);
        }
    }
}

int inf(FILE *source, FILE *dest) // 11 December 2005  Mark Adler (http://www.zlib.net/zpipe.c)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


#endif
