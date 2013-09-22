/*********************************************************************************
 *      Copyright:  (C) 2013 KEWELL
 *
 *       Filename:  bmp2jpeg.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(08/09/13~)
 *         Author:  WENJING <WJ@HIGHEASYRD.COM WENJING0101@GMAIL.COM>
 *      ChangeLog:  1, Release initial version on "08/09/13 17:28:03"
 *                  2,
 *                 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 ********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "jpeglib.h"
//#pragma comment(lib,"jpeg.lib")
//using namespace std;

#pragma pack(2)        //两字节对齐，否则bmp_fileheader会占16Byte
struct bmp_fileheader
{
    unsigned short    bfType;        //若不对齐，这个会占4Byte
    unsigned long    bfSize;
    unsigned short    bfReverved1;
    unsigned short    bfReverved2;
    unsigned long    bfOffBits;
};

struct bmp_infoheader
{
    unsigned long    biSize;
    unsigned long    biWidth;
    unsigned long    biHeight;
    unsigned short    biPlanes;
    unsigned short    biBitCount;
    unsigned long    biCompression;
    unsigned long    biSizeImage;
    unsigned long    biXPelsPerMeter;
    unsigned long    biYpelsPerMeter;
    unsigned long    biClrUsed;
    unsigned long    biClrImportant;
};

FILE *input_file;
FILE *output_file;

struct bmp_fileheader bfh;
struct bmp_infoheader bih;

unsigned char *src_buffer;
unsigned char *dst_buffer;

void read_bmp_header()
{    
    fread(&bfh,sizeof(struct bmp_fileheader),1,input_file);
    fread(&bih,sizeof(struct bmp_infoheader),1,input_file);
}

void read_bmp_data()
{
    fseek(input_file,bfh.bfOffBits,SEEK_SET);
    src_buffer=(unsigned char*)malloc(bih.biWidth*bih.biHeight*bih.biBitCount/8);
    fread(src_buffer,sizeof(unsigned char)*bih.biWidth*bih.biHeight*bih.biBitCount/8,1,input_file);

    unsigned long width=bih.biWidth;
    unsigned long height=bih.biHeight;
    unsigned short depth=(unsigned short)(bih.biBitCount/8);
    unsigned char *src_point;
    unsigned char *dst_point;

    dst_buffer=(unsigned char*)malloc(width*height*depth);
    src_point=src_buffer+width*depth*(height-1);
    dst_point=dst_buffer+width*depth*(height-1);
    unsigned long i = 0,j = 0;
    for (i=0;i<height;i++)
    {
        for (j=0;j<width*depth;j+=depth)
        {
            if (depth==1)        //处理灰度图
            {
                dst_point[j]=src_point[j];
            }

            if (depth==3)        //处理彩色图
            {
                dst_point[j+2]=src_point[j+0];
                dst_point[j+1]=src_point[j+1];
                dst_point[j+0]=src_point[j+2];
            }
        }
        dst_point-=width*depth;
        src_point-=width*depth;
    }
}

void synthese_jpeg()
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;

    unsigned long width=bih.biWidth;
    unsigned long height=bih.biHeight;
    unsigned short depth=(unsigned short)(bih.biBitCount/8);
    unsigned char *point;

    cinfo.err=jpeg_std_error(&jerr);        //libjpeg各种配置
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo,output_file);

    cinfo.image_width=width;
    cinfo.image_height=height;
    cinfo.input_components=depth;
    if (depth==1)
        cinfo.in_color_space=JCS_GRAYSCALE;
    else
        cinfo.in_color_space=JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo,20,TRUE);    //中间的值为压缩质量，越大质量越好
    jpeg_start_compress(&cinfo,TRUE);

    buffer=(*cinfo.mem->alloc_sarray)
            ((j_common_ptr)&cinfo,JPOOL_IMAGE,width*depth,1);

    point=dst_buffer+width*depth*(height-1);
    while (cinfo.next_scanline<height)
    {
        memcpy(*buffer,point,width*depth);
        jpeg_write_scanlines(&cinfo,buffer,1);
        point-=width*depth;
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}

int main()
{
    input_file=fopen("1.bmp","rb");
    output_file=fopen("1.jpg","wb");

    read_bmp_header();
    read_bmp_data();

    synthese_jpeg();

    fclose(input_file);
    fclose(output_file);

    free(src_buffer);
    free(dst_buffer);

    //cout<<"good job."<<endl;
    //cin.get();
    return 0;
}
