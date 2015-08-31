/*************************************************
 * Copyright (c) 2015 Tacumi Susukihara           *
 * Released under the MIT licence                 *
 * http://opensource.org/licenses/mit-license.php *
 *************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#define VERSION "VIP language Compiler Version 0.0.1\n"

void usage(char *cmd);
char getByte(FILE *fp);
void VIPtoBinary(FILE *in,FILE *out);
void BinarytoVIP(FILE *in,FILE *out);

int main(int argc, char** argv)
{
  FILE *infp,*outfp;
  char *inputFileName,*outputFileName;
  void (*compile[])(FILE *,FILE *)={VIPtoBinary,BinarytoVIP};
  int opt;
  int decompile=0;
  outputFileName = "a.out";
  inputFileName=NULL;
  while ((opt=getopt(argc,argv,"dho:v")) != -1)
  {
    switch(opt)
    {
      case 'd':
        decompile=1;
        if(!strcmp("a.out",outputFileName)){
          outputFileName="a.vip";
        }
        break;
      case 'o':
        outputFileName = optarg;
        break;
      case 'v':
        fprintf(stderr,"%s",VERSION);
        return EXIT_SUCCESS;
      case 'h':
        // Fall through
      default:
        usage(argv[0]);
        return EXIT_FAILURE;

    }
  }
  while(optind < argc)
  {
    inputFileName = argv[optind++];
  }
  if(inputFileName==NULL)
  {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
  infp = fopen(inputFileName, "rb");
  outfp = fopen(outputFileName, "wb");
  (*compile[decompile])(infp,outfp);
  fclose(outfp);
  fclose(infp);
}
char* getVIPByte(FILE *in, char *buf,int carriage){
  char inputbyte;
  fread(&inputbyte,sizeof(char),1,in);
  *(buf+7)=inputbyte&1?'w':'W';
  *(buf+6)=inputbyte&2?'w':'W';
  *(buf+5)=inputbyte&4?'w':'W';
  *(buf+4)=inputbyte&8?'w':'W';
  *(buf+3)=inputbyte&16?'w':'W';
  *(buf+2)=inputbyte&32?'w':'W';
  *(buf+1)=inputbyte&64?'w':'W';
  *(buf+0)=inputbyte&128?'w':'W';
  *(buf+8)=carriage==3?0:' ';
  *(buf+9)=0;
  return buf;
}
void BinarytoVIP(FILE *in,FILE *out){
  char buf[10];
  char carriage=0;
  while (!feof(in)) {
    getVIPByte(in,buf,carriage);
    if(feof(in))continue;
    fwrite(&buf,sizeof(char),strlen(buf),out);
    if(!(carriage=(carriage+1)%4))fwrite("\n",sizeof(char),1,out);
  }
}
void VIPtoBinary(FILE *in,FILE *out){
  char onebyte_data;
  while (!feof(in)) {
    onebyte_data=getByte(in);
    if(feof(in))continue;
    fwrite(&onebyte_data,sizeof(char),1,out);
  }
}
void usage(char *cmd)
{
  fprintf(stderr, "usage: %s [options] file\n",cmd);
  fprintf(stderr, "%s -o outputfile\t: specify output file\n",cmd);
  fprintf(stderr, "%s -d\t\t: disassemble input file\n",cmd);
  fprintf(stderr, "%s -h\t\t: show help\n",cmd);
  fprintf(stderr, "%s -v\t\t: show version\n",cmd);
}
char getByte(FILE *fp)
{
  char ret,buf;
  int idx;
  idx=0;
  ret=0;
  while(idx < sizeof(char)*8){
    buf=fgetc(fp);
    ret*=2;
    if(buf == 'w'){
      ret+=1;
      idx++;
    }else if(buf == 'W' || buf==EOF){
      idx++;
    }
  }
  return ret;
}
