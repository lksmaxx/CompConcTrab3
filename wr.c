#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define V_SIZE 1000000

int main(int argc,char* argv[])
{
	FILE* file;
	srand(time(NULL));
	int* buffer;
	buffer = malloc(sizeof(int)*V_SIZE);
	int test = 0;
	for(int i = 0; i < V_SIZE - 6; i++)
	{
		buffer[i] = rand() % 5;
	}

	
	for(int i = 0; i < 6;i++)
	{
		buffer[V_SIZE - 6 + i] = i;
	}
	if(test == 1)
	{
		int of = 9999;
		for(int i = of; i < of + 500; i++)
		{
			buffer[i] = 2;
		}
	}
	for(int i = 0; i< V_SIZE; i++)
	{
		printf("%d\n",buffer[i]);
	}
	file = fopen(argv[1],"w+b");
	long long int s = V_SIZE;
	fwrite(&s,sizeof(s),1,file);
	fwrite(buffer, sizeof(int), s, file);
	
	fclose(file);
	free(buffer);
	return 0;
}
