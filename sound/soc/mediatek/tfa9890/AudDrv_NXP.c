/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*******************************************************************************
 *
 * Filename:
 * ---------
 *   AudDrv_Kernelc
 *
 * Project:
 * --------
 *    Audio smart pa Function
 *
 * Description:
 * ------------
 *   Audio register
 *
 * Author:
 * -------
 * Chipeng Chang
 *
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime:$
 * $Log:$
 *
 *
 *******************************************************************************/


/*****************************************************************************
 *                     C O M P I L E R   F L A G S
 *****************************************************************************/


/*****************************************************************************
 *                E X T E R N A L   R E F E R E N C E S
 *****************************************************************************/
#include "AudDrv_NXP.h"
#ifdef CONFIG_MTK_NXP_TFA9890
#include <linux/input.h>	/* BUS_I2C */
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/module.h>
#include <linux/types.h>
//for platform device
#include <linux/platform_device.h>
//for misc device
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/dma-mapping.h>
#endif

#ifdef CONFIG_MTK_NXP_TFA9890
#define ECODEC_SLAVE_ADDR_WRITE 0x68
#define ECODEC_SLAVE_ADDR_READ  0x69
#else
#define ECODEC_SLAVE_ADDR_WRITE 0x68
#define ECODEC_SLAVE_ADDR_READ  0x69
#endif

#ifdef CONFIG_MTK_NXP_TFA9890
#define I2C_MASTER_CLOCK       400
#endif
//#define NXPEXTSPK_I2C_DEVNAME "TFA98XX"
#define NXPEXTSPK_I2C_DEVNAME "i2c_smartpa"

/*****************************************************************************
*           DEFINE AND CONSTANT
******************************************************************************
*/

#define AUDDRV_NXPSPK_NAME   "MediaTek Audio NXPSPK Driver"
#define AUDDRV_AUTHOR "MediaTek WCX"
#define RW_BUFFER_LENGTH (256)

/*****************************************************************************
*           V A R I A B L E     D E L A R A T I O N
*******************************************************************************/

static char       auddrv_nxpspk_name[]       = "AudioMTKNXPSPK";
// I2C variable
static struct i2c_client *new_client = NULL;
char WriteBuffer[RW_BUFFER_LENGTH];
char ReadBuffer[RW_BUFFER_LENGTH];

#ifdef CONFIG_MTK_NXP_TFA9890
static u8 *Tfa9890I2CDMABuf_va = NULL;
static u32 Tfa9890I2CDMABuf_pa = NULL;
#endif

// new I2C register method
static const struct i2c_device_id NXPExt_i2c_id[] = {{NXPEXTSPK_I2C_DEVNAME, 0}, {}};
static struct i2c_board_info __initdata  NXPExt_dev = {I2C_BOARD_INFO(NXPEXTSPK_I2C_DEVNAME, (ECODEC_SLAVE_ADDR_WRITE >> 1))};

static struct i2c_board_info NXPExt_dev_info = {
    .type = NXPEXTSPK_I2C_DEVNAME,
    .addr = (ECODEC_SLAVE_ADDR_WRITE >> 1),
};

//function declration
static int NXPExtSpk_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int NXPExtSpk_i2c_remove(struct i2c_client *client);
void AudDrv_NXPSpk_Init(void);
bool NXPExtSpk_Register(void);
static int NXPExtSpk_register(void);
ssize_t  NXPSpk_read_byte(u8 addr, u8 *returnData);

struct pinctrl *pinctrlnxp;

enum audio_system_gpio_type {
	GPIO_DEFAULT = 0,
	GPIO_RST_MODE0,
	GPIO_RST_MODE1,
	GPIO_I2S3_MODE0,
	GPIO_I2S3_MODE1,
	GPIO_NUM
};

struct nxp_gpio_attr {
	const char *name;
	bool gpio_prepare;
	struct pinctrl_state *gpioctrl;
};

