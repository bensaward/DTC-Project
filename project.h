#ifndef _PROJECT_H
#define _PROJECT_H
// we are dealing with images so lets struct our pixel format
typedef struct // use chars to save memory?
{
	int red;
	int green;
	int blue;
	// include? int alpha;
} pixel;
//BEGIN FUNCTION DEFS

void getdimensions(FILE *image, int *retvalue); // lets find the IDHR of our png file and read it to get dimensions
void findwell(/* args */); // use circular edge detection to find the well plates
void greyscaleimage(pixel **image); /* for this we need our image in grey scale */

//END FUNCTION DEFS

void getdimensions(FILE *image, int *retvalue) //return x, y dimensions if this matches what we expect, NULL otherwise
{
	char *buffer=malloc(sizeof(char)*101); //how many bytes usually before IHDR? +13 bytes for IHDR +4 for "IHDR"
	strcpy(buffer, "00000000000000000000000000");
	char *IHDR="IHDR";
	int bytes_read=0;
	//while((strstr(buffer, IHDR) == NULL))
	//{
		fgets(buffer, 100, image);
		//if ((strstr(buffer, IHDR) != NULL)
       // {
          //  break;
      //  }
		printf("%s\n", buffer);
	//}
	//lets make a string with our header in :D
	char *tempheader=strstr(buffer, IHDR);
	tempheader+=4; //move over IHDR into the buffer
	// causes SIGSEV - trying to overwrite ROM *(header+14)='\0'; // null terminate so we have a complete header
	char *header=malloc(97);
	strcpy(header, tempheader);
	header[13]='\0';
	printf("%s\n", header);
	int x,y;
	int i=0;
	scanf(header, "%d%d", &x, &y);
	/*char *x_char=malloc(sizeof(char)*5);
	x_char[4]='\0';
	for (i=0; i<4; i++)
    {
        x_char[i]=header[i];
    }
    x=(int)x_char;
    for (i=4;i<8; i++)
    {
        x_char[i]=header[i];
    }
    y=(int)x_char; */
    retvalue[0]=x;
    retvalue[1]=y;
	// free(buffer); //can we call without breaking header?
}


#endif
