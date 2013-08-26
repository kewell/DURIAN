/*********************************************************************************
 *      Copyright:  (C) 2013 KEWELL
 *
 *       Filename:  common_drv_tools.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(06/19/13~)
 *         Author:  WENJING <WJ@HIGHEASYRD.COM WENJING0101@GMAIL.COM>
 *      ChangeLog:  1, Release initial version on "06/19/13 16:09:13"
 *                  2,
 *                 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/**************************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 *************************************************************************************/
int usage (char *appName)
{
    printf("usage : \n");
    printf("%s DEV_NAME CMD ARGs\n\n", appName);
    printf("ext   :\n");
    printf("%s /dev/hi_gpio 0x270 0\n", appName);
    printf("%s /dev/hi_gpio 0x263 1\n", appName);

    return 0;
} /* ----- End of usage()  ----- */

int main (int argc, char **argv)
{
    int fd = -1;
    int iCmd = -1;
    char * pcArgs;
    unsigned char ucArg = 0;
    int iRet  = -1;

    if (3 >= argc)
    {
        usage(argv[0]);
        return -1;
    }

    fd = open (argv[1], O_RDWR);
    
    if (0 > fd)
    {
        printf("open %s failed\n", argv[1]);
        return -1;
    }
    
    iCmd = atoi(argv[2]);
    pcArgs = argv[3];
    ucArg = (unsigned char)atoi(argv[3]);

    //printf("ioctl(%s, 0x%x, %s)", argv[1], iCmd, pcArgs);

    //iRet = ioctl(fd, iCmd, pcArgs);
    iRet = ioctl(fd, iCmd, &ucArg);
    {
        //printf(" = %d\n", iRet);
    }

    close(fd);
    
    return iRet;
} /* ----- End of main() ----- */