static struct nxp_gpio_attr nxp_gpios[GPIO_NUM] = {
	[GPIO_DEFAULT] = {"default", false, NULL},
	[GPIO_RST_MODE0] = {"nxp9890-rst0", false, NULL},
	[GPIO_RST_MODE1] = {"nxp9890-rst1", false, NULL},
	[GPIO_I2S3_MODE0] = {"audi2s3-mode0", false, NULL},
	[GPIO_I2S3_MODE1] = {"audi2s3-mode1", false, NULL},
};

static void get_nxp_dts_func(struct device *dev)
{
    int ret = 0, i = 0;

    pinctrlnxp = devm_pinctrl_get(dev);
    if (IS_ERR(pinctrlnxp)) {
        ret = PTR_ERR(pinctrlnxp);
        printk("Cannot find pinctrlnxp!\n");
        return;
    }

    for (i = 0; i < ARRAY_SIZE(nxp_gpios); i++) {
        nxp_gpios[i].gpioctrl = pinctrl_lookup_state(pinctrlnxp, nxp_gpios[i].name);
        if (IS_ERR(nxp_gpios[i].gpioctrl)) {
            ret = PTR_ERR(nxp_gpios[i].gpioctrl);
            printk("%s pinctrl_lookup_state %s fail %d\n", __func__, nxp_gpios[i].name,ret);
        } else {
            nxp_gpios[i].gpio_prepare = true;
            printk("%s pinctrl_lookup_state %s success\n", __func__, nxp_gpios[i].name);
        }
    }

    return;
}

#ifdef CONFIG_OF
static const struct of_device_id NXPExtSpk_i2c_of_ids[] = {
    {.compatible = "mediatek,nxp9890_i2c",},
    {}
};
#endif

//i2c driver
struct i2c_driver NXPExtSpk_i2c_driver =
{
    .probe = NXPExtSpk_i2c_probe,
    .remove = NXPExtSpk_i2c_remove,
    .driver = {
        .name = NXPEXTSPK_I2C_DEVNAME,
#ifdef CONFIG_OF
        .of_match_table = NXPExtSpk_i2c_of_ids,
#endif
    },
    .id_table = NXPExt_i2c_id,
};

static int NXPExtSpk_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    new_client = client;
    new_client->timing = 400;
    printk("NXPExtSpk_i2c_probe \n");

#if CONFIG_MTK_NXP_TFA9890
    int retval = 0;
    if (nxp_gpios[GPIO_RST_MODE0].gpio_prepare) {
        retval = pinctrl_select_state(pinctrlnxp, nxp_gpios[GPIO_RST_MODE0].gpioctrl);
        if (retval)
            printk("could not set nxp_gpios[GPIO_RST_MODE0] pins\n");
        msleep(2);
        retval = pinctrl_select_state(pinctrlnxp, nxp_gpios[GPIO_RST_MODE1].gpioctrl);
        if (retval)
            printk("could not set nxp_gpios[GPIO_RST_MODE1] pins\n");
        msleep(2);
        retval = pinctrl_select_state(pinctrlnxp, nxp_gpios[GPIO_RST_MODE0].gpioctrl);
        if (retval)
            printk("could not set nxp_gpios[GPIO_RST_MODE0] pins\n");
        msleep(10);
    } else {
        printk("could not set nxp_gpios[GPIO_RST_MODE0] pins, nxp_gpios[GPIO_RST_MODE0] pins is init fail\n");
    }
#endif
#ifdef CONFIG_MTK_NXP_TFA9890
    Tfa9890I2CDMABuf_va = (u8 *)dma_alloc_coherent(&(client->dev), 4096, (dma_addr_t *)&Tfa9890I2CDMABuf_pa, GFP_KERNEL);
    //dma_alloc_coherent(&(camerahw_platform_device.dev) , bytes, (dma_addr_t *)&phyAddr, GFP_KERNEL);

    if(!Tfa9890I2CDMABuf_va)
    {
        NXP_ERROR("tfa9890 dma_alloc_coherent error\n");
        NXP_INFO("tfa9890_i2c_probe failed\n");
        return -1;
    }
    NXP_INFO("tfa9890_i2c_probe success\n");
