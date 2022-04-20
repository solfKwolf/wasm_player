#include "savepicture.h"
#include "jpeglib.h"
#include <iostream>
#include <string.h>

SavePicture::SavePicture()
{

}

SavePicture::~SavePicture()
{

}



int SavePicture::yuv420sp_to_jpg(const char *filename, int width, int height, unsigned char *pYUVBuffer)
{
    FILE *fJpg;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    int i = 0, j = 0;
    unsigned char yuvbuf[width * 3];
    unsigned char *pY, *pU, *pV;
    int ulen;

    ulen = width * height / 4;

    if(pYUVBuffer == NULL){
        std::cout<<("pBGRBuffer is NULL!\n");
        return -1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    fJpg = fopen(filename, "wb");
    if(fJpg == NULL){
        std::cout<<"Cannot open file "<< filename<<","<< errno<<std::endl;
        jpeg_destroy_compress(&cinfo);
        return -1;
    }

    jpeg_stdio_dest(&cinfo, fJpg);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_YCbCr;
    cinfo.dct_method = JDCT_ISLOW;
    jpeg_set_defaults(&cinfo);


    jpeg_set_quality(&cinfo, 99, TRUE);

    jpeg_start_compress(&cinfo, TRUE);
    row_stride = cinfo.image_width * 3; /* JSAMPLEs per row in image_buffer */

    pY = pYUVBuffer;
    pU = pYUVBuffer + width*height;
    pV = pYUVBuffer + width*height + ulen;
    j = 1;
    while (cinfo.next_scanline < cinfo.image_height) {
        if(j % 2 == 1 && j > 1){
            pU = pYUVBuffer + width*height + width / 2 * (j / 2);
            pV = pYUVBuffer + width*height * 5 / 4 + width / 2 *(j / 2);
        }
        for(i = 0; i < width; i += 2){
            yuvbuf[i*3] = *pY++;
            yuvbuf[i*3 + 1] = *pU;
            yuvbuf[i*3 + 2] = *pV;

            yuvbuf[i*3 + 3] = *pY++;
            yuvbuf[i*3 + 4] = *pU++;
            yuvbuf[i*3 + 5] = *pV++;
        }

        row_pointer[0] = yuvbuf;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
        j++;
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fJpg);
    return 0;
}

int SavePicture::yuv420p_to_jpeg(const char *filename, unsigned char *pdata, int image_width, int image_height)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE * outfile;    // target file
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = image_width;  // image width and height, in pixels
    cinfo.image_height = image_height;
    cinfo.input_components = 3;    // # of color components per pixel
    cinfo.in_color_space = JCS_YCbCr;  //colorspace of input image
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE );

    //////////////////////////////
    //  cinfo.raw_data_in = TRUE;
    cinfo.jpeg_color_space = JCS_YCbCr;
    cinfo.comp_info[0].h_samp_factor = 2;
    cinfo.comp_info[0].v_samp_factor = 2;
    /////////////////////////

    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];

    unsigned char *yuvbuf;
    if((yuvbuf=(unsigned char *)malloc(image_width*3))!=NULL)
        memset(yuvbuf,0,image_width*3);

    unsigned char *ybase,*ubase;
    ybase=pdata;
    ubase=pdata+image_width*image_height;
    int j=0;
    while (cinfo.next_scanline < cinfo.image_height)
    {
        int idx=0;
    int i;
        for(i=0;i<image_width;i++)
        {
            yuvbuf[idx++]=ybase[i + j * image_width];
            yuvbuf[idx++]=ubase[j/2 * image_width+(i/2)*2];
            yuvbuf[idx++]=ubase[j/2 * image_width+(i/2)*2+1];
        }
        row_pointer[0] = yuvbuf;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
        j++;
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
    free(yuvbuf);
    return 0;
}

int SavePicture::rgb_to_jpeg(const char * filename, unsigned char* rgbData,int image_width,int image_height,int quality)
{
    struct jpeg_compress_struct jpeg;  //identify a compress object
    struct jpeg_error_mgr jerr;  //error information

    jpeg.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&jpeg);  //init compress object

    FILE* pFile;

    pFile = fopen(filename, "wb");
    if( !pFile )  return 0;
    jpeg_stdio_dest(&jpeg, pFile);

    //compress param set,i just did a simple param set
    jpeg.client_data=(void*)&pFile;
    jpeg.image_width = image_width;
    jpeg.image_height = image_height;
    jpeg.input_components  = 3;
    jpeg.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jpeg);
    //// 指定亮度及色度质量
    jpeg.q_scale_factor[0] = jpeg_quality_scaling(100);
    jpeg.q_scale_factor[1] = jpeg_quality_scaling(100);
    //// 图像采样率，默认为2 * 2
    jpeg.comp_info[0].v_samp_factor = 2;
    jpeg.comp_info[0].h_samp_factor = 2;
    //// set jpeg compress quality
    jpeg_set_quality(&jpeg, quality, TRUE);  //100 is the highest

    //start compress
    jpeg_start_compress(&jpeg, TRUE);

    JSAMPROW row_pointer[1];

    //from up to down ,set every pixel
    for( unsigned int i=0;i<jpeg.image_height;i++ )
    {
        row_pointer[0] = rgbData+i*jpeg.image_width*3;
        jpeg_write_scanlines( &jpeg,row_pointer,1 );
    }
    //stop compress
    jpeg_finish_compress(&jpeg);

    fclose( pFile );
    pFile = NULL;
    jpeg_destroy_compress(&jpeg);
    return 0;
}

int SavePicture::yuv420p_to_yuv420sp(unsigned char *yuv420p, unsigned char *yuv420sp, int width, int height)
{
    if(yuv420p==NULL)
        return 0;
    int i=0,j=0;
    //Y
    for(i=0;i<width*height;i++)
    {
        yuv420sp[i]=yuv420p[i];
    }

    int m=0,n=0;
    for(j=0;j<width*height/2;j++)
    {
        if(j%2==0)
        {
            yuv420sp[j+width*height]=yuv420p[width*height+m];
            m++;
        }
        else
        {
            yuv420sp[j+width*height]=yuv420p[width*height*5/4+n];
            n++;
        }
    }
}

