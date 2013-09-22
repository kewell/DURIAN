// Conversion from RGB to YUV420
int RGB2YUV_YR[256], RGB2YUV_YG[256], RGB2YUV_YB[256];
int RGB2YUV_UR[256], RGB2YUV_UG[256], RGB2YUV_UBVR[256];
int RGB2YUV_VG[256], RGB2YUV_VB[256];
// Conversion from YUV420 to RGB24
static long int crv_tab[256];
static long int cbu_tab[256];
static long int cgu_tab[256];
static long int cgv_tab[256];
static long int tab_76309[256];
static unsigned char clp[1024]; //for clip in CCIR601
//
// Table used for RGB to YUV420 conversion
//
void InitLookupTable()
{
    int i;
    for (i = 0; i < 256; i++) RGB2YUV_YR[i] = (float)65.481 * (i<<8);
    for (i = 0; i < 256; i++) RGB2YUV_YG[i] = (float)128.553 * (i<<8);
    for (i = 0; i < 256; i++) RGB2YUV_YB[i] = (float)24.966 * (i<<8);
    for (i = 0; i < 256; i++) RGB2YUV_UR[i] = (float)37.797 * (i<<8);
    for (i = 0; i < 256; i++) RGB2YUV_UG[i] = (float)74.203 * (i<<8);
    for (i = 0; i < 256; i++) RGB2YUV_VG[i] = (float)93.786 * (i<<8);
    for (i = 0; i < 256; i++) RGB2YUV_VB[i] = (float)18.214 * (i<<8);
    for (i = 0; i < 256; i++) RGB2YUV_UBVR[i] = (float)112 * (i<<8);
}
int ConvertRGB2YUV(int w,int h,unsigned char *bmp,unsigned int *yuv)
{
    unsigned int *u,*v,*y,*uu,*vv;
    unsigned int *pu1,*pu2,*pu3,*pu4;
    unsigned int *pv1,*pv2,*pv3,*pv4;
    unsigned char *r,*g,*b;
    int i,j;
    uu=new unsigned int[w*h];
    vv=new unsigned int[w*h];
    if(uu==NULL || vv==NULL)
        return 0;
    y=yuv;
    u=uu;
    v=vv;
    // Get r,g,b pointers from bmp image data....
    r=bmp;
    g=bmp+1;
    b=bmp+2;
    //Get YUV values for rgb values...
    for(i=0;i<h;i++)
    {
        for(j=0;j<w;j++)
        {
            *y++=( RGB2YUV_YR[*r] +RGB2YUV_YG[*g]+RGB2YUV_YB[*b]+1048576)>>16;
            *u++=(-RGB2YUV_UR[*r] -RGB2YUV_UG[*g]+RGB2YUV_UBVR[*b]+8388608)>>16;
            *v++=( RGB2YUV_UBVR[*r]-RGB2YUV_VG[*g]-RGB2YUV_VB[*b]+8388608)>>16;
            r+=3;
            g+=3;
            b+=3;
        }
    }
    // Now sample the U & V to obtain YUV 4:2:0 format
    // Sampling mechanism...
    /* @ -> Y
# -> U or V
@ @ @ @
# #
@ @ @ @
@ @ @ @
# #
@ @ @ @
*/
    // Get the right pointers...
    u=yuv+w*h;
    v=u+(w*h)/4;
    // For U
    pu1=uu;
    pu2=pu1+1;
    pu3=pu1+w;
    pu4=pu3+1;
    // For V
    pv1=vv;
    pv2=pv1+1;
    pv3=pv1+w;
    pv4=pv3+1;
    // Do sampling....
    for(i=0;i<h;i+=2)
    {
        for(j=0;j<w;j+=2)
        {
            *u++=(*pu1+*pu2+*pu3+*pu4)>>2;
            *v++=(*pv1+*pv2+*pv3+*pv4)>>2;
            pu1+=2;
            pu2+=2;
            pu3+=2;
            pu4+=2;
            pv1+=2;
            pv2+=2;
            pv3+=2;
            pv4+=2;
        }
        pu1+=w;
        pu2+=w;
        pu3+=w;
        pu4+=w;
        pv1+=w;
        pv2+=w;
        pv3+=w;
        pv4+=w;
    }
    delete uu;
    delete vv;
    return 1;
}
//
//Initialize conversion table for YUV420 to RGB
//
void InitConvertTable()
{
    long int crv,cbu,cgu,cgv;
    int i,ind;
    crv = 104597; cbu = 132201; /* fra matrise i global.h */
    cgu = 25675; cgv = 53279;
    for (i = 0; i < 256; i++) {
        crv_tab[i] = (i-128) * crv;
        cbu_tab[i] = (i-128) * cbu;
        cgu_tab[i] = (i-128) * cgu;
        cgv_tab[i] = (i-128) * cgv;
        tab_76309[i] = 76309*(i-16);
    }
    for (i=0; i<384; i++)
        clp[i] =0;
    ind=384;
    for (i=0;i<256; i++)
        clp[ind++]=i;
    ind=640;
    for (i=0;i<384;i++)
        clp[ind++]=255;
}
