#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/time.h>
#include "kd_flashlight.h"
#include <asm/io.h>
#include <asm/uaccess.h>
#include "kd_camera_typedef.h"
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/i2c.h>
#include <linux/leds.h>



/******************************************************************************
 * Debug configuration
******************************************************************************/
/* availible parameter */
/* ANDROID_LOG_ASSERT */
/* ANDROID_LOG_ERROR */
/* ANDROID_LOG_WARNING */
/* ANDROID_LOG_INFO */
/* ANDROID_LOG_DEBUG */
/* ANDROID_LOG_VERBOSE */

#define TAG_NAME "[leds_strobe.c]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    pr_debug(TAG_NAME "%s: " fmt, __func__ , ##arg)

//#define DEBUG_LEDS_STROBE
#ifdef DEBUG_LEDS_STROBE
#define PK_DBG PK_DBG_FUNC
#else
#define PK_DBG(a, ...)
#endif

/******************************************************************************
 * local variables
******************************************************************************/

static DEFINE_SPINLOCK(g_strobeSMPLock);	/* cotta-- SMP proection */


static u32 strobe_Res;
static u32 strobe_Timeus;
static BOOL g_strobe_On;

//static int g_duty = -1;
static int g_timeOutTimeMs;

static DEFINE_MUTEX(g_strobeSem);


#define STROBE_DEVICE_ID 0xC6


static struct work_struct workTimeOut;

/*****************************************************************************
Functions
*****************************************************************************/
static void work_timeOutFunc(struct work_struct *data);

 struct i2c_client *SY7806_i2c_client;
struct SY7806_platform_data {
	u8 torch_pin_enable;	/* 1:  TX1/TORCH pin isa hardware TORCH enable */
	u8 pam_sync_pin_enable;	/* 1:  TX2 Mode The ENVM/TX2 is a PAM Sync. on input */
	u8 thermal_comp_mode_enable;	/* 1: LEDI/NTC pin in Thermal Comparator Mode */
	u8 strobe_pin_disable;	/* 1 : STROBE Input disabled */
	u8 vout_mode_enable;	/* 1 : Voltage Out Mode enable */
};

struct SY7806_chip_data {
	struct i2c_client *client;

	/* struct led_classdev cdev_flash; */
	/* struct led_classdev cdev_torch; */
	/* struct led_classdev cdev_indicator; */

	struct SY7806_platform_data *pdata;
	struct mutex lock;

	u8 last_flag;
	u8 no_pdata;
};

 int SY7806_write_reg(struct i2c_client *client, u8 reg, u8 val)
{
	int ret = 0;
	struct SY7806_chip_data *chip = i2c_get_clientdata(client);

	mutex_lock(&chip->lock);
	ret = i2c_smbus_write_byte_data(client, reg, val);
	mutex_unlock(&chip->lock);

	if (ret < 0)
		PK_DBG("failed writing at 0x%02x\n", reg);
	return ret;
}

static int SY7806_read_reg(struct i2c_client *client, u8 reg)
{
	int val = 0;
	struct SY7806_chip_data *chip = i2c_get_clientdata(client);

	mutex_lock(&chip->lock);
	val = i2c_smbus_read_byte_data(client, reg);
	mutex_unlock(&chip->lock);


	return val;
}
int readReg(int reg)
{
	int val;
	val = SY7806_read_reg(SY7806_i2c_client, reg);
	return (int)val;
}

enum
{
	e_DutyNum = 26,
};

