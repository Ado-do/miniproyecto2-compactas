#include <stdio.h>
#include <math.h>
#include <string.h>
#include "kTree.h"



int main(int argc, char* argv[]){

	if(argc<2){
		fprintf(stderr,"USAGE: %s <GRAPH>\n",argv[0]);
		return(-1);
	}

	char *filename = (char *)malloc(sizeof(char)*(strlen(argv[1])+10));
	TREP * rep = loadTreeRepresentation(argv[1]);
	
  strcpy(filename,argv[1]);
  strcat(filename,".rbfull");
	FILE *fr = fopen(filename,"w");
	 fwrite(&(rep->numberOfNodes),sizeof(uint),1,fr);
  fwrite(&(rep->numberOfEdges),sizeof(ulong),1,fr);
	
	int * listady;
	listady = (int *) compactFullDecompression(rep);

//	for(int i=0;i<10;i++)
//		fprintf(stderr, "%d \t", listady[i] );

//	fprintf(stderr,"\nnumber of Nodes %d number of edges %d\n",rep->numberOfNodes,rep->numberOfEdges);

	fwrite(listady,sizeof(int),rep->numberOfNodes+rep->numberOfEdges,fr);
  
  fclose(fr);
  
//  destroyTreeRepresentation(rep);
  free(filename);
  free(listady);  
  return 0;
}