#endif
    //printk("client new timing=%dK \n", new_client->timing);
    return 0;
}

static int NXPExtSpk_i2c_remove(struct i2c_client *client)
{
    new_client = NULL;
    i2c_unregister_device(client);
    i2c_del_driver(&NXPExtSpk_i2c_driver);
#ifdef CONFIG_MTK_NXP_TFA9890
    if(Tfa9890I2CDMABuf_va)
    {
        dma_free_coherent(&(client->dev), 4096, Tfa9890I2CDMABuf_va, Tfa9890I2CDMABuf_pa);
        Tfa9890I2CDMABuf_va = NULL;
        Tfa9890I2CDMABuf_pa = 0;
    }
    msleep(1);
#endif
    return 0;
}

// read write implementation
//read one register
ssize_t  NXPSpk_read_byte(u8 addr, u8 *returnData)
{
    char     cmd_buf[1] = {0x00};
    char     readData = 0;
    int     ret = 0;
    cmd_buf[0] = addr;

    if (!new_client)
    {
        printk("NXPSpk_read_byte I2C client not initialized!!");
        return -1;
    }
    ret = i2c_master_send(new_client, &cmd_buf[0], 1);
    if (ret < 0)
    {
        printk("NXPSpk_read_byte read sends command error!!\n");
        return -1;
    }
    ret = i2c_master_recv(new_client, &readData, 1);
    if (ret < 0)
    {
        printk("NXPSpk_read_byte reads recv data error!!\n");
        return -1;
    }
    *returnData = readData;
    //printk("addr 0x%x data 0x%x \n", addr, readData);
    return 0;
}

//write register
ssize_t  NXPExt_write_byte(u8 addr, u8 writeData)
{
    if (!new_client)
    {
        printk("I2C client not initialized!!");
        return -1;
    }
    char    write_data[2] = {0};
    int    ret = 0;
    write_data[0] = addr;         // ex. 0x01
    write_data[1] = writeData;
    ret = i2c_master_send(new_client, write_data, 2);
    if (ret < 0)
    {
        printk("write sends command error!!");
        return -1;
    }
    //printk("addr 0x%x data 0x%x \n", addr, writeData);
    return 0;
}


static int NXPExtSpk_register()
{
    printk("NXPExtSpk_register \n");

    if (i2c_add_driver(&NXPExtSpk_i2c_driver))
    {
        printk("fail to add device into i2c");
        return -1;
    }
    return 0;
}


bool NXPExtSpk_Register(void)
{
    printk("NXPExtSpk_Register \n");
    NXPExtSpk_register();
    return true;
}

void AudDrv_NXPSpk_Init(void)
{
    printk("Set GPIO for AFE I2S output to external DAC \n");
    int retval = 0;
    if (nxp_gpios[GPIO_I2S3_MODE1].gpio_prepare) {
        retval = pinctrl_select_state(pinctrlnxp, nxp_gpios[GPIO_I2S3_MODE1].gpioctrl);
        if (retval)
            printk("could not set nxp_gpios[GPIO_I2S3_MODE1] pins\n");
    } else {
        printk("could not set nxp_gpios[GPIO_I2S3_MODE1] pins, nxp_gpios[GPIO_I2S3_MODE1] pins is init fail\n");
    }
}

/*****************************************************************************
 * FILE OPERATION FUNCTION
 *  AudDrv_nxpspk_ioctl
 *
 * DESCRIPTION
 *  IOCTL Msg handle
 *
 *****************************************************************************
 */