static int isMovieMode[e_DutyNum] = {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int torchDuty[e_DutyNum]=    {35,71,106,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//50, 99, 148, 179, 

static int flashDuty[e_DutyNum]={0,0,0,0,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,98,102};
//188, 246, 293, 340, 387, 434, 481, 528, 575, 621, 668, 715, 762, 809, 855, 902, 950, 996, 1044, 1090, 1161, 1207

int m_duty1=0;
int m_duty2=0;
int LED1CloseFlag = 1;
int LED2CloseFlag = 1;

int flashEnable_SY7806_2(void)
{
	
	int err;
	int enable_value =0;

	PK_DBG(" flashDisable_SY7806_2 S line=%d\n",__LINE__);

	enable_value = readReg(0x01);
	PK_DBG(" LED1&2_enable_value S =0x%x\n",enable_value);

	if((LED1CloseFlag == 1) && (LED2CloseFlag == 1))
	{
		err = SY7806_write_reg(SY7806_i2c_client,0x01, (enable_value & 0xF0));
		return 0;
	}

	if(LED1CloseFlag == 1)
	{
		if(isMovieMode[m_duty2] == 1)
			err = SY7806_write_reg(SY7806_i2c_client,0x01, (enable_value & 0xF0) | 0x0A);
		else
			err = SY7806_write_reg(SY7806_i2c_client,0x01, (enable_value & 0xF0) | 0x0E);
	}
	else if(LED2CloseFlag == 1)
	{
		if(isMovieMode[m_duty1] == 1)
			err = SY7806_write_reg(SY7806_i2c_client,0x01, (enable_value & 0xF0) | 0x09);
		else
			err = SY7806_write_reg(SY7806_i2c_client,0x01, (enable_value & 0xF0) | 0x0D);
	}
	else
	{
		if((isMovieMode[m_duty1] == 1) && (isMovieMode[m_duty2] == 1))
			err = SY7806_write_reg(SY7806_i2c_client,0x01, (enable_value & 0xF0) | 0x0B);
		else
			err = SY7806_write_reg(SY7806_i2c_client,0x01, (enable_value & 0xF0) | 0x0F);			
	}

	enable_value = readReg(0x01);
	PK_DBG(" LED1&2_enable_value E =0x%x\n",enable_value);
	//PK_DBG(" isMovieMode[m_duty1] = %d  ,isMovieMode[m_duty2]=%d \n",isMovieMode[m_duty1],isMovieMode[m_duty2]);
	//PK_DBG(" m_duty1 %d  ,m_duty2 %d \n ",m_duty1,m_duty2);
	PK_DBG(" flashDisable_SY7806_2 E line=%d\n",__LINE__);
	return err;

}
int flashDisable_SY7806_2(void)
{
	flashEnable_SY7806_2();
	return 0;
}


int setDuty_SY7806_2(int duty)
{

	int err=0;
	int temp=0;
	PK_DBG(" setDuty_SY7806_2 S line=%d\n",__LINE__);

	if((LED1CloseFlag == 1) && (LED2CloseFlag == 1))
	{
			
	}
	else if(LED1CloseFlag == 1)
	{
		if(isMovieMode[m_duty2] == 1)
		{
			temp = readReg(0x06);
			PK_DBG(" LED2_torch_register S =0x%x\n",temp);			
			err =  SY7806_write_reg(SY7806_i2c_client,0x06, torchDuty[m_duty2]);			
			temp = readReg(0x06);
			PK_DBG(" LED2_torch_register E =0x%x\n",temp);
		}
		else
		{
			temp = readReg(0x04);
			PK_DBG(" LED2_flash_register S =0x%x\n",temp);			
			err =  SY7806_write_reg(SY7806_i2c_client,0x04, flashDuty[m_duty2]);			
			temp = readReg(0x04);
			PK_DBG(" LED2_flash_register E =0x%x\n",temp);
		}
	}
	else if(LED2CloseFlag == 1)
	{
		if(isMovieMode[m_duty1] == 1)
		{
			temp = readReg(0x05);
			PK_DBG(" LED1_torch_register S =0x%x\n",temp);			
			err =  SY7806_write_reg(SY7806_i2c_client,0x05, torchDuty[m_duty1]);		
			temp = readReg(0x05);
			PK_DBG(" LED1_torch_register E =0x%x\n",temp);

		}
		else
		{
			temp = readReg(0x03);
			PK_DBG(" LED1_flash_register S =0x%x\n",temp);			
			err =  SY7806_write_reg(SY7806_i2c_client,0x03, flashDuty[m_duty1]);			
			temp = readReg(0x03);
			PK_DBG(" LED1_flash_register E =0x%x\n",temp);

		}		
	}
	else
	{
		if((isMovieMode[m_duty1] == 1)&&((isMovieMode[m_duty2] == 1)))
		{
			temp = readReg(0x05);
			temp = readReg(0x06);
			PK_DBG(" LED1&2_torch_register S =0x%x\n",temp);			
			err =  SY7806_write_reg(SY7806_i2c_client,0x05, torchDuty[m_duty1]);
			err =  SY7806_write_reg(SY7806_i2c_client,0x06, torchDuty[m_duty2]);
			temp = readReg(0x05);
			PK_DBG("  LED1&2_torch_register 0x05 =0x%x\n",temp);
			temp = readReg(0x06);
			PK_DBG("  LED1&2_torch_register 0x04 =0x%x\n",temp);			
		}
		else
		{
			temp = readReg(0x03);
			temp = readReg(0x04);
			PK_DBG(" LED1&2_flash_register S =0x%x\n",temp);
			err =  SY7806_write_reg(SY7806_i2c_client,0x03, flashDuty[m_duty1]);
			err =  SY7806_write_reg(SY7806_i2c_client,0x04, flashDuty[m_duty2]);
			temp = readReg(0x03);
			PK_DBG("  LED1&2_flash_register 0x03 =0x%x\n",temp);
			temp = readReg(0x04);
			PK_DBG("  LED1&2_flash_register 0x04 =0x%x\n",temp);		
		}
	}	

	PK_DBG(" setDuty_SY7806_2 E line=%d\n",__LINE__);

	return err;

}



static int SY7806_chip_init(struct SY7806_chip_data *chip)
{


	return 0;
}

static int SY7806_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct SY7806_chip_data *chip;
	struct SY7806_platform_data *pdata = client->dev.platform_data;

	int err = -1;

	PK_DBG("SY7806_probe start--->.\n");

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		err = -ENODEV;
		PK_DBG("SY7806 i2c functionality check fail.\n");
		return err;
	}

	chip = kzalloc(sizeof(struct SY7806_chip_data), GFP_KERNEL);
	chip->client = client;

	mutex_init(&chip->lock);
	i2c_set_clientdata(client, chip);

	if (pdata == NULL) {	/* values are set to Zero. */
		PK_DBG("SY7806 Platform data does not exist\n");
		pdata = kzalloc(sizeof(struct SY7806_platform_data), GFP_KERNEL);
		chip->pdata = pdata;
		chip->no_pdata = 1;
	}

	chip->pdata = pdata;
	if (SY7806_chip_init(chip) < 0)
		goto err_chip_init;

	SY7806_i2c_client = client;
	PK_DBG("SY7806 Initializing is done\n");

	return 0;

err_chip_init:
	i2c_set_clientdata(client, NULL);
	kfree(chip);
	PK_DBG("SY7806 probe is failed\n");
	return -ENODEV;
}

