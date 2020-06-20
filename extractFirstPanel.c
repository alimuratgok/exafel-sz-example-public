#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

int main(int argc, char *argv[]){
  if(argc!=3){
    printf("Wrong number of arguments. Usage:\n");
    printf("./test inFile outFile\n");
    printf("READ THE SOURCE CODE FOR FUNTIONALITY!\n");
    return 0;
  }
  
  FILE *inbf;
  inbf=fopen(argv[1],"rb");
  if(inbf==NULL){
    printf("ERROR: Input file cannot be opened.\n");
    printf("Filename: %s\n",argv[1]);
    return 1;
  }
  
  // fseek(inbf, 0L, SEEK_END);
  // uint64_t inbf_size = ftell(inbf);
  // fseek(inbf, 0L, SEEK_SET);
  // printf("Input file size : %ld\n",inbf_size);
  
  FILE *outbf;
  outbf=fopen(argv[2],"wb");
  if(outbf==NULL){
    printf("Filename: %s\n",argv[2]);
    printf("ERROR: Output file cannot be opened.\n");
    return 1;
  }  
  
  uint8_t *calibPanel;
  calibPanel=(uint8_t*)malloc(185*388*sizeof(uint8_t));
  fread(calibPanel,sizeof(uint8_t),185*388,inbf);  
  fwrite(calibPanel,sizeof(uint8_t),185*388,outbf);
  
  fclose(outbf);
  fclose(inbf);
  return 0;
}

