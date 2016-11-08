#ifndef _PROJECT_H
#define _PROJECT_H
// we are dealing with images so lets struct our pixel format
typedef struct pixel // use chars to save memory?
{
	int red;
	int green;
	int blue;
	// include? int alpha;
};
//BEGIN FUNCTION DEFS

short int checkimagetype (FILE *image); // lets find the IDHR of our png file and read it to see if it matches the type we plan on reading
void findwell(/* args */); // use circular edge detection to find the well plates
void greyscaleimage(pixel **image); /* for this we need our image in grey scale */

//END FUNCTION DEFS

int checkimagetype (FILE *image) //return 1 if this matches what we expect, 0 otherwise
{
	char *buffer=malloc(sizeof(char)*100); //how many bytes usually before IHDR? +13 bytes for IHDR +4 for "IHDR"
	char *IDHR="IHDR";
	int bytes_read=0;
	while(!(strstr(buffer, IDHR) != NULL))
	{
		fgets(buffer, 40, image);
	}
	//lets make a string with our header in :D
	char *header=strstr(buffer, IHDR);
	header+=4; //move over IHDR into the buffer
	*(header+14)='\0'; // null terminate so we have a complete header
	// free(buffer); //can we call without breaking header?
}


#endif
