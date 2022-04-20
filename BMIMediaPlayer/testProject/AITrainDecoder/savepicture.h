#ifndef SAVEPICTURE_H
#define SAVEPICTURE_H


class SavePicture
{
public:
    SavePicture();
    ~SavePicture();
    int yuv420sp_to_jpg(const char *filename, int width, int height, unsigned char *pYUVBuffer);
    int yuv420p_to_jpeg(const char *filename, unsigned char *pdata, int image_width, int image_height);
    int rgb_to_jpeg(const char * filename, unsigned char* rgbData,int image_width,int image_height,int quality);
    int yuv420p_to_yuv420sp(unsigned char * yuv420p,unsigned char* yuv420sp,int width,int height);
};

#endif // SAVEPICTURE_H