static long AudDrv_nxpspk_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    int  ret = 0;

    //printk("AudDrv_nxpspk_ioctl cmd = 0x%x arg = %lu\n", cmd, arg);

    switch (cmd)
    {
		case I2C_SLAVE:
		case I2C_SLAVE_FORCE:
			if(arg == (ECODEC_SLAVE_ADDR_WRITE >> 1))
			{
			    //client->addr = arg;
			    return 0;
			}
			else
			    return -1;
        default:
        {
            //printk("AudDrv_nxpspk_ioctl Fail command: %x \n", cmd);
            ret = 0;
            break;
        }
    }
    return ret;
}

static int AudDrv_nxpspk_probe(struct platform_device *dev)
{
    int ret = 0;
    printk("AudDrv_nxpspk_probe \n");

    if (ret < 0)
    {
        printk("AudDrv_nxpspk_probe request_irq MT6582_AP_BT_CVSD_IRQ_LINE Fail \n");
    }

    get_nxp_dts_func(&dev->dev);

    NXPExtSpk_Register();
    AudDrv_NXPSpk_Init();

    memset((void *)WriteBuffer, 0, RW_BUFFER_LENGTH);
    memset((void *)ReadBuffer, 0, RW_BUFFER_LENGTH);

    printk("-AudDrv_nxpspk_probe \n");
    return 0;
}

static int AudDrv_nxpspk_open(struct inode *inode, struct file *fp)
{
    return 0;
}

#ifdef CONFIG_MTK_NXP_TFA9890
static int nxp_i2c_master_send(const struct i2c_client *client, const char *buf, int count)
{
	int ret;
	struct i2c_adapter *adap = client->adapter;
	struct i2c_msg msg;
	
	msg.timing = I2C_MASTER_CLOCK;

	if(count <= 8)
	{	
		msg.addr = client->addr & I2C_MASK_FLAG;
	}
	else
	{
		msg.addr = client->addr & I2C_MASK_FLAG | I2C_DMA_FLAG;
	}	
		
	msg.flags = client->flags & I2C_M_TEN;
//	msg.timing = client->timing;

	msg.len = count;
	msg.buf = (char *)buf;
	msg.ext_flag = client->ext_flag;
	ret = i2c_transfer(adap, &msg, 1);

	/*
	 * If everything went ok (i.e. 1 msg transmitted), return #bytes
	 * transmitted, else error code.
	 */
	return (ret == 1) ? count : ret;
}

static int nxp_i2c_master_recv(const struct i2c_client *client, char *buf, int count)
{
	struct i2c_adapter *adap = client->adapter;
	struct i2c_msg msg;
	int ret;

	msg.timing = I2C_MASTER_CLOCK;
	msg.flags = client->flags & I2C_M_TEN;
	msg.flags |= I2C_M_RD;
	msg.len = count;
	msg.ext_flag = client->ext_flag;
	msg.buf = (char *)buf;

	if(count <= 8)
	{
		msg.addr = client->addr & I2C_MASK_FLAG;
	}
	else
	{
		msg.addr = client->addr & I2C_MASK_FLAG | I2C_DMA_FLAG;
	}

	ret = i2c_transfer(adap, &msg, 1);

	/*
	 * If everything went ok (i.e. 1 msg received), return #bytes received,
	 * else error code.
	 */
	return (ret == 1) ? count : ret;
}

//static ssize_t tfa9890_write(struct file *file, const char __user *data, size_t count, loff_t *offset)
static ssize_t AudDrv_nxpspk_write(struct file *fp, const char __user *data, size_t count, loff_t *offset)

