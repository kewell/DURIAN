/*********************************************************************************
 *      Copyright:  (C) 2013 KEWELL
 *
 *       Filename:  drv_skeleton.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(06/20/13~)
 *         Author:  WENJING <WJ@HIGHEASYRD.COM WENJING0101@GMAIL.COM>
 *      ChangeLog:  1, Release initial version on "06/20/13 11:20:24"
 *                  2,
 *                 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 ********************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

/*
    Name    String  Meaning alias function
    KERN_EMERG  "<0>"   Emergency messages, system is about to crash or is unstable pr_emerg
    KERN_ALERT  "<1>"   Something bad happened and action must be taken immediately pr_alert
    KERN_CRIT   "<2>"   A critical condition occurred like a serious hardware/software failure  pr_crit
    KERN_ERR     "<3>"  An error condition, often used by drivers to indicate difficulties with the hardware    pr_err
    KERN_WARNING    "<4>"   A warning, meaning nothing serious by itself but might indicate problems    pr_warning
    KERN_NOTICE "<5>"   Nothing serious, but notably nevertheless. Often used to report security events.    pr_notice
    KERN_INFO   "<6>"   Informational message e.g. startup information at driver initialization pr_info
    KERN_DEBUG  "<7>"   Debug messages  pr_debug, pr_devel if DEBUG is defined
    KERN_CONT   "<c>"   "continued" line of log printout (only done after a line that had no enclosing \n) [1]  pr_cont

     cat /proc/sys/kernel/printk
        7       4       1       7
    current default minimum boot-time-default
*/

static int hello_init(void)
{
    //printk(KERN_EMERG"Hello, world\n");
    printk(KERN_CONT"CONT\n");
    printk(KERN_DEBUG"DEBUG\n");
    printk(KERN_DEBUG"%s() %d\n", __func__, __LINE__);
    printk(KERN_INFO"INFO\n");
    printk(KERN_NOTICE"NOTICE\n");
    printk(KERN_WARNING"WARNING\n");
    printk(KERN_ERR"ERR\n");
    printk(KERN_CRIT"CRIT\n");
    printk(KERN_ALERT"ALERT\n");
    printk(KERN_EMERG"EMERG\n");
    printk("__NONE__\n");

    return 0;
}

static void hello_exit(void)
{
    //printk(KERN_EMERG"Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);        


