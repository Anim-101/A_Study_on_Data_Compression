#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"uTypes.h"
#include"BitIO.c"
#include"LempelZivList.c"
#include"AdaptiveHuffmanFGK.c"

#define WIN_BUFFSIZE 4096
#define NUM_POS_BITS 12
#define WIN_MASK 4095
#define PAT_BUFFSIZE 18
#define NUM_LEN_BITS 4

typedef struct
{
    char algo[4];
    ulong fileSize;
}fileStamp;

typedef struct
{
    uint position,length;
}pos_t;

pos_t pos;
uchar winBuf[WIN_BUFFSIZE];
uchar pattern[PAT_BUFFSIZE];
uint win_cnt=0;

int main(int argc,char *argv[])
{
    ulong fsize=0;
    uint i,k;
    FILE *out;
    fileStamp fstamp;


    if(argc!=3)
    {
        fprintf(stderr,"Basic Lempel Ziv 77 And Adaptive Huffman Based on FGK Algorithm's Implementation");
        fprintf(stderr,"\nDeCompressor for 256 Base");
        fprintf(stderr,"\nProvide Input File name and Out put File Name");
        return 0;
    }

    if((gIN=fopen(argv[1],"rb"))==NULL)
    {
        fprintf(stderr,"\nError Opening Input File");
        return 0;
    }

    if((out=fopen(argv[2],"wb"))==NULL)
    {
        fprintf(stderr,"\nError Opening Output File");
        return 0;
    }


    fprintf(stderr,"\nA Lempel Ziv 77 and Adaptive Huffman FGK Algorithm's DeCompressor");
    fprintf(stderr,"\nWriten and Modified by Anim");
    fprintf(stderr,"\nWindow Buffer Size    : %151lu bytes",(ulong)WIN_BUFFSIZE);
    fprintf(stderr,"\nLook Ahead Buffer Size: %151lu bytes",(ulong)PAT_BUFFSIZE);
    fprintf(stderr,"\nName of the Input File: %s",argv[1]);

    clock_t begin=clock();

    fread(&fstamp,sizeof(fstamp),1,gIN);
    init_getBuffer();

    fprintf(stderr,"\nDecompressing...");

    //Initializing Sliding Window
    memset(winBuf,0,WIN_BUFFSIZE);

    //Adaptive Huffman Initialization
    hmin=0;
    hmax_symbols=PAT_BUFFSIZE-2;
    huffmanSymbol_bitSize=NUM_LEN_BITS;
    adaptiveHuffman_initFirstNode(hmin);

    //Starting Decompression
    fsize=fstamp.fileSize;
    while(fsize)
    {
        if(get_bit()==1)
        {
            k=(uchar)adaptiveHuffman_decodeSymbol();
            k+=3;
            pos.length=k;

            //Getting Window Position
            pos.position=get_nbits(NUM_POS_BITS);

            i=pos.length;
            while(i--)
            {
                pattern[i]=winBuf[(pos.position+i)&WIN_MASK];
            }
            i=pos.length;
            while(i--)
            {
                winBuf[(win_cnt+i)&WIN_MASK]=pattern[i];
            }

            //Output String
            fwrite(pattern,pos.length,1,out);
            fsize-=pos.length;
            win_cnt=((win_cnt+pos.length)&WIN_MASK);
        }
        else
        {
            switch(get_bit())
            {
            case 0:
                //Getting Byte to Output it
                k=get_nbits(8);
                fputc((uchar)k,out);
                fsize--;

                //Updating Window Buffer
                winBuf[win_cnt]=(uchar)k;
                if((++win_cnt)==WIN_BUFFSIZE)
                {
                    win_cnt=0;
                }
                break;

            case 1:
                pos.position=get_nbits(NUM_POS_BITS);
                pos.length=2;
                i=pos.length;
                while(i--)
                {
                    pattern[i]=winBuf[(pos.position+i)&WIN_MASK];
                }
                i=pos.length;
                while(i--)
                {
                    winBuf[(win_cnt+i)&WIN_MASK]=pattern[i];
                }

                //Output String
                fwrite(pattern,pos.length,1,out);
                fsize-=pos.length;
                win_cnt=((win_cnt+pos.length)&WIN_MASK);
                break;
            }
        }
    }
    clock_t end=clock();
    double timeTook=(double)(end-begin)/CLOCKS_PER_SEC;

    fprintf(stderr,"\nComplete.");
    fprintf(stderr,"\nName of the Output File : %s",argv[2]);
    fprintf(stderr,"\nTime Took to DeCompress That File Using Lempel Ziv 77 and Adaptive Huffman Based on FGK Algorithm %lf Seconds\n",timeTook);

	halt_prog:

    free_getBuffer();
    if(gIN)
    {
        fclose(gIN);
    }
    if(out)
    {
        fclose(out);
    }
    return 0;

}
