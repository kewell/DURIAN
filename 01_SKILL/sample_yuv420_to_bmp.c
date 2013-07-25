/*********************************************************************************
 *      Copyright:  (C) 2013 KEWELL
 *
 *       Filename:  sample_yuv420_to_bmp.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(07/25/13~)
 *         Author:  WENJING <WJ@HIGHEASYRD.COM WENJING0101@GMAIL.COM>
 *      ChangeLog:  1, Release initial version on "07/25/13 16:47:20"
 *                  2,
 *                 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 ********************************************************************************/
int sample_yuv420_to_bmp (void *hChannelHandle, unsigned char *pBuf, unsigned int *realtime, unsigned int *size)
{
    unsigned char *pImage;
    long uvPos = 0;
    long yPos = 0;
    long Width = 704;
    long hsize;
    int y, u, v, r, g, b;
    int i, j, rgbpos;
    int DecodeImageW;
    int DecodeImageH;
    unsigned char *yrec, *urec, *vrec;
    unsigned char bmpHeader[] =
        { 0x42, 0x4D, 0x36, 0x90, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0xC0, 0x02, 0x00, 0x00, 0x40,
        0x02, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x12, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    char *DecodeDest;
    int rgbY, rV, bU, gU, gV;
    int *pH, *pV;
    VIDEO_FRAME_INFO_S stFrame;
    HI_U32 u32PhyAddr, destStride = 0;
    HI_U8 *pVirAddr;
    HI_U32 u32FrmSize;
    HI_S32 u32Height, u32Stride;
    CDecChannel_t *pDecCh;
    int S32ret;
    int VPSSCH = 4;

    if (g_Vodev_playback == 0)
        VPSSCH = 2;
    else
        VPSSCH = 4;

    if (IsDecHandleOk(hChannelHandle))
        return -1;

    pDecCh = (CDecChannel_t *) hChannelHandle;

    if (!pDecCh->m_bDecChannelOpen)
    {
        return -1;
    }

#if 1
    VPSS_CHN_MODE_S stVpssMode;
    S32ret = HI_MPI_VPSS_GetChnMode(pDecCh->m_nPort+16, VPSSCH, &stVpssMode);
    if (HI_SUCCESS != S32ret)
    {
        SDKERROR("failed ret = %#x\n",S32ret);
    }
    stVpssMode.enChnMode = VPSS_CHN_MODE_USER;
    stVpssMode.bDouble = HI_FALSE;
    stVpssMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stVpssMode.u32Height = 576;
    stVpssMode.u32Width = CAP_WIDTH;
    S32ret = HI_MPI_VPSS_SetChnMode(pDecCh->m_nPort+16, VPSSCH, &stVpssMode);
    if (HI_SUCCESS != S32ret)
    {
        SDKINFO_INT(stVpssMode.bDouble);
        SDKINFO_INT(stVpssMode.enChnMode);
        SDKINFO_INT(stVpssMode.enPixelFormat);
        SDKINFO_INT(stVpssMode.u32Height);
        SDKINFO_INT(stVpssMode.u32Width);
        SDKINFO("pDecCh->m_nPort+16 = %d\n",pDecCh->m_nPort+16);
        SDKERROR("failed ret = %#x\n",S32ret);
    }
#endif

#define GET_IMAGE_FROM_VDEC

#ifdef GET_IMAGE_FROM_VDEC
    if (Higheasy_MPI_VPSS_GetChnFrame(pDecCh->m_nPort,VPSSCH , &stFrame))
#else
    if (Higheasy_MPI_VO_GetChnFrame(pDecCh->m_nPort, &stFrame))
#endif
    {
        stVpssMode.enChnMode = VPSS_CHN_MODE_AUTO;
        S32ret = HI_MPI_VPSS_SetChnMode(pDecCh->m_nPort+16,VPSSCH , &stVpssMode);
        if (HI_SUCCESS != S32ret)
        {
            SDKINFO("pDecCh->m_nPort+16 = %d\n",pDecCh->m_nPort+16);
            SDKERROR("failed ret = %#x\n",S32ret);
        }
        SDKERROR("HI_MPI_VO_GetChnFrame failed \n");
        return -1;
    }

    stVpssMode.enChnMode = VPSS_CHN_MODE_AUTO;
    S32ret = HI_MPI_VPSS_SetChnMode(pDecCh->m_nPort+16,VPSSCH , &stVpssMode);
    u32Stride = stFrame.stVFrame.u32Stride[0];
    u32Height = stFrame.stVFrame.u32Height;
    Width = stFrame.stVFrame.u32Width;
    destStride = Width;

    SDKINFO_INT(u32Height );
    SDKINFO_INT(Width  );

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == stFrame.stVFrame.enPixelFormat)
        u32FrmSize = (u32Stride * u32Height * 3) / 2;
    else
        u32FrmSize = u32Stride * u32Height * 2;

    u32PhyAddr = stFrame.stVFrame.u32PhyAddr[0];
    pVirAddr = (HI_U8 *) Higheasy_memmap(u32PhyAddr, &u32FrmSize);


    if (NULL == pVirAddr)
    {
#ifdef GET_IMAGE_FROM_VDEC
        Higheasy_MPI_VPSS_ReleaseChnFrame(pDecCh->m_nPort,VPSSCH , &stFrame);
#else
        Higheasy_MPI_VO_ReleaseChnFrame(pDecCh->m_nPort, &stFrame);
#endif
#if 1
    if (HI_SUCCESS != S32ret)
    {
        SDKINFO("pDecCh->m_nPort+16 = %d\n",pDecCh->m_nPort+16);
        SDKERROR("failed ret = %#x\n",S32ret);
    }
#endif
        return -1;
    }

    for (i = 0; i < u32Height; i++)
    {
        memcpy(dec_snap_buffer + destStride * i, pVirAddr + u32Stride * i, destStride);
    }
    for (i = 0; i < u32Height / 2; i++)
    {
        memcpy(dec_snap_buffer + destStride * u32Height + destStride * i, pVirAddr + u32Stride * u32Height + u32Stride * i, destStride);
    }

    Higheasy_memunmap((HI_U8 *) ((HI_U32) pVirAddr & 0xFFFFF000), u32FrmSize);


#ifdef GET_IMAGE_FROM_VDEC
        Higheasy_MPI_VPSS_ReleaseChnFrame(pDecCh->m_nPort,VPSSCH , &stFrame);
#else
    Higheasy_MPI_VO_ReleaseChnFrame(pDecCh->m_nPort, &stFrame);
#endif

    pV = (int *) (bmpHeader + 22);
    *pV = u32Height;

    pH = (int *) (bmpHeader + 18);
    *pH = Width;

    pImage = dec_snap_buffer;
    hsize = Width * u32Height;
    DecodeImageW = Width;
    DecodeImageH = u32Height;
    yrec = pImage;
    urec = pImage + hsize + 1;
    vrec = pImage + hsize;
    if (*size < (Width * u32Height + 54))
    {
        SDKERROR("size is too small\n");
        return -1;
    }

    DecodeDest = pBuf;
    *(int *) (bmpHeader + 2) = Width * u32Height * 3 + 54;
    *(int *) (bmpHeader + 34) = Width * u32Height * 3;
    memcpy(DecodeDest, &bmpHeader, 54);
    DecodeDest += 54;
    for (i = 0; i < u32Height; i += 2)
    {
        for (j = 0; j < Width; j += 2)
        {
            yPos = i * DecodeImageW + j;
            uvPos = i * DecodeImageW / 2 + j;
            rgbpos = (u32Height - i - 1) * Width + j;
            y = *(yrec + yPos) - 0x10;
            if (y < 0)
                y = 0;
            u = urec[uvPos] - 0x80;
            v = vrec[uvPos] - 0x80;
            rgbY = 1216348 * y;
            rV = 1640707 * v;
            bU = 2025744 * u;
            gU = 406743 * u;
            gV = 823132 * v;
            r = (rgbY + rV) >> 20;
            g = (rgbY - gU - gV) >> 20;
            b = (rgbY + bU) >> 20;

            r = r < 0 ? 0 : (r > 255 ? 255 : r);
            g = g < 0 ? 0 : (g > 255 ? 255 : g);
            b = b < 0 ? 0 : (b > 255 ? 255 : b);
            *(DecodeDest + rgbpos * 3) = b;
            *(DecodeDest + rgbpos * 3 + 1) = g;
            *(DecodeDest + rgbpos * 3 + 2) = r;

            yPos++;
            y = *(yrec + yPos) - 0x10;
            if (y < 0)
                y = 0;
            rgbY = 1216348 * y;
            r = (rgbY + rV) >> 20;
            g = (rgbY - gU - gV) >> 20;
            b = (rgbY + bU) >> 20;

            r = r < 0 ? 0 : (r > 255 ? 255 : r);
            g = g < 0 ? 0 : (g > 255 ? 255 : g);
            b = b < 0 ? 0 : (b > 255 ? 255 : b);
            *(DecodeDest + (rgbpos + 1) * 3) = b;
            *(DecodeDest + (rgbpos + 1) * 3 + 1) = g;
            *(DecodeDest + (rgbpos + 1) * 3 + 2) = r;

            yPos = (i + 1) * DecodeImageW + j;
            rgbpos = (u32Height - (i + 1) - 1) * Width + j;
            y = *(yrec + yPos) - 0x10;
            if (y < 0)
                y = 0;
            rgbY = 1216348 * y;
            r = (rgbY + rV) >> 20;
            g = (rgbY - gU - gV) >> 20;
            b = (rgbY + bU) >> 20;

            r = r < 0 ? 0 : (r > 255 ? 255 : r);
            g = g < 0 ? 0 : (g > 255 ? 255 : g);
            b = b < 0 ? 0 : (b > 255 ? 255 : b);
            *(DecodeDest + rgbpos * 3) = b;
            *(DecodeDest + rgbpos * 3 + 1) = g;
            *(DecodeDest + rgbpos * 3 + 2) = r;

            yPos++;
            y = *(yrec + yPos) - 0x10;
            if (y < 0)
                y = 0;
            rgbY = 1216348 * y;
            r = (rgbY + rV) >> 20;
            g = (rgbY - gU - gV) >> 20;
            b = (rgbY + bU) >> 20;

            r = r < 0 ? 0 : (r > 255 ? 255 : r);
            g = g < 0 ? 0 : (g > 255 ? 255 : g);
            b = b < 0 ? 0 : (b > 255 ? 255 : b);
            *(DecodeDest + (rgbpos + 1) * 3) = b;
            *(DecodeDest + (rgbpos + 1) * 3 + 1) = g;
            *(DecodeDest + (rgbpos + 1) * 3 + 2) = r;
        }
    }


    *size = hsize * 3 + 54;

    *realtime = MP4Sys_DecGetVoShowTime(hChannelHandle);

    return 0;
}