static int SY7806_remove(struct i2c_client *client)
{
	struct SY7806_chip_data *chip = i2c_get_clientdata(client);

	if (chip->no_pdata)
		kfree(chip->pdata);
	kfree(chip);
	return 0;
}


#define SY7806_NAME "leds-SY7806"
static const struct i2c_device_id SY7806_id[] = {
	{SY7806_NAME, 0},
	{}
};

#ifdef CONFIG_OF
static const struct of_device_id SY7806_of_match[] = {
	{.compatible = "mediatek,strobe_main"},
	{},
};
#endif

static struct i2c_driver SY7806_i2c_driver = {
	.driver = {
		   .name = SY7806_NAME,
#ifdef CONFIG_OF
		   .of_match_table = SY7806_of_match,
#endif
		   },
	.probe = SY7806_probe,
	.remove = SY7806_remove,
	.id_table = SY7806_id,
};
static int __init SY7806_init(void)
{
	PK_DBG("SY7806_init\n");
	return i2c_add_driver(&SY7806_i2c_driver);
}

static void __exit SY7806_exit(void)
{
	i2c_del_driver(&SY7806_i2c_driver);
}


module_init(SY7806_init);
module_exit(SY7806_exit);

MODULE_DESCRIPTION("Flash driver for SY7806");
MODULE_AUTHOR("pw <pengwei@mediatek.com>");
MODULE_LICENSE("GPL v2");


int flashEnable_SY7806_1(void)
{
	return 0;
}
int flashDisable_SY7806_1(void)
{
	return 0;
}


int setDuty_SY7806_1(int duty)
{
	PK_DBG(" setDuty_SY7806_1 S line=%d\n",__LINE__);
	//mt_set_gpio_out(FLASH_GPIO_EN,GPIO_OUT_ONE);
	if(duty<0)
		duty=0;
	else if(duty>=e_DutyNum)
		duty=e_DutyNum-1;
	m_duty1=duty;


	PK_DBG(" setDuty_SY7806_1 E line=%d\n",__LINE__);
	return 0;
}

int init_SY7806(void)
{
	int err;
	
	PK_DBG(" init_SY7806 S line=%d\n",__LINE__);
	mtkcam_gpio_set(MainCam,CAMFL,Flashlight_Open);
	err = SY7806_write_reg(SY7806_i2c_client,0x01, 0x00);
	err = SY7806_write_reg(SY7806_i2c_client,0x08, 0x1F);
	PK_DBG(" init_SY7806 E line=%d\n",__LINE__);
	return err;
}



int FL_Enable(void)
{
	PK_DBG(" FL_Enable line=%d\n", __LINE__);
	flashEnable_SY7806_1();
	return 0;
}



int FL_Disable(void)
{
	PK_DBG(" FL_Disable line=%d\n", __LINE__);
	flashDisable_SY7806_1();
	return 0;
}

static int FL_dim_duty(kal_uint32 duty)
{
	PK_DBG(" FL_dim_duty line=%d\n", __LINE__);
	setDuty_SY7806_1(duty);
	return 0;
}

int FL_Init(void)
{
	PK_DBG("FL_Init  \n");
	init_SY7806();
	INIT_WORK(&workTimeOut, work_timeOutFunc);
	return 0 ;
}