{
	int i = 0;
	int ret;
	char *tmp;

	//if (count > 8192)
	//	count = 8192;

	tmp = kmalloc(count,GFP_KERNEL);
	if (tmp==NULL)
		return -ENOMEM;
	if (copy_from_user(tmp,data,count)) {
		kfree(tmp);
		return -EFAULT;
	}

	for(i = 0;  i < count; i++)
	{
		Tfa9890I2CDMABuf_va[i] = tmp[i];
	}

	if(count <= 8)
	{
	    ///new_client->addr = new_client->addr & I2C_MASK_FLAG;  //cruson
		ret = nxp_i2c_master_send(new_client,tmp,count);
	}
	else
	{
	    //new_client->addr = new_client->addr & I2C_MASK_FLAG | I2C_DMA_FLAG |I2C_ENEXT_FLAG;  //cruson
		ret = nxp_i2c_master_send(new_client, Tfa9890I2CDMABuf_pa, count);
	}
	kfree(tmp);
	return ret;
}

//static ssize_t tfa9890_read(struct file *file,  char __user *data, size_t count,loff_t *offset)
static ssize_t AudDrv_nxpspk_read(struct file *fp,  char __user *data, size_t count, loff_t *offset)
{
	int i = 0;
	char *tmp;
	int ret;

	if (count > 8192)
		count = 8192;

	tmp = kmalloc(count,GFP_KERNEL);
	if (tmp==NULL)
		return -ENOMEM;

	if(count <= 8)
	{
	    //new_client->addr = new_client->addr & I2C_MASK_FLAG;  //cruson
		ret = nxp_i2c_master_recv(new_client,tmp,count);
	}
	else
	{
	    //new_client->addr = new_client->addr & I2C_MASK_FLAG | I2C_DMA_FLAG |I2C_ENEXT_FLAG;  //cruson
		ret = nxp_i2c_master_recv(new_client,Tfa9890I2CDMABuf_pa,count);
		for(i = 0; i < count; i++)
		{
			tmp[i] = Tfa9890I2CDMABuf_va[i];
		}
	}
	
	if (ret >= 0)
		ret = copy_to_user(data,tmp,count)?-EFAULT:ret;
	kfree(tmp);
	return ret;
}
#else
static ssize_t AudDrv_nxpspk_write(struct file *fp, const char __user *data, size_t count, loff_t *offset)
{
    int written_size = count;
    int ret = 0;
    //printk("AudDrv_nxpspk_write count = %d\n", count);
    if (!access_ok(VERIFY_READ, data, count))
    {
        printk("AudDrv_nxpspk_write !access_ok\n");
        return count;
    }
    else
    {
        // copy data from user space
        if (copy_from_user(WriteBuffer, data, count))
        {
            printk("printk Fail copy from user \n");
            return -1;
        }

        char *Write_ptr = WriteBuffer;
        //printk("data0 = 0x%x \n",  WriteBuffer[0]);
        unsigned char TempBuffer[3];
        TempBuffer[0] = WriteBuffer[0];
        //printk("written_size = %d\n", written_size);
        Write_ptr++;
        if(written_size == 1)
        {
            //printk("send first data \n");
            ret = i2c_master_send(new_client, &TempBuffer[0] , 1);
            Write_ptr++;
            written_size --;
        }

        while(written_size >= 3)
        {
            TempBuffer[1] = *Write_ptr;
            Write_ptr++;
            TempBuffer[2] = *Write_ptr;
            Write_ptr++;
            ret = i2c_master_send(new_client, &TempBuffer [0], 3);
            written_size -= 2 ;
        }

        if(written_size == 2)
        {
            //printk("send last data \n");
            TempBuffer[1] = *Write_ptr;
            ret = i2c_master_send(new_client, &TempBuffer[0], 2);
            written_size --;
        }

        if (ret < 0)
        {
            printk("write sends command error!!");
            return 0;
        }
    }
    return count;
}

