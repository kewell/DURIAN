/*********************************************************************************
 *      Copyright:  (C) 2013 KEWELL
 *
 *       Filename:  force_panic.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(06/21/13~)
 *         Author:  WENJING <WJ@HIGHEASYRD.COM WENJING0101@GMAIL.COM>
 *      ChangeLog:  1, Release initial version on "06/21/13 16:05:16"
 *                  2,
 *                 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 ********************************************************************************/
#ifdef __KERNEL__

#include <linux/module.h>
#include <linux/kernel.h>

static int __init panic_init(void)
{
    panic("force-panic");
    printk("...... ...... CAN U YOU ME ...... ......\n");
    printk("...... ...... CAN U YOU ME ...... ......\n");
    printk("...... ...... CAN U YOU ME ...... ......\n");
    printk("...... ...... CAN U YOU ME ...... ......\n");
    printk("...... ...... CAN U YOU ME ...... ......\n");
    printk("...... ...... CAN U YOU ME ...... ......\n");
    return 0;
}

static void __exit panic_exit(void)
{
}

module_init(panic_init);
module_exit(panic_exit);

#endif