int FL_Uninit(void)
{
	mtkcam_gpio_set(MainCam,CAMFL,Flashlight_Close);

	PK_DBG("FL_UInit  \n");
	return 0;
}

/*****************************************************************************
User interface
*****************************************************************************/

static void work_timeOutFunc(struct work_struct *data)
{
	FL_Disable();
	PK_DBG("ledTimeOut_callback\n");
}



enum hrtimer_restart ledTimeOutCallback(struct hrtimer *timer)
{
	schedule_work(&workTimeOut);
	return HRTIMER_NORESTART;
}

static struct hrtimer g_timeOutTimer;
void timerInit(void)
{
	INIT_WORK(&workTimeOut, work_timeOutFunc);
	g_timeOutTimeMs = 1000;
	hrtimer_init(&g_timeOutTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	g_timeOutTimer.function = ledTimeOutCallback;
}



static int constant_flashlight_ioctl(unsigned int cmd, unsigned long arg)
{
	int i4RetValue = 0;
	int ior_shift;
	int iow_shift;
	int iowr_shift;

	ior_shift = cmd - (_IOR(FLASHLIGHT_MAGIC, 0, int));
	iow_shift = cmd - (_IOW(FLASHLIGHT_MAGIC, 0, int));
	iowr_shift = cmd - (_IOWR(FLASHLIGHT_MAGIC, 0, int));
/*	PK_DBG
	    ("SY7806 constant_flashlight_ioctl() line=%d ior_shift=%d, iow_shift=%d iowr_shift=%d arg=%d\n",
	     __LINE__, ior_shift, iow_shift, iowr_shift, (int)arg);*/

	switch (cmd) {

	case FLASH_IOC_SET_TIME_OUT_TIME_MS:
		PK_DBG("FLASH_IOC_SET_TIME_OUT_TIME_MS: %d\n", (int)arg);
		g_timeOutTimeMs = arg;
		break;


	case FLASH_IOC_SET_DUTY:
		PK_DBG("FLASHLIGHT_DUTY: %d\n", (int)arg);
		FL_dim_duty(arg);
		break;


	case FLASH_IOC_SET_STEP:
		PK_DBG("FLASH_IOC_SET_STEP: %d\n", (int)arg);

		break;

	case FLASH_IOC_SET_ONOFF:
		PK_DBG("FLASHLIGHT_ONOFF: %d\n", (int)arg);
		if(arg==1)
		{
			if(g_timeOutTimeMs!=0)
			{
				ktime_t ktime;
				ktime = ktime_set( 0, g_timeOutTimeMs*1000000 );
				hrtimer_start( &g_timeOutTimer, ktime, HRTIMER_MODE_REL );
			}
			LED1CloseFlag = 0;
			FL_Enable();
		}
		else
		{
			LED1CloseFlag = 1;
			FL_Disable();
			hrtimer_cancel( &g_timeOutTimer );
		}

		break;
      case FLASH_IOC_SET_REG_ADR:
    	    break;
    	case FLASH_IOC_SET_REG_VAL:
    	    break;
    	case FLASH_IOC_SET_REG:
    	    break;
    	case FLASH_IOC_GET_REG:
    	    break;
	default:
		PK_DBG(" No such command\n");
		i4RetValue = -EPERM;
		break;
	}
	return i4RetValue;
}




static int constant_flashlight_open(void *pArg)
{
	int i4RetValue = 0;

	PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

	if (0 == strobe_Res) {
		FL_Init();
		timerInit();
	}
	PK_DBG("constant_flashlight_open line=%d\n", __LINE__);
	spin_lock_irq(&g_strobeSMPLock);


	if (strobe_Res) {
		PK_DBG(" busy!\n");
		i4RetValue = -EBUSY;
	} else {
		strobe_Res += 1;
	}


	spin_unlock_irq(&g_strobeSMPLock);
	PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

	return i4RetValue;

}


static int constant_flashlight_release(void *pArg)
{
	PK_DBG(" constant_flashlight_release\n");

	if (strobe_Res) {
		spin_lock_irq(&g_strobeSMPLock);

		strobe_Res = 0;
		strobe_Timeus = 0;

		/* LED On Status */
		g_strobe_On = FALSE;

		spin_unlock_irq(&g_strobeSMPLock);

		FL_Uninit();
	}

	PK_DBG(" Done\n");

	return 0;

}


FLASHLIGHT_FUNCTION_STRUCT constantFlashlightFunc = {
	constant_flashlight_open,
	constant_flashlight_release,
	constant_flashlight_ioctl
};


MUINT32 constantFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc)
{
	if (pfFunc != NULL)
		*pfFunc = &constantFlashlightFunc;
	return 0;
}



/* LED flash control for high current capture mode*/
ssize_t strobe_VDIrq(void)
{

	return 0;
}
EXPORT_SYMBOL(strobe_VDIrq);