static ssize_t AudDrv_nxpspk_read(struct file *fp,  char __user *data, size_t count, loff_t *offset)
{
    int read_count = count;
    int ret = 0;

    //printk("AudDrv_nxpspk_read  count = %d\n", count);
    if (!access_ok(VERIFY_READ, data, count))
    {
        printk("AudDrv_nxpspk_read !access_ok\n");
        return count;
    }
    else
    {
        // copy data from user space
        if (copy_from_user(ReadBuffer, data, count))
        {
            printk("printk Fail copy from user \n");
            return -1;
        }
        char *Read_ptr = &ReadBuffer[0];
        //printk("data0 = 0x%x data1 = 0x%x \n",  ReadBuffer[0], ReadBuffer[1]);

        /*
        ret = i2c_master_send(new_client,  &ReadBuffer[0], 1);
        if (ret < 0)
        {
            printk("AudDrv_nxpspk_read read sends command error!!\n");
            return -1;
        }
        */

        //printk("i2c_master_recv read_count = %d \n", read_count);
        ret = i2c_master_recv(new_client, Read_ptr, read_count);

        if (ret < 0)
        {
            printk("write sends command error!!");
            return 0;
        }
        //printk("data0 = 0x%x data1 = 0x%x  \n",  ReadBuffer[0], ReadBuffer[1]);
        if (copy_to_user((void __user *)data, (void *)ReadBuffer, count))
        {
            printk("printk Fail copy from user \n");
            return -1;
        }
    }
    return count;
}
#endif

/**************************************************************************
 * STRUCT
 *  File Operations and misc device
 *
 **************************************************************************/

static struct file_operations AudDrv_nxpspk_fops =
{
    .owner   = THIS_MODULE,
    .open    = AudDrv_nxpspk_open,
    .unlocked_ioctl   = AudDrv_nxpspk_ioctl,
    .write   = AudDrv_nxpspk_write,
    .read    = AudDrv_nxpspk_read,
};

#ifdef CONFIG_MTK_NXP_TFA9890
static struct miscdevice AudDrv_nxpspk_device =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = "smartpa_i2c",
    .fops = &AudDrv_nxpspk_fops,
};
#else
static struct miscdevice AudDrv_nxpspk_device =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = "nxpspk",
    .fops = &AudDrv_nxpspk_fops,
};
#endif

/***************************************************************************
 * FUNCTION
 *  AudDrv_nxpspk_mod_init / AudDrv_nxpspk_mod_exit
 *
 * DESCRIPTION
 *  Module init and de-init (only be called when system boot up)
 *
 **************************************************************************/
static const struct of_device_id nxp_of_ids[] = {
    {.compatible = "mediatek,nxp9890",},
    {}
};

static struct platform_driver AudDrv_nxpspk = {
    .probe    = AudDrv_nxpspk_probe,
    .driver = {
        .name = auddrv_nxpspk_name,
        .owner = THIS_MODULE,
        .of_match_table = nxp_of_ids,
    },
};

static int AudDrv_nxpspk_mod_init(void)
{
    int ret = 0;
    printk("+AudDrv_nxpspk_mod_init \n");

    // Register platform DRIVER
    ret = platform_driver_register(&AudDrv_nxpspk);
    if (ret)
    {
        printk("AudDrv Fail:%d - Register DRIVER \n", ret);
        return ret;
    }

    // register MISC device
    if ((ret = misc_register(&AudDrv_nxpspk_device)))
    {
    	printk("AudDrv_nxpspk_mod_init misc_register Fail:%d \n", ret);
    	return ret;
    }

    printk("-AudDrv_nxpspk_mod_init\n");
    return 0;
}

static void  AudDrv_nxpspk_mod_exit(void)
{
    printk("+AudDrv_nxpspk_mod_exit \n");

    printk("-AudDrv_nxpspk_mod_exit \n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(AUDDRV_NXPSPK_NAME);
MODULE_AUTHOR(AUDDRV_AUTHOR);

module_init(AudDrv_nxpspk_mod_init);
module_exit(AudDrv_nxpspk_mod_exit);

EXPORT_SYMBOL(NXPSpk_read_byte);
EXPORT_SYMBOL(NXPExt_write_byte);


