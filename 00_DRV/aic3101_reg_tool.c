#include <sys/timeb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/resource.h>

#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
	
enum
{
	AIC3101_REG_0  = 0x0180,
	AIC3101_REG_1  = 0x6501,
	AIC3101_REG_2  = 0x0700,
	AIC3101_REG_3  = 0x0381, //PLL Programming Register A
	AIC3101_REG_4  = 0x0420, //PLL Programming Register B
	AIC3101_REG_5  = 0x051e,
	AIC3101_REG_6  = 0x0600,
	AIC3101_REG_7  = 0x02aa, //Codec Sample Rate Select Register
	AIC3101_REG_8  = 0x08C0, //Audio Serial Data Interface Control Register A
	//
	AIC3101_REG_9  = 0x0907, //Audio Serial Data Interface Control Register B
	AIC3101_REG_A  = 0x1304, //MIC2/LINE2 to Right-ADC Control Register
	AIC3101_REG_B  = 0x0F00, //Left-ADC PGA Gain Control Register
	AIC3101_REG_C  = 0x1800, //MIC1LP/LINE1LP to Right-ADC Control Register
	AIC3101_REG_D  = 0x167C, //MIC1RP/LINE1RP to Right-ADC Control Register
	AIC3101_REG_E  = 0x1000, //Right-ADC PGA Gain Control Register

    AIC3101_REG_F  = 0x0c00, //Audio Codec Digital Filter Control Register
};

int g_fd_gpio_iic = -1;
    
int gpio_ioctl(int ioct_value, void *pValue)
{
    return ioctl(g_fd_gpio_iic, ioct_value, pValue);
}

#define GPIO_I2C_READ     0x115
#define GPIO_I2C_WRITE   0x116

#define WRITE_AIC3101(regval) \
		gpio_ioctl(GPIO_I2C_WRITE, (void *)((0x30<<24)|((regval>>8)<<16)|(regval & 0xff)))

#define READ_AIC3101(regval) \
		gpio_ioctl(GPIO_I2C_READ, (void *)((0x30<<24)|((regval>>8)<<16)|(regval & 0xff)))

#define GPIO_IIC_FILE   "/dev/hi_gpio"

#define Write_1_Bit(reg, val) \
    gpio_ioctl(GPIO_I2C_WRITE, (void *)((0x30 << 24) | (reg << 16) | val))

#define Read_1_Bit(reg) \
		gpio_ioctl(GPIO_I2C_READ, (void *)((0x30 << 24) | (reg << 16)))

void usage(char *appName)
{
    printf("%s : %s [Reg]                     ---> Read  Reg\nor\n", __func__, appName);
    printf("%s : %s [Reg] [MSB_Val] [LSB_Val] ---> Write Reg, then read\n", __func__, appName);
}

int main2(int argc, char ** argv)
{
    int s32Ret = -1;
    g_fd_gpio_iic = open(GPIO_IIC_FILE, O_RDWR);
    if (g_fd_gpio_iic < 0)
    {
        printf("can't open g_fd_gpio_iic\n");
        return -1;
    }

	int bAIC3101 = 0;
	sleep(1);
	WRITE_AIC3101(AIC3101_REG_0);		// 设备reset
	sleep(1);
	WRITE_AIC3101(AIC3101_REG_1);		// CODEC动作时钟源选择为Q分频器
	bAIC3101 = ((READ_AIC3101(AIC3101_REG_1) & 0xFF) == (AIC3101_REG_1 & 0xFF) ? 1 : 0);

    if (1 == bAIC3101)
	{
        s32Ret = 0;
		//sleep(1);
		WRITE_AIC3101(AIC3101_REG_0);		// 设备reset
		//sleep(1);
		//WRITE_AIC3101(AIC3101_REG_1);		// CODEC动作时钟源选择为Q分频器
		//WRITE_AIC3101(AIC3101_REG_2);	
		//WRITE_AIC3101(AIC3101_REG_3);
		//WRITE_AIC3101(AIC3101_REG_4);
		//WRITE_AIC3101(AIC3101_REG_5);
		//WRITE_AIC3101(AIC3101_REG_6);
		//WRITE_AIC3101(AIC3101_REG_7);
		
		WRITE_AIC3101(AIC3101_REG_8);		// 设置bclk、wclk为master mode
		WRITE_AIC3101(AIC3101_REG_9);		// 消除由于数据(DATA)输入不同步而产生的杂音现象
		sleep(1);
		WRITE_AIC3101(AIC3101_REG_A);		// 设置MIC1LP为single-ended,并将其连接至L-ADC，最后power up L-ADC
		WRITE_AIC3101(AIC3101_REG_B);		// 使L-ADC非静音
		WRITE_AIC3101(AIC3101_REG_C);		// 连接MIC1LP到R-ADC
		WRITE_AIC3101(AIC3101_REG_D);		// Power up R-ADC
		WRITE_AIC3101(AIC3101_REG_E);		// 使R-ADC非静音

        WRITE_AIC3101(AIC3101_REG_F);	
	}
    else
    {
        printf("aic driver read error\n");
    }

    close(g_fd_gpio_iic);

    return s32Ret;
}

int main(int argc, char ** argv)
{
    if (argc != 2 && argc != 4)
    {
        usage(argv[0]);
        return 0;
    }

    int reg = atoi(argv[1]);
    int val = 0;

    g_fd_gpio_iic = open(GPIO_IIC_FILE, O_RDWR);
    if (g_fd_gpio_iic < 0)
    {
        printf("can't open g_fd_gpio_iic\n");
        return -1;
    }    

    if (2 == argc) //read
    {
        val = Read_1_Bit(reg);
        printf("Reg_%03d : MSB=%02d LSB=%02d\n", reg, (val & 0xf0) >> 4, val & 0xf);
    }
    else if (4 == argc)
    {
        val  = atoi(argv[2]) << 4 | atoi(argv[3]);
        Write_1_Bit(reg, val);

        usleep(10);

        val = Read_1_Bit(reg);
        printf("Reg_%03d : MSB=%02d LSB=%02d\n", reg, (val & 0xf0) >> 4, val & 0xf);
    }
    
    close(g_fd_gpio_iic);
    return 0;
}

