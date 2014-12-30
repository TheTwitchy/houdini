#include <stdio.h>
#include <unistd.h>

#define MAJ_VERSION 1
#define MIN_VERSION 1
#define AUTHOR "TheTwitchy"

#define BUFFER_SIZE 1024

/*Escape type options*/
#define URL_ESC 0
#define PHP_ESC 1
#define HTML_ESC 2

void printHelp(char * name){
  printf("%s v%d.%d by %s\n", name, MAJ_VERSION, MIN_VERSION, AUTHOR);
  printf("\tinfo: a tool to escape stdin(default) or files into another ASCII readable form\n");
  printf("\tusage: %s [-e escape_type] [-i input_file] [-o output_file]\n", name);
  printf("\toptions:\n\t\t-i input filename\n\t\t-o output filename\n");
  printf("\t\t-e escape type (html,php,url)\n");
}

int main(int argc, char ** argv){
  /*Defaults*/
  int escType = URL_ESC;
  int c;

  FILE * inFilePtr = NULL;
  FILE * outFilePtr = NULL;

  while((c = getopt(argc, argv, "he:i:o:")) != -1){
    switch (c){
    case 'h':
      printHelp(argv[0]);
      return 0;
    case 'e':
      if(strcmp("url", optarg) == 0) escType = URL_ESC;
      else if(strcmp("php", optarg) == 0) escType = PHP_ESC;
      else if(strcmp("html", optarg) == 0) escType = HTML_ESC;
      else{
	fprintf(stderr, "%s: unknown escape type '%s'\n", argv[0], optarg);
	return -1;
      }
      break;
    case 'i':
      inFilePtr = fopen(optarg, "rb");
      if(inFilePtr == NULL){
	fprintf(stderr, "%s: failed to open file '%s'\n", argv[0], optarg);
	return -1;
      }
      break;
    case 'o':
      outFilePtr = fopen(optarg, "wb");
      if(outFilePtr == NULL){
	fprintf(stderr, "%s: failed to open file '%s'\n", argv[0], optarg);
	return -1;
      }
      break;
    default:
      break;
    }
  }
  
  if(inFilePtr == NULL){
    inFilePtr = stdin;
  }
  if(outFilePtr == NULL){
    outFilePtr = stdout;
  }
  
  unsigned char buf[BUFFER_SIZE];
  unsigned int bytesRead;
  while((bytesRead = fread((void *)buf, sizeof(char), BUFFER_SIZE, inFilePtr)) != 0){
    int n;  
    for(n = 0;n < BUFFER_SIZE && n < bytesRead; n++){
      switch(escType){
      case URL_ESC:
	fprintf(outFilePtr, "%%%02x", buf[n]);
	break;
      case PHP_ESC:
	fprintf(outFilePtr, "\\x%02x", buf[n]);
	break;
      case HTML_ESC:
	fprintf(outFilePtr, "&#%d;", buf[n]);
	break;
      }
    }
  }
  if(outFilePtr == stdout) printf("\n");
  fclose(inFilePtr);
  fclose(outFilePtr);
  return 0;
}
