#include <stdlib.h>
#include <stdio.h>

FILE *output;

void writeToFile(char messageString[]);

int main(int argc, char *argv[])
{
	/*The file name is given in the first argument*/
	output = fopen(argv[1], "w");
	if(!output)
	{
		perror("Opening the outoutfile");
		exit(EXIT_FAILURE);
	}
	writeToFile(argv[2]);
	fclose(output);
}

void writeToFile(char messageString[])
{
	/*Things to improve, Make it not overide the entire file contents*/
	fprintf(output, "%s\n", messageString);
}

