#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

int main(int argc, char *argv[]){
  if(argc!=3){
    printf("Wrong number of arguments. Usage:\n");
    printf("./fixPythonBinPeakFile inFile outFile\n");
    printf("Fixes the problem in the python binary file: nPeaks are written twice, and this program fixes it.\n");
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
  
  //Input peaks file format:
  //nPeaks : uint64_t
  //nPeaks : uint64_t
  //for(peak=0;peak<nPeaks;peak++){
  //  panel : uint16_t
  //  row : uint16_t
  //  col : uint16_t
  //}
  uint64_t nPeaks1;
  uint64_t nPeaks2;
  // uint16_t panel;
  // uint16_t row;
  // uint16_t col;
  uint16_t data[3];
  uint64_t peak;
  
  // calibPanel=(uint8_t*)malloc(185*388*sizeof(uint8_t));
  
  while(1){
    fread(&nPeaks1,sizeof(uint64_t),1,inbf);
    if(feof(inbf))
      break;
    fread(&nPeaks2,sizeof(uint64_t),1,inbf);
    assert(nPeaks1==nPeaks2);
    fwrite(&nPeaks1,sizeof(uint64_t),1,outbf); 
    // printf("nPeaks = %u\n",(unsigned int)nPeaks1);
    
    for(peak=0;peak<nPeaks1;peak++){
      // fread(panel,sizeof(uint16_t),1,inbf); 
      // fread(row,sizeof(uint16_t),1,inbf); 
      // fread(col,sizeof(uint16_t),1,inbf); 
      fread(data,sizeof(uint16_t),3,inbf);
      fwrite(data,sizeof(uint16_t),3,outbf); 
      // printf("Panel= %d Row=%d Col=%d\n",data[0],data[1],data[2]);
    }
  }
  fclose(outbf);
  fclose(inbf);
  return 0;
}

