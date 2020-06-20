#include <stdio.h>

// TO DO LIST:
// Compressed buffer: All counters are uint64_t. Maybe we can fix this? DO NOT FORGET: compressedSize
// COMPRESS: Write peaks into the compressed file just while reading them from the input buffer.
// COMPRESS: Directly write as much as possible into the output buffer? (Instead of using intermediate buffers)
// COMPRESS: nPeaksTotal may not be needed to be written into the output buffer.

#include "sz.h"

int main(int argc, char *argv[]){
  
  if(argc!=6 && argc!=7){
    printf("EXAFEL data compression\n");
    printf("Wrong number of arguments. Usage:\n");
    printf("./exafelsz originalFile peaksFile calibFile compFile decompFile\n");
    assert(0);
    return 0;
  }
  
  //Config:
  exafelSZ_params pr;
  pr.binSize=2;
  pr.tolerance=15;
  pr.szDim=2;
  //pr.szBlockSize=?; //Currently unused
  pr.peakSize=17; //Must be odd.
  
  size_t panels=32;
  size_t rows=185;
  size_t cols=388;
  
  FILE *origFile, *peaksFile, *calibFile, *compFile, *decompFile;

  origFile=fopen(argv[1],"rb");
  peaksFile=fopen(argv[2],"rb");
  calibFile=fopen(argv[3],"rb");
  compFile=fopen(argv[4],"wb");
  decompFile=fopen(argv[5],"wb");
  
  if(origFile==NULL) assert(0);
  if(peaksFile==NULL) assert(0);
  if(calibFile==NULL) assert(0);
  if(compFile==NULL) assert(0);
  if(decompFile==NULL) assert(0);
  
  size_t n;
  uint64_t eventSize=panels*rows*cols*sizeof(float);
  
  if(argc==6){
    fseek(origFile,0L,SEEK_END);
    uint64_t origFileSize=ftell(origFile);
    fseek(origFile,0L,SEEK_SET);
    if(origFileSize%eventSize != 0){
      printf("ERROR: origFileSize \% eventSize = %d \% %d = %d != 0",origFileSize,eventSize,origFileSize%eventSize);
      assert(0);
    }
    n=origFileSize/eventSize;
  }else if(argc==7){
    n=atoi(argv[6]);
  }
  // printf("N : %d\n",n);
  
  pr.calibPanel=(uint8_t*)malloc(rows*cols);
  if(pr.calibPanel==NULL){
    printf("ERROR: Allocation for pr.calibPanel is failed.\n");
    printf("Allocation size tried: %ld\n",rows*cols*sizeof(uint8_t));
    assert(0);
  }
  fread(pr.calibPanel,sizeof(uint8_t),rows*cols,calibFile);
  
  // fseek(peaksFile,0L,SEEK_END);
  // uint64_t peaksFileSize=ftell(peaksFile);
  // fseek(peaksFile,0L,SEEK_SET);
  // pr.peaks=(uint8_t*)malloc(peaksFileSize);
  // fread(pr.peaks,sizeof(uint8_t),peaksFileSize,peaksFile);
  
  uint64_t peaksBufSize=10485760; //10MB should more than enough (1747625 peaks capacity)
  //Peaks buffer look like this: 
  //uint64_t nPeaks
  //nPeaks{
  //   uint16_t p_
  //   uint16_t r_
  //   uint16_t c_
  //}
  pr.peaks=(uint8_t*)malloc(peaksBufSize);
  if(pr.peaks==NULL){
    printf("ERROR: Allocation for pr.peaks is failed.\n");
    printf("Allocation size tried: %ld\n",peaksBufSize);
    assert(0);
  }
  
  float *origData=(float*)malloc(eventSize);
  if(origData==NULL){
    printf("ERROR: Allocation for origData is failed.\n");
    printf("Allocation size tried: %ld\n",eventSize);
    assert(0);
  }
  
  //if(peaksFileSize>10*1024*1024) assert(0);
  ////if(nEvents*panels*rows*cols>250*1024*1024) assert(0);
  //if(panels*rows*cols>250*1024*1024) assert(0);
  
  int compStatus;
  SZ_Init(NULL);
  
  size_t e;
  for(e=0;e<n;e++){
    fread(origData,sizeof(float),panels*rows*cols,origFile);
    
    fread(pr.peaks,sizeof(uint64_t),1,peaksFile);
    //fread(pr.peaks,sizeof(uint64_t),1,peaksFile); //Python 2 needs this line!

    uint64_t nPeaks=*(uint64_t*)(pr.peaks);
    // printf("nPeaks = %d\n",nPeaks);
    if(nPeaks*6+8 > peaksBufSize){  //If true, it means: nPeaks > 1747625
      printf("WARNING: nPeaks is too large: %ld\n",nPeaks);
      free(pr.peaks);
      peaksBufSize*=2;
      pr.peaks=(uint8_t*)malloc(peaksBufSize);
      if(pr.peaks==NULL){
        printf("ERROR: pr.peaks has been failed to be allocated.\n");
        printf("Allocation size tried: %ld\n",peaksBufSize);
        assert(0);
      }
      *(uint64_t*)pr.peaks=nPeaks;
    }
    fread(pr.peaks+8,sizeof(uint16_t),3*nPeaks,peaksFile);
    
    unsigned char *compressedBuffer; //No need to allocate, but you probably should deallocate!
    size_t compressedSize;
    compressedBuffer=SZ_compress_customize("ExaFEL",(void*)(&pr),SZ_FLOAT,(void*)origData,0,1,panels,rows,cols,&compressedSize,&compStatus);
  
    fwrite(compressedBuffer,sizeof(char),compressedSize,compFile);
  
    void *decompressedBuffer; //No need to allocate, but you probably should deallocate!
    int decompStatus;
    decompressedBuffer=SZ_decompress_customize("ExaFEL",(void*)(&pr),SZ_FLOAT,compressedBuffer,compressedSize,0,1,panels,rows,cols,&decompStatus);
    
    //fwrite(decompressedBuffer,sizeof(float),nEvents*panels*rows*cols,decompFile);
    fwrite(decompressedBuffer,sizeof(float),panels*rows*cols,decompFile);
    
    free(compressedBuffer);
    free(decompressedBuffer);
  }
  SZ_Finalize();
  
  // if(origData==NULL){printf("origData=NULL\n");}
  // if(compressedBuffer==NULL){printf("compressedBuffer=NULL\n");printf("compStatus=%d\n",compStatus);}
  // if(decompressedBuffer==NULL){printf("decompressedBuffer=NULL\n");printf("decompStatus=%d\n",decompStatus);}
  // if(pr.calibPanel==NULL){printf("pr.calibPanel=NULL\n");}
  // if(pr.peaks==NULL){printf("pr.peaks=NULL\n");}
  
  free(origData);
  free(pr.calibPanel);
  free(pr.peaks);
  
  fclose(origFile);
  fclose(peaksFile);
  fclose(calibFile);
  fclose(compFile);
  fclose(decompFile);
  
  return 0;
}  
