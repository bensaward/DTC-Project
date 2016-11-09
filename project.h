#ifndef _PROJECT_H
#define _PROJECT_H
/*
Bit depth:          1 byte
   Color type:         1 byte
   Compression method: 1 byte
   Filter method:      1 byte
   Interlace method:   1 byte
*/

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

void getheader(FILE *image, int *retvalue); // lets find the IDHR of our png file and read it to get dimensions
void findwell(/* args */); // use circular edge detection to find the well plates
void greyscaleimage(pixel **image); /* for this we need our image in grey scale */
void swaplocations(char *array); // swap our big endian number to a little endian number
void idatread(FILE *image, char *array); //read a block of the idat
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

void idatread(FILE *image, char *array)
{
    // we need to collect the 4 bytes before IDAT to see its length
    int i=0;
    char *IDAT="IDAT";
    char *buff=malloc(sizeof(char)*41);
    char *holder=malloc(sizeof(char)*4);
    int overruncounter=0;
    int movedforward=0;
    fgets(buff, 40, image);
    int endofstring=0;
    printf("Passing into idatread\n");
    while (strstr(buff, IDAT) == NULL)
    {
        printf("strstr didnt detect IDAT\n");
        printf("buffer = %s\n", buff);

        while(buff[0]=='\0') // deal with nulls again loop1
        {
            buff+=sizeof(char);
            movedforward++;
            printf("jumped forward! now on %d\n", movedforward);
            printf("buffer after 1 = %s\n", buff);
            //if (movedforward > 40)
            //{
             //   break;
            //}
        }
        if (strstr(buff, IDAT) != NULL)//here we know the string doesnt begin with \0
        {
            break;
        }
        else
        {
             printf("strstr didnt detect IDAT\n");
            for (i=0; i<(41-movedforward); i++) //loop2
            {
                if (buff[i]=='\0') //jump to our next series of null,
                    //trying to catch a match with \0asjds\0hfjsf\0\0\0IDAT for eg
                {
                    endofstring=i;
                    break;
                }
            }
            if ((endofstring+movedforward)<36)
            {
                printf("storing values before we skip\n");
                holder[0]=buff[endofstring+movedforward-3];
                holder[1]=buff[endofstring+movedforward-2];
                holder[2]=buff[endofstring+movedforward-1];
                holder[3]=buff[endofstring+movedforward];
                printf("moved along %d bytes\n", endofstring+movedforward);
                buff+=(sizeof(char)*endofstring);
                printf("buffer after loop 2 = %s\n", buff);
                movedforward+=endofstring;
            }
            else
            {
                        printf("storing last 4 values\n");
                        holder[0]=buff[36-movedforward];
                        holder[1]=buff[37-movedforward];
                        holder[2]=buff[38-movedforward];
                        holder[3]=buff[39-movedforward];
                        for (i=1; i<4; i++) // check if we have part of the IDAT header
                        {
                            if (holder[i]=='I')
                            {
                                char nextchar=fgetc(image);
                                if (nextchar==IDAT[4-i])
                                {
                                    endian.asstring[0]=buff[36-movedforward-(4-i)]; //store the length in little endian format
                                    endian.asstring[1]=buff[37-movedforward-(4-i)];
                                    endian.asstring[2]=buff[38-movedforward-(4-i)];
                                    endian.asstring[3]=buff[39-movedforward-(4-i)];
                                    break;

                                }
                            }
                        }
                        fgets(buff, 40, image);
                        printf("getting more from file");
                        endofstring=0;
                        movedforward=0;
            }
        }
        //if (strstr(buff, "IDAT") != NULL)
        //{
          //  break;
        //}
        printf("Press enter to cycle through again!\n");
        getchar();

    }
    char *startofstring=&buff[0];
    char *IDAT_LOC=strstr(buff, IDAT);
    if (IDAT_LOC < startofstring+4)
    {
        switch(IDAT_LOC-startofstring)
        {
            case 0:
                {
                    endian.asstring[0]=holder[0];
                    endian.asstring[1]=holder[1];
                    endian.asstring[2]=holder[2];
                    endian.asstring[3]=holder[3];
                    break;
                }
            case 1:
                {
                    endian.asstring[0]=holder[1];
                    endian.asstring[1]=holder[2];
                    endian.asstring[2]=holder[3];
                    endian.asstring[3]=buff[0];
                    break;
                }
            case 2:
                {
                    endian.asstring[0]=holder[2];
                    endian.asstring[1]=holder[3];
                    endian.asstring[2]=buff[0];
                    endian.asstring[3]=buff[1];
                    break;
                }
            case 3:
                {
                    endian.asstring[0]=holder[3];
                    endian.asstring[1]=buff[0];
                    endian.asstring[2]=buff[1];
                    endian.asstring[3]=buff[2];
                    break;
                }
        }
    }
    else
    {
        for (i=0; i<4; i++)
        {
                endian.asstring[i]=*(IDAT_LOC-(sizeof(char)*4)+i);
        }
    }
 //   overruncounter=IDAT_LOC-startofstring;
 //   printf("overruncounter = %d", overruncounter);
}

#endif
