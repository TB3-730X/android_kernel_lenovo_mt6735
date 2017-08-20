/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/* drivers/hwmon/mt6516/amit/em30918.c - em30918 ALS/PS driver
 * 
 * Author: 
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <linux/platform_device.h>
#include <asm/atomic.h>
#include <linux/sysfs.h>
#include <linux/device.h> 

#include <linux/wakelock.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <cust_alsps.h>
#include <alsps.h>
#include "em30918.h"

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

//#define POWER_NONE_MACRO MT65XX_POWER_NONE


/******************************************************************************
 * configuration
*******************************************************************************/
/*----------------------------------------------------------------------------*/

#define EM30918_DEV_NAME     "em30918"
/*----------------------------------------------------------------------------*/
#define APS_TAG                  "[ALS/PS] "
#define APS_FUN(f)               printk(APS_TAG"%s %d \n", __FUNCTION__ , __LINE__)
#define APS_ERR(fmt, args...)    printk(APS_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define APS_LOG(fmt, args...)    printk(APS_TAG fmt, ##args)
#define APS_DBG(fmt, args...)    printk(APS_TAG fmt, ##args)                 
/******************************************************************************
 * extern functions
*******************************************************************************/
/*for interrup work mode support --*/
/*
	extern void mt_eint_mask(unsigned int eint_num);
	extern void mt_eint_unmask(unsigned int eint_num);
	extern void mt_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
	extern void mt_eint_set_polarity(unsigned int eint_num, unsigned int pol);
	extern unsigned int mt_eint_set_sens(unsigned int eint_num, unsigned int sens);
	extern void mt_eint_registration(unsigned int eint_num, unsigned int flow, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);
	extern void mt_eint_print_status(void);
	*/
#define PS_ENABLE 0x80  //0XA0
#define PS_ENABLE_LONGDISATANCE 0xB0
#define PS_LTHD_VAL 5     //20

//wt add for ps calibration start
struct PS_CALI_DATA_STRUCT
{
    int close;
    int far_away;
    int valid;
} ;
static struct PS_CALI_DATA_STRUCT ps_cali={0,0,0};

//wt add for ps calibration end

/*----------------------------------------------------------------------------*/
static struct i2c_client *em30918_i2c_client = NULL;
/*----------------------------------------------------------------------------*/
static const struct i2c_device_id em30918_i2c_id[] = {{EM30918_DEV_NAME,0},{}};
/*----------------------------------------------------------------------------*/

//static struct i2c_board_info __initdata i2c_em30918={ I2C_BOARD_INFO(EM30918_DEV_NAME, 0x48>>1)};

static int em30918_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id); 
static int em30918_i2c_remove(struct i2c_client *client);
static int em30918_i2c_detect(struct i2c_client *client, struct i2c_board_info *info);
/*----------------------------------------------------------------------------*/
static int em30918_i2c_suspend(struct i2c_client *client, pm_message_t msg);
static int em30918_i2c_resume(struct i2c_client *client);
int em30918_setup_eint(struct i2c_client *client);
int em30918_read_ps(struct i2c_client *client, u16 *data);
int em30918_read_als(struct i2c_client *client, u16 *data);

//static struct em30918_priv *g_em30918_ptr = NULL;

struct alsps_hw alsps_cust;
static struct alsps_hw *hw = &alsps_cust;
struct platform_device *alspsPltFmDev;


static int	em30918_init_flag = -1;	// 0<==>OK -1 <==> fail
static int intr_flag_value = 0;
static int  em30918_local_init(void);
static int  em30918_local_uninit(void);

//static int em3071_read(struct i2c_client *client,u8 reg);
//static int em3071_write(struct i2c_client *client,u8 reg,u8 data);

//static int dev_attr_config(u8 reg,int config,int dev_s,int data);
static DEFINE_MUTEX(sensor_lock);
static struct alsps_init_info em30918_init_info = {
		.name = "em30918",
		.init = em30918_local_init,
		.uninit = em30918_local_uninit,
	
};
/*----------------------------------------------------------------------------*/
typedef enum {
    CMC_BIT_ALS    = 1,
    CMC_BIT_PS     = 2,
} CMC_BIT;
/*----------------------------------------------------------------------------*/
struct em30918_i2c_addr {    /*define a series of i2c slave address*/
    u8  write_addr;  
    u8  ps_thd;     /*PS INT threshold*/
};
/*----------------------------------------------------------------------------*/
struct em30918_priv {
    struct alsps_hw  *hw;
    struct i2c_client *client;
    struct work_struct  eint_work;

    /*i2c address group*/
    struct em30918_i2c_addr  addr;
    
    /*misc*/
    atomic_t    i2c_retry;
    atomic_t    als_suspend;
    atomic_t    als_debounce;   /*debounce time after enabling als*/
    atomic_t    als_deb_on;     /*indicates if the debounce is on*/
    atomic_t    als_deb_end;    /*the jiffies representing the end of debounce*/
    atomic_t    ps_mask;        /*mask ps: always return far away*/
    atomic_t    ps_debounce;    /*debounce time after enabling ps*/
    atomic_t    ps_deb_on;      /*indicates if the debounce is on*/
    atomic_t    ps_deb_end;     /*the jiffies representing the end of debounce*/
    atomic_t    ps_suspend;


    /*data*/
    u16         als;
    u16          ps;
    u8          _align;
    u16         als_level_num;
    u16         als_value_num;
    u32         als_level[C_CUST_ALS_LEVEL-1];
    u32         als_value[C_CUST_ALS_LEVEL];

    atomic_t    als_cmd_val;    /*the cmd value can't be read, stored in ram*/
    atomic_t    ps_cmd_val;     /*the cmd value can't be read, stored in ram*/
    atomic_t    ps_thd_val;     /*the cmd value can't be read, stored in ram*/
    ulong       enable;         /*enable mask*/
    ulong       pending_intr;   /*pending interrupt*/
	atomic_t ps_thd_val_high;	/*the cmd value can't be read, stored in ram */
	atomic_t ps_thd_val_low;	/*the cmd value can't be read, stored in ram */
	atomic_t ps_thd_val_ld_high;	/*the cmd value can't be read, stored in ram */
	atomic_t ps_thd_val_ld_low;	/*the cmd value can't be read, stored in ram */
    /*early suspend*/
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend    early_drv;
#endif    
	struct device_node *irq_node;
	int irq;
};

#ifdef CONFIG_OF
static const struct of_device_id alsps_of_match[] = {
	{.compatible = "mediatek,alsps"},
	{},
};
#endif

/*----------------------------------------------------------------------------*/
static struct i2c_driver em30918_i2c_driver = {	
	.probe      = em30918_i2c_probe,
	.remove     = em30918_i2c_remove,
	.detect     = em30918_i2c_detect,
	.suspend    = em30918_i2c_suspend,
	.resume     = em30918_i2c_resume,
	.id_table   = em30918_i2c_id,
	//.address_data = &em30918_addr_data,
	.driver = {
		.owner          = THIS_MODULE,
		.name           = EM30918_DEV_NAME,
#ifdef CONFIG_OF
		.of_match_table = alsps_of_match,
#endif
	},
};

static int ps_enabled=0;

static struct em30918_priv *em30918_obj = NULL;
static struct platform_driver em30918_alsps_driver;

static struct alsps_hw *get_cust_alsps(void)
{
	return &alsps_cust;
}
static int em30918_read(struct i2c_client *client,u8 reg)
{
	int res = 0;
	u8 buffer[1];
	
	buffer[0] = reg;
	res = i2c_master_send(client, buffer, 0x1);
	if(res <= 0)
	{
		return 0;
	}
	res = i2c_master_recv(client, buffer, 0x1);
	if(res <= 0)
	{
		return 0;
	}
	return buffer[0];
}

static int em30918_write(struct i2c_client *client,u8 reg,u8 data)
{
	int res = 0;
	u8 buffer[2];
	buffer[0] = reg;
	buffer[1] = data;
	res = i2c_master_send(client, buffer, 0x2);
	if(res <= 0)
	{
		return 0;
	}
	return 1;
}
/******************************************************************************
*********************DEVICE ATTER**********************************************
******************************************************************************/

//static struct kobject *em_kobj;


/*----------------------------------------------------------------------------*/
static ssize_t em30918_show_ps(struct device_driver *ddri, char *buf)
{
	int res;
	struct em30918_priv *obj = em30918_obj;
	if(!obj)
	{
		APS_ERR("em30918_obj is null!!\n");
		return 0;
	}
	
	if((res = em30918_read_ps(obj->client, &obj->ps)))
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
	else
	{
		#ifndef VENDOR_EDIT 
			return scnprintf(buf, PAGE_SIZE, "0x%04X\n", obj->ps); 
		#else/* VENDOR_EDIT */
			return scnprintf(buf, PAGE_SIZE, "%d\n", obj->ps);   
		#endif/* VENDOR_EDIT */		
	}
}
/*----------------------------------------------------------------------------*/


static ssize_t em30918_show_als(struct device_driver *ddri, char *buf)
{
	struct em30918_priv *obj = em30918_obj;
	int res;
	
	if(!obj)
	{
		APS_ERR("em30918_obj is null!!\n");
		return 0;
	}
	
	if((res = em30918_read_als(obj->client, &obj->als)))
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
	else
	{
		return scnprintf(buf, PAGE_SIZE, "%d\n", obj->als);   
	}

}
static ssize_t em30918_show_id(struct device_driver *ddri, char *buf)
{
	struct em30918_priv *obj = em30918_obj;
	int res;
	
	if(!obj)
	{
		APS_ERR("em30918_obj is null!!\n");
		return 0;
	}
	res=em30918_read(obj->client,EM30918_CMM_ID);

	if(res)
	{
		return snprintf(buf, PAGE_SIZE, "%x\n", res); 
	}
	else
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
}
static ssize_t em30918_show_ht(struct device_driver *ddri, char *buf)
{
	struct em30918_priv *obj = em30918_obj;
	int res;
	
	if(!obj)
	{
		APS_ERR("em30918_obj is null!!\n");
		return 0;
	}
	res=em30918_read(obj->client,EM30918_CMM_INT_PS_HB);

	if(res)
	{
		return snprintf(buf, PAGE_SIZE, "%x\n", res); 
	}
	else
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
}
static ssize_t em30918_show_lt(struct device_driver *ddri, char *buf)
{
	struct em30918_priv *obj = em30918_obj;
	int res;
	
	if(!obj)
	{
		APS_ERR("em30918_obj is null!!\n");
		return 0;
	}
	res=em30918_read(obj->client,EM30918_CMM_INT_PS_LB);

	if(res)
	{
		return snprintf(buf, PAGE_SIZE, "%x\n", res); 
	}
	else
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
}

static ssize_t em30918_show_config(struct device_driver *ddri, char *buf)
{
	struct em30918_priv *obj = em30918_obj;
	int res;
	
	if(!obj)
	{
		APS_ERR("em30918_obj is null!!\n");
		return 0;
	}
	res = em30918_read(obj->client,EM30918_CMM_ENABLE);
	if(res)
	{
		return snprintf(buf, PAGE_SIZE, "%x\n", res); 
	}
	else
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}

}
static ssize_t em30918_store_config(struct device_driver *ddri, const char *buf, size_t count)
{
	struct em30918_priv *obj = em30918_obj;
	int config;
	
	if(!obj)
	{
		APS_ERR("em30918_obj is null!!\n");
		return 0;
	}
	
	if(1 != sscanf(buf, "%x", &config))
	{
		APS_ERR("invalid content: \n");

	}
	else
	{
		em30918_write(obj->client,EM30918_CMM_ENABLE,config);
	}
	return count; 
}
static ssize_t em30918_show_longdistance_flag(struct device_driver *ddri, char *buf)
{
	struct em30918_priv *obj = em30918_obj;
	int res;
	
	if(!obj)
	{
		APS_ERR("em30918_obj is null!!\n");
		return 0;
	}
	res = em30918_read(obj->client,EM30918_CMM_ENABLE);
	if(res)
	{
		return snprintf(buf, PAGE_SIZE, "%x\n", res); 
	}
	else
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}

}
static ssize_t em30918_store_longdistance_flag(struct device_driver *ddri, const char *buf, size_t count)
{
		struct em30918_priv *obj = em30918_obj;
		int em30918_longdistance_flag,ps_ht,ps_lt ;

		if(!obj)
		{
			APS_ERR("em30918_obj is null!!\n");
			return 0;
		}
	        if(3 != sscanf(buf, "%x %x  %x ", &em30918_longdistance_flag,&ps_ht,&ps_lt))
		{
			APS_ERR("invalid content: \n");
	
		}else{
	
			atomic_set(&obj->ps_thd_val_ld_low,ps_lt);
			atomic_set(&obj->ps_thd_val_ld_high,ps_ht);
				
			em30918_write(obj->client,EM30918_CMM_ENABLE,em30918_longdistance_flag);
			em30918_write(obj->client,EM30918_CMM_INT_PS_LB,(atomic_read(&obj->ps_thd_val_ld_low)));
			em30918_write(obj->client,EM30918_CMM_INT_PS_HB,(atomic_read(&obj->ps_thd_val_ld_high)));
		
		}
		return count;	
	}

static DRIVER_ATTR(ps,S_IWUSR | S_IRUGO,em30918_show_ps,NULL);
static DRIVER_ATTR(als,S_IWUSR | S_IRUGO,em30918_show_als,NULL);
static DRIVER_ATTR(id,S_IWUSR | S_IRUGO,em30918_show_id,NULL);
static DRIVER_ATTR(ht,S_IWUSR | S_IRUGO,em30918_show_ht,NULL);
static DRIVER_ATTR(lt,S_IWUSR | S_IRUGO,em30918_show_lt,NULL);
static DRIVER_ATTR(config,S_IWUSR | S_IRUGO,em30918_show_config,em30918_store_config);
static DRIVER_ATTR(longdistance_flag,S_IWUSR | S_IRUGO,em30918_show_longdistance_flag,em30918_store_longdistance_flag);

static struct driver_attribute *em30918_attr_list[]={
	&driver_attr_ps,
	&driver_attr_als,
	&driver_attr_id,
	&driver_attr_ht,
	&driver_attr_lt,
	&driver_attr_config,
	&driver_attr_longdistance_flag,

};

static int em30918_create_attr(struct device_driver *driver) 
{
	//int ret;


	int idx, err = 0;
	int num = (int)(sizeof(em30918_attr_list)/sizeof(em30918_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}
	for(idx = 0; idx < num; idx++)
	{
		if((err = driver_create_file(driver, em30918_attr_list[idx])))
		{            
			APS_ERR("driver_create_file (%s) = %d\n", em30918_attr_list[idx]->attr.name, err);
			break;
		}
	}
	return err;
}

static int em30918_delete_attr(struct device_driver *driver)
{
	int idx ,err = 0;
	int num = (int)(sizeof(em30918_attr_list)/sizeof(em30918_attr_list[0]));

	if(driver == NULL)
	{
		return -EINVAL;
	}
	

	for(idx = 0; idx < num; idx++)
	{
		driver_remove_file(driver, em30918_attr_list[idx]);
	}
	

	return err;
}
/*----------------------------------------------------------------------------*/
int em30918_get_addr(struct alsps_hw *hw, struct em30918_i2c_addr *addr)
{
	if(!hw || !addr)
	{
		return -EFAULT;
	}
	addr->write_addr= 0x48>>1;//hw->i2c_addr[0];
	return 0;
}
/*----------------------------------------------------------------------------*/
static void em30918_power(struct alsps_hw *hw, unsigned int on) 
{
	static unsigned int power_on = 0;

	APS_LOG("power %s\n", on ? "on" : "off");
/*
	if(hw->power_id != POWER_NONE_MACRO)
	{
		if(power_on == on)
		{
			APS_LOG("ignore power control: %d\n", on);
		}
		else if(on)
		{
			if(!hwPowerOn(hw->power_id, hw->power_vol, "EM30918")) 
			{
				APS_ERR("power on fails!!\n");
			}
		}
		else
		{
			if(!hwPowerDown(hw->power_id, "EM30918")) 
			{
				APS_ERR("power off fail!!\n");   
			}
		}
	}

#ifdef GPIO_ALS_PWREN_PIN
    if(power_on == on)
    {
            APS_LOG("ignore power control: %d\n", on);
    }
    else if( on)
    {
        mt_set_gpio_mode(GPIO_ALS_PWREN_PIN, GPIO_ALS_PWREN_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_ALS_PWREN_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_ALS_PWREN_PIN, 1);
        msleep(100);
    }
    else
    {
        mt_set_gpio_mode(GPIO_ALS_PWREN_PIN, GPIO_ALS_PWREN_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_ALS_PWREN_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_ALS_PWREN_PIN, 0);
    }
#endif
	*/

	power_on = on;
}

/*----------------------------------------------------------------------------*/
static int em30918_enable_als(struct i2c_client *client, int enable)
{
        struct em30918_priv *obj = i2c_get_clientdata(client);
        u8 databuf[2];      
        int res = 0;
        //u8 reg_value[1];
        //u8 buffer[2];
        if(client == NULL)
        {
            APS_DBG("CLIENT CANN'T EQUL NULL\n");
            return -1;
        }

	      databuf[0] = EM30918_CMM_ENABLE;    
            res = i2c_master_send(client, databuf, 0x1);
            if(res <= 0)
            {
                goto EXIT_ERR;
            }
            res = i2c_master_recv(client, databuf, 0x1);
            if(res <= 0)
            {
                goto EXIT_ERR;
            }

        if(enable)
        {			
            databuf[1] = ((databuf[0] & 0xF8) | 0x06);
            databuf[0] = EM30918_CMM_ENABLE;    
            res = i2c_master_send(client, databuf, 0x2);
            if(res <= 0)
            {
                goto EXIT_ERR;
            }
           
            atomic_set(&obj->als_deb_on, 1);
            atomic_set(&obj->als_deb_end, jiffies+atomic_read(&obj->als_debounce)/(1000/HZ));
            APS_DBG("em30918 als power on\n");
        }
        else
        {
            databuf[1] = (databuf[0] & 0xF8);
            databuf[0] = EM30918_CMM_ENABLE;    
            res = i2c_master_send(client, databuf, 0x2);
            if(res <= 0)
            {
                goto EXIT_ERR;
            }
            atomic_set(&obj->als_deb_on, 0);
            APS_DBG("EM30918 als power off\n");
        }
		
        return 0;
        
    EXIT_ERR:
        APS_ERR("EM30918_enable_als fail\n");
        return res;
}
int em30918_read_ps(struct i2c_client *client, u16 *data)
{
	//struct em30918_priv *obj = i2c_get_clientdata(client);       
	u8 ps_value[1];
	u8 buffer[1];
	int res = 0;

	if(client == NULL)
	{
		APS_DBG("CLIENT CANN'T EQUL NULL\n");
		return -1;
	}

	buffer[0]=EM30918_CMM_PDATA;
	res = i2c_master_send(client, buffer, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	res = i2c_master_recv(client, ps_value, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	*data = ps_value[0];
	printk("bingo *******************ps_data=%d\n", *data);
	return 0;    

EXIT_ERR:
	APS_ERR("em30918_read_ps fail\n");
	return res;
}
/*----------------------------------------------------------------------------*/
static int em30918_enable_ps(struct i2c_client *client, int enable)
{
	struct em30918_priv *obj = i2c_get_clientdata(client);
	u8 databuf[2];    
	int res = 0;
	//report_val = 3;
	//u8 buffer[2];
//	u8 offset_reg;
	//struct hwm_sensor_data sensor_data;

	if(client == NULL)
	{
		APS_DBG("CLIENT CANN'T EQUL NULL\n");
		return -1;
	}
	
	APS_DBG("em30918_enable_ps, enable = %d\n", enable);

		databuf[0] = EM30918_CMM_ENABLE;	
		res = i2c_master_send(client, databuf, 0x1);
		if(res <= 0)
		{
			goto EXIT_ERR;
		}
		res = i2c_master_recv(client, databuf, 0x1);
		if(res <= 0)
		{
			goto EXIT_ERR;
		}

	if(enable)
	{
		databuf[1] = ((databuf[0] & 0x06)|PS_ENABLE);   
		databuf[0] = EM30918_CMM_ENABLE;    

		res = i2c_master_send(client, databuf, 0x2);
		if(res <= 0)
		{
			goto EXIT_ERR;
		}

		APS_DBG("em30918 ps power on\n");

		/*for interrup work mode support */
		if(0 == obj->hw->polling_mode_ps)
		{
		#if 0
				em30918_read_ps(client,&obj->ps);
				if(obj->ps > atomic_read(&obj->ps_thd_val))
				{
					report_val = 0;
				}	
				if(obj->ps < (atomic_read(&obj->ps_thd_val) - PS_LTHD_VAL))
				{
					report_val = 1;
				}
				sensor_data.values[0] = report_val;
				sensor_data.value_divide = 1;
				sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
			
				//if((res = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
				{
				 APS_ERR("report_val= %d; ps_data=%d\n", report_val,obj->ps);
				}
				#endif
			//mt_eint_unmask(CUST_EINT_ALS_NUM);
			enable_irq(em30918_obj->irq);
		}
	}
	else
	{
		databuf[1] = (databuf[0] & 0x06);
		
		databuf[0] = EM30918_CMM_ENABLE;    
                       
		res = i2c_master_send(client, databuf, 0x2);
		if(res <= 0)
		{
			goto EXIT_ERR;
		}

		atomic_set(&obj->ps_deb_on, 0);
		APS_DBG("em30918 ps power off\n");

		/*for interrup work mode support */
		if(0 == obj->hw->polling_mode_ps)
		{
			cancel_work_sync(&obj->eint_work);
			//mt_eint_mask(CUST_EINT_ALS_NUM);
			disable_irq(em30918_obj->irq);
		}
	}
	ps_enabled=enable;
	return 0;
	
EXIT_ERR:
	APS_ERR("em30918_enable_ps fail\n");
	return res;
}

/*----------------------------------------------------------------------------*/
/*for interrup work mode support --*/
static int em30918_check_and_clear_intr(struct i2c_client *client) 
{
	//struct em30918_priv *obj = i2c_get_clientdata(client);
	int res;
	u8 buffer[2];

	buffer[0] = EM30918_CMM_STATUS;
	res = i2c_master_send(client, buffer, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	res = i2c_master_recv(client, buffer, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	//APS_ERR("em30918_check_and_clear_intr status=0x%x\n", buffer[0]);
	if(0 != (buffer[0] & 0x80))
	{
		buffer[0] = EM30918_CMM_STATUS;
		buffer[1] = 0x00;
		res = i2c_master_send(client, buffer, 0x2);
		if(res <= 0)
		{
			goto EXIT_ERR;
		}
		res = 0;
	}

	return res;

EXIT_ERR:
	APS_ERR("em30918_check_and_clear_intr fail\n");
	return 1;
}
/*----------------------------------------------------------------------------*/

void em30918_eint_func(void)
{
	struct em30918_priv *obj = em30918_obj;
	if(!obj)
	{
		return;
	}
	
	schedule_work(&obj->eint_work);
}

static irqreturn_t em30918_eint_handler(int irq, void *desc)
{
	APS_ERR("em30918_eint_handler enter");
	em30918_eint_func();
	disable_irq_nosync(em30918_obj->irq);

	return IRQ_HANDLED;
}

/*----------------------------------------------------------------------------*/
/*for interrup work mode support --*/
int em30918_setup_eint(struct i2c_client *client)
{

#if defined(CONFIG_OF)
	u32 ints[2] = { 0, 0 };
#endif

	int ret;
	struct pinctrl *pinctrl;
	struct pinctrl_state *pins_default;
	struct pinctrl_state *pins_cfg;

	alspsPltFmDev = get_alsps_platformdev();

	pinctrl = devm_pinctrl_get(&alspsPltFmDev->dev);
	if (IS_ERR(pinctrl)) {
		ret = PTR_ERR(pinctrl);
		APS_ERR("Cannot find alsps pinctrl!\n");
	}
	pins_default = pinctrl_lookup_state(pinctrl, "pin_default");
	if (IS_ERR(pins_default)) {
		ret = PTR_ERR(pins_default);
		APS_ERR("Cannot find alsps pinctrl default!\n");

	}

	pins_cfg = pinctrl_lookup_state(pinctrl, "pin_cfg");
	if (IS_ERR(pins_cfg)) {
		ret = PTR_ERR(pins_cfg);
		APS_ERR("Cannot find alsps pinctrl pin_cfg!\n");

	}
	pinctrl_select_state(pinctrl, pins_cfg);

	if (em30918_obj->irq_node) {
		of_property_read_u32_array(em30918_obj->irq_node, "debounce", ints,
					   ARRAY_SIZE(ints));
		gpio_request(ints[0], "p-sensor");
		gpio_set_debounce(ints[0], ints[1]);
		APS_LOG("ints[0] = %d, ints[1] = %d!!\n", ints[0], ints[1]);

		em30918_obj->irq = irq_of_parse_and_map(em30918_obj->irq_node, 0);
		APS_LOG("cm36686_obj->irq = %d\n", em30918_obj->irq);
		if (!em30918_obj->irq) {
			APS_ERR("irq_of_parse_and_map fail!!\n");
			return -EINVAL;
		}
		if (request_irq
			(em30918_obj->irq, em30918_eint_handler, IRQF_TRIGGER_FALLING, "ALS-eint", NULL)) {
			APS_ERR("IRQ LINE NOT AVAILABLE!!\n");
			return -EINVAL;
		}

		enable_irq(em30918_obj->irq);
	} else {
		APS_ERR("null irq node!!\n");
		return -EINVAL;
	}

	return 0;
	
}

/****************************************************************************** 
**
*****************************************************************************/
int em30918_read_als(struct i2c_client *client, u16 *data)
{
	//struct em30918_priv *obj = i2c_get_clientdata(client);	 
	//u16 als_value;	 
	u8 als_value_low[1], als_value_high[1];
	u8 buffer[1];
	int res = 0;
	
	if(client == NULL)
	{
		APS_DBG("CLIENT CANN'T EQUL NULL\n");
		return -1;
	}

	buffer[0]=EM30918_CMM_C0DATA_L;
	res = i2c_master_send(client, buffer, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	res = i2c_master_recv(client, als_value_low, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	
	buffer[0]=EM30918_CMM_C0DATA_H;
	res = i2c_master_send(client, buffer, 0x1);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	res = i2c_master_recv(client, als_value_high, 0x01);
	if(res <= 0)
	{
		goto EXIT_ERR;
	}
	
	*data = als_value_low[0] | ((als_value_high[0])<<8);
	 
	printk("em30918 als value = %d\t\n",*data);
	return 0;	 

EXIT_ERR:
	APS_ERR("em30918_read_ps fail\n");
	return res;
}
/*----------------------------------------------------------------------------*/

static int em30918_get_als_value(struct em30918_priv *obj, u16 als)
{
	int idx;
	int invalid = 0;
	for(idx = 0; idx < obj->als_level_num; idx++)
	{
		if(als < obj->hw->als_level[idx])
		{
			break;
		}
	}
	
	if(idx >= obj->als_value_num)
	{
		APS_ERR("exceed range\n"); 
		idx = obj->als_value_num - 1;
	}
	
	if(1 == atomic_read(&obj->als_deb_on))
	{
		unsigned long endt = atomic_read(&obj->als_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->als_deb_on, 0);
		}
		
		if(1 == atomic_read(&obj->als_deb_on))
		{
			invalid = 1;
		}
	}

	if(!invalid)
	{
		APS_DBG("zjb ALS: %05d => %05d\n", als, obj->hw->als_value[idx]);	
		return obj->hw->als_value[idx];
	}
	else
	{
		APS_ERR(" zjb ALS: %05d => %05d (-1)\n", als, obj->hw->als_value[idx]);    
		return -1;
	}
}
/*----------------------------------------------------------------------------*/
//wt add for ps calibration start
static int em30918_read_data_for_cali(struct i2c_client *client, struct PS_CALI_DATA_STRUCT *ps_data_cali)
{
	//struct ltr559_priv *obj = i2c_get_clientdata(client);

	int i=0 ,err = 0,sum=0,data_cali=0;
	u16 data[21] = {0};
	
	for(i = 0;i<20;i++)
	{
		mdelay(100);//mdelay(5);fangliang
		if(!em30918_read_ps(client,&data[i]))
		{
			sum += data[i];
			//APS_LOG("[hjf++]data[%d] = %d\n", i, data);
		}
		else
		{
		break;
		}
	}
	if(i == 20)
	{
		data_cali = sum/20;
		//APS_LOG("[hjf++]em30918_read_data_for_cali data_cali = %d\n",data_cali);
		ps_data_cali->far_away = data_cali + CUST_EM30918_PS_THRES_FAR;
		ps_data_cali->close = data_cali + CUST_EM30918_PS_THRES_CLOSE;
		ps_data_cali->valid = 1;
		err= 0;
	}
	else
	{
            	ps_data_cali->valid = 0;
            	//APS_LOG("em30918_read_data_for_cali get data error!\n");
            	err=  -1;
	}

	return err;
}

static void em30918_WriteCalibration(struct i2c_client *client, struct PS_CALI_DATA_STRUCT *data_cali)
{
	struct em30918_priv *obj = i2c_get_clientdata(client);
	struct alsps_hw *hw = get_cust_alsps();
	const char *name = "mediatek,em30918";
	APS_FUN();
    APS_LOG("em30918_WriteCalibration() ...start!\n");
	hw =   get_alsps_dts_func(name, hw);
	if (!hw)
	{
		APS_ERR("get dts info fail\n");
	}
 	if(data_cali->valid == 1)
  	{
		atomic_set(&obj->ps_thd_val_high,  data_cali->close);
		atomic_set(&obj->ps_thd_val_low,  data_cali->far_away);
		ps_cali.valid = 1;
	}
	else if(data_cali->valid == 0){
		atomic_set(&obj->ps_thd_val_high,  hw->ps_threshold_high);
		atomic_set(&obj->ps_thd_val_low,  hw->ps_threshold_low);
		ps_cali.valid = 0;	
	}
		ps_cali.close = atomic_read(&obj->ps_thd_val_high);
		ps_cali.far_away = atomic_read(&obj->ps_thd_val_low);

}
//wt add for ps calibration end

/*----------------------------------------------------------------------------*/
static int em30918_get_ps_value(struct em30918_priv *obj, u16 ps)
{
	int val;
	int invalid = 0;
	static int val_temp=1;
	//int mask = atomic_read(&obj->ps_mask);
	printk("em30918  get ps %d\n",ps);
	if((ps > atomic_read(&obj->ps_thd_val_high)))
	{
		val = 0;  /*close*/
		val_temp = 0;
		intr_flag_value = 1;
	}
	else if((ps < (atomic_read(&obj->ps_thd_val_low))))
	{
		val = 1;  /*far away*/
		val_temp = 1;
		intr_flag_value = 0;
	}
	else
	       val = val_temp;	
				
	if(atomic_read(&obj->ps_suspend))
	{
		invalid = 1;
	}
	else if(1 == atomic_read(&obj->ps_deb_on))
	{
		unsigned long endt = atomic_read(&obj->ps_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->ps_deb_on, 0);
		}
		
		if (1 == atomic_read(&obj->ps_deb_on))
		{
			invalid = 1;
		}
	}

	if(!invalid)
	{
		return val;
	}	
	else
	{
		return -1;
	}	
}

/*for interrup work mode support --*/
static int em30918_init_client(struct i2c_client *client)
{
	struct em30918_priv *obj = i2c_get_clientdata(client);
	u8 databuf[2]/*,ps_data*/;    
	int res = 0,i=0,sum=0;
	//struct PS_CALI_DATA_STRUCT ps_cali_temp;
	  u8 offset_val;
	
	   databuf[0] = EM30918_CMM_ENABLE;	 
	   databuf[1] = PS_ENABLE;
	   res = i2c_master_send(client, databuf, 0x2);
	   if(res <= 0)
	   {
			APS_FUN();
		   goto EXIT_ERR;
	   }
	   databuf[0] = EM30918_CMM_STATUS;	 
	   databuf[1] = 0X00;
	   res = i2c_master_send(client, databuf, 0x2);
	   if(res <= 0)
	   {
			APS_FUN();
		   goto EXIT_ERR;
	   }
	   for(i=0;i<3;i++)
	   {
		   em30918_read_ps(obj->client, &obj->ps);
		   sum+=obj->ps;
		mdelay(100);
	   }
		offset_val =0x01;

	   databuf[0] = EM30918_CMM_OFFSET;	 
     	  databuf[1] = offset_val;
	   res = i2c_master_send(client, databuf, 0x2);
	   if(res <= 0)
	   {
	  	   APS_FUN();
		   goto EXIT_ERR;
	   }
	   	/*for interrup work mode support*/
		if(0 == obj->hw->polling_mode_ps)
		{
			databuf[0] = EM30918_CMM_INT_PS_LB;	
			databuf[1] = (u8)((atomic_read(&obj->ps_thd_val_low)));
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{  
				APS_FUN();
				goto EXIT_ERR;
			}
			databuf[0] = EM30918_CMM_INT_PS_HB;	
			databuf[1] = (u8)(atomic_read(&obj->ps_thd_val_high));
			res = i2c_master_send(client, databuf, 0x2);
			if(res <= 0)
			{
				APS_FUN();
				goto EXIT_ERR;
			}

		//mt65xx_eint_unmask(CUST_EINT_ALS_NUM);
		}

	/*for interrup work mode support */
	res = em30918_setup_eint(client);
	if(res )
	{
		APS_ERR("setup eint: %d\n", res);
		return res;
	}
	res = em30918_check_and_clear_intr(client);
	if(res)
	{
		APS_ERR("check/clear intr: %d\n", res);
		//    return res;
	}
	
	return EM30918_SUCCESS;

EXIT_ERR:
	APS_ERR("init dev: %d\n", res);
	return res;
}


static void em30918_eint_work(struct work_struct *work)
{
	struct em30918_priv *obj = (struct em30918_priv *)container_of(work, struct em30918_priv, eint_work);
	int err;
	//struct hwm_sensor_data sensor_data;
	int values = -1;
	//u8 buffer[1];
	u8 databuf[2];
	int res = 0;
	if((err = em30918_check_and_clear_intr(obj->client)))
	{
		APS_ERR("em30918_eint_work check intrs: %d\n", err);
	}
	else
	{
		//get raw data
		em30918_read_ps(obj->client, &obj->ps);
		//em30918_read_als(obj->client, &obj->als);
		APS_DBG("em30918_eint_work rawdata ps=%d \n",obj->ps);
		values = em30918_get_ps_value(obj, obj->ps);
	if(intr_flag_value){
			databuf[0] = EM30918_CMM_INT_PS_LB;	
			databuf[1] = (u8)((atomic_read(&obj->ps_thd_val_low)));
			res = i2c_master_send(obj ->client, databuf, 0x2);
			if(res <= 0)
			{  
				APS_FUN();
				//goto EXIT_ERR;
			}
			databuf[0] = EM30918_CMM_INT_PS_HB;	
			databuf[1] = 0xff;//(u8)(atomic_read(&obj->ps_thd_val_high));
			res = i2c_master_send(obj ->client, databuf, 0x2);
			if(res <= 0)
			{
				APS_FUN();
				//goto EXIT_ERR;
			}
		}else{

		databuf[0] = EM30918_CMM_INT_PS_LB; 
		databuf[1] = 0x00;//(u8)((atomic_read(&obj->ps_thd_val_low)));
		res = i2c_master_send(obj ->client, databuf, 0x2);
		if(res <= 0)
		{  
			APS_FUN();
			//goto EXIT_ERR;
		}
		databuf[0] = EM30918_CMM_INT_PS_HB; 
		databuf[1] = (u8)(atomic_read(&obj->ps_thd_val_high));
		res = i2c_master_send(obj ->client, databuf, 0x2);
		if(res <= 0)
		{
			APS_FUN();
			//goto EXIT_ERR;
		}

		}
		#if 0
		//sensor_data.values[0] = em30918_get_ps_value(obj, obj->ps);
		//sensor_data.value_divide = 1;
		//sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;			

		//if((err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
		{
	  		APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", values);
		}
		#endif
		ps_report_interrupt_data(values);
		
	}
	
	if((err = em30918_check_and_clear_intr(obj->client)))
	{
		APS_ERR("em30918_eint_work check intrs: %d\n", err);
		
	}
	//	em30918_read_ps(obj->client, &obj->ps);
	    enable_irq(em30918_obj->irq);   
	    return;
//EXIT_ERR:
		 //mt_eint_unmask(CUST_EINT_ALS_NUM);  
		//return;

}


/****************************************************************************** 
 * Function Configuration
******************************************************************************/
static int em30918_open(struct inode *inode, struct file *file)
{
	file->private_data = em30918_i2c_client;

	if (!file->private_data)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}
	
	return nonseekable_open(inode, file);
}
/*----------------------------------------------------------------------------*/
static int em30918_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*----------------------------------------------------------------------------*/

static long em30918_ioctl( struct file *file, unsigned int cmd,unsigned long arg)
{
	struct i2c_client *client = (struct i2c_client*)file->private_data;
	struct em30918_priv *obj = i2c_get_clientdata(client);  
	int err = 0;
	void __user *ptr = (void __user*) arg;
	int dat;
	uint32_t enable;
	//wt add for ps calibration start
	struct PS_CALI_DATA_STRUCT ps_cali_temp;
	//wt add for ps calibration end

	switch (cmd)
	{
		case ALSPS_SET_PS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				err = em30918_enable_ps(obj->client, 1);
				if(err )
				{
					APS_ERR("enable ps fail: %d\n", err); 
					goto err_out;
				}
				
				set_bit(CMC_BIT_PS, &obj->enable);
			}
			else
			{
				err = em30918_enable_ps(obj->client, 0);
				if(err)
				{
					APS_ERR("disable ps fail: %d\n", err); 
					goto err_out;
				}
				
				clear_bit(CMC_BIT_PS, &obj->enable);
			}
			break;

		case ALSPS_GET_PS_MODE:
			enable = test_bit(CMC_BIT_PS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_PS_DATA:
			err = em30918_read_ps(obj->client, &obj->ps);
			if(err)
			{
				goto err_out;
			}
			
			dat = em30918_get_ps_value(obj, obj->ps);
			if(dat == -1)
			{
				err = -EFAULT;
				goto err_out;
			}
			
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
			break;
			   //wt add for ps calibration start
		case ALSPS_SET_PS_CALI:
			if(ptr == NULL)
			{
				//APS_LOG("ptr == NULL\n");
				err = -EINVAL;
				break;	  
			}
			if(copy_from_user(&ps_cali_temp, ptr, sizeof(ps_cali_temp)))
			{
				//APS_LOG("copy_from_user is fail\n");
				err = -EFAULT;
				break;	  
			}
			em30918_WriteCalibration(obj->client,&ps_cali_temp);
		   // APS_LOG(" ALSPS_SET_PS_CALI %d,%d,%d\t",ps_cali_temp.close,ps_cali_temp.far_away,ps_cali_temp.valid);
			break;
		case ALSPS_GET_PS_RAW_DATA_FOR_CALI:
			em30918_enable_ps(obj->client, 1);
			em30918_read_data_for_cali(obj->client,&ps_cali_temp);
			if(copy_to_user(ptr, &ps_cali_temp, sizeof(ps_cali_temp)))
			{
				err = -EFAULT;
				goto err_out;
			}			   
			break;
		//wt add for ps calibration end

		case ALSPS_GET_PS_RAW_DATA:
			err = em30918_read_ps(obj->client, &obj->ps);
			if(err)
			{
				goto err_out;
			}
			
			dat = obj->ps;
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
			break;              

		case ALSPS_SET_ALS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				err = em30918_enable_als(obj->client, 1);
				if(err)
				{
					APS_ERR("enable als fail: %d\n", err); 
					goto err_out;
				}
				set_bit(CMC_BIT_ALS, &obj->enable);
			}
			else
			{
				err = em30918_enable_als(obj->client, 0);
				if(err)
				{
					APS_ERR("disable als fail: %d\n", err); 
					goto err_out;
				}
				clear_bit(CMC_BIT_ALS, &obj->enable);
			}
			break;

		case ALSPS_GET_ALS_MODE:
			enable = test_bit(CMC_BIT_ALS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_ALS_DATA: 
			err = em30918_read_als(obj->client, &obj->als);
			if(err)
			{
				goto err_out;
			}

			dat = em30918_get_als_value(obj, obj->als);
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}              
			break;

		case ALSPS_GET_ALS_RAW_DATA:    
			err = em30918_read_als(obj->client, &obj->als);
			if(err)
			{
				goto err_out;
			}

			dat = obj->als;
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}              
			break;
		default:
			APS_ERR("%s not supported = 0x%04x", __FUNCTION__, cmd);
			err = -ENOIOCTLCMD;
			break;
	}

	err_out:
	return err;    
}
/*----------------------------------------------------------------------------*/
static struct file_operations em30918_fops = {
	.owner = THIS_MODULE,
	.open = em30918_open,
	.release = em30918_release,
	.unlocked_ioctl = em30918_ioctl,
};
/*----------------------------------------------------------------------------*/
static struct miscdevice em30918_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "als_ps",
	.fops = &em30918_fops,
};
/*----------------------------------------------------------------------------*/
static int em30918_i2c_suspend(struct i2c_client *client, pm_message_t msg) 
{
	APS_FUN(); 
	return 0;
}
/*----------------------------------------------------------------------------*/
static int em30918_i2c_resume(struct i2c_client *client)
{
//	struct em30918_priv *obj = i2c_get_clientdata(client);        
	APS_FUN();
	return 0;
}
/*----------------------------------------------------------------------------*/
#ifdef CONFIG_HAS_EARLYSUSPEND
static void em30918_early_suspend(struct early_suspend *h) 
{   /*early_suspend is only applied for ALS*/
	struct em30918_priv *obj = container_of(h, struct em30918_priv, early_drv);   
	int err;
	APS_FUN();    

	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}

	atomic_set(&obj->als_suspend, 1);
	
	mutex_lock(&sensor_lock);
	err = em30918_enable_als(obj->client, 0);
	if(err)
	{
		APS_ERR("disable als fail: %d\n", err); 
	}
	mutex_unlock(&sensor_lock);

}
/*----------------------------------------------------------------------------*/
static void em30918_late_resume(struct early_suspend *h)
{   /*late_resume is only applied for ALS*/
	struct em30918_priv *obj = container_of(h, struct em30918_priv, early_drv);         
	int err;
	APS_FUN();

	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}
	
	atomic_set(&obj->als_suspend, 0);
	if(test_bit(CMC_BIT_ALS, &obj->enable))
	{
		mutex_lock(&sensor_lock);	
		err = em30918_enable_als(obj->client, 1);
		if(err)
		{
			APS_ERR("enable als fail: %d\n", err);        
		}
		
		mutex_unlock(&sensor_lock);
	}

}
#endif

int em30918_ps_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	struct hwm_sensor_data* sensor_data;
	struct em30918_priv *obj = (struct em30918_priv *)self;
	
	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{				
				value = *(int *)buff_in;
				if(value)
				{
				
					mutex_lock(&sensor_lock);
					err = em30918_enable_ps(obj->client, 1);
					if(err)
					{
						APS_ERR("enable ps fail: %d\n", err); 
						
						mutex_unlock(&sensor_lock);
						return -1;
					}
					mutex_unlock(&sensor_lock);
					
					set_bit(CMC_BIT_PS, &obj->enable);
					
				}
				else
				{
				
					mutex_lock(&sensor_lock);
					err = em30918_enable_ps(obj->client, 0);
					if(err)
					{
						APS_ERR("disable ps fail: %d\n", err); 
						
						mutex_unlock(&sensor_lock);
						return -1;
					}
					mutex_unlock(&sensor_lock);
					clear_bit(CMC_BIT_PS, &obj->enable);
					
				}
			}
			break;

		case SENSOR_GET_DATA:
			if((buff_out == NULL) || (size_out< sizeof(struct hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				sensor_data = (struct hwm_sensor_data *)buff_out;	
				mutex_lock(&sensor_lock);
				em30918_read_ps(obj->client, &obj->ps);			
				mutex_unlock(&sensor_lock);
				APS_ERR("e3071_ps_operate ps data=%d!\n",obj->ps);
				sensor_data->values[0] = em30918_get_ps_value(obj, obj->ps);
				sensor_data->value_divide = 1;
				sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;			
			}
			break;
		default:
			APS_ERR("proxmy sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}

int em30918_als_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	struct hwm_sensor_data* sensor_data;
	struct em30918_priv *obj = (struct em30918_priv *)self;

	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;				
				if(value)
				{
					mutex_lock(&sensor_lock);
					err = em30918_enable_als(obj->client, 1);
					if(err)
					{
						APS_ERR("enable als fail: %d\n", err); 
						mutex_unlock(&sensor_lock);
						return -1;
					}
					mutex_unlock(&sensor_lock);
					set_bit(CMC_BIT_ALS, &obj->enable);
				}
				else
				{
					mutex_lock(&sensor_lock);
					err = em30918_enable_als(obj->client, 0);
					if(err)
					{
						mutex_unlock(&sensor_lock);
						APS_ERR("disable als fail: %d\n", err); 
						return -1;
					}
					mutex_unlock(&sensor_lock);
					clear_bit(CMC_BIT_ALS, &obj->enable);
				}
				
			}
			break;

		case SENSOR_GET_DATA:
			if((buff_out == NULL) || (size_out< sizeof(struct hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				sensor_data = (struct hwm_sensor_data *)buff_out;
				
				mutex_lock(&sensor_lock);
				em30918_read_als(obj->client, &obj->als);
				mutex_unlock(&sensor_lock);
								
				sensor_data->values[0] = em30918_get_als_value(obj, obj->als);
				sensor_data->value_divide = 1;
				sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
			}
			break;
		default:
			APS_ERR("light sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}


/*----------------------------------------------------------------------------*/
static int em30918_i2c_detect(struct i2c_client *client, struct i2c_board_info *info) 
{    
	strcpy(info->type, EM30918_DEV_NAME);
	return 0;
}

/*----------------------------------------------------------------------------*/

//Gionee BSP1 yaoyc 20151215 add for em30918 before
static int als_open_report_data(int open)
{
	/* should queuq work to report event if  is_report_input_direct=true */
	return 0;
}

/* if use  this typ of enable , Gsensor only enabled but not report inputEvent to HAL */

static int als_enable_nodata(int en)
{
	//int res = 0;
	//0112 add
	int err = 0;
	//u8 databuf[2];  
	APS_LOG("em30918 als enable value = %d\n", en);
	if(em30918_obj == NULL){
		APS_ERR("em30918_obj is null\n");
		return -1;
	}

		if(en)
		{
			mutex_lock(&sensor_lock);
			err = em30918_enable_als(em30918_obj->client, 1);
			if(err)
			{
				APS_ERR("enable ps fail: %d\n", err); 
				
				mutex_unlock(&sensor_lock);
				return -1;
			}
			mutex_unlock(&sensor_lock);
			
			set_bit(CMC_BIT_PS, &em30918_obj->enable);
			
		}
		else
		{
		
			mutex_lock(&sensor_lock);
			err = em30918_enable_als(em30918_obj->client, 0);
			if(err)
			{
				APS_ERR("disable ps fail: %d\n", err); 
				
				mutex_unlock(&sensor_lock);
				return -1;
			}
			mutex_unlock(&sensor_lock);
			clear_bit(CMC_BIT_PS, &em30918_obj->enable);
			
		}


	return 0;
}

static int als_set_delay(u64 ns)
{
	return 0;
}

static int als_get_data(int *value, int *status)
{
	int err = 0;

	APS_LOG("em30918 als get data \n");
	//0112 add
	if(em30918_obj == NULL){
		APS_ERR("em30918_obj is null\n");
		return -1;
	}
	mutex_lock(&sensor_lock);
	err = em30918_read_als(em30918_obj->client, &em30918_obj->als);			
	mutex_unlock(&sensor_lock);

	*value = em30918_get_als_value(em30918_obj, em30918_obj->als);
	*status = SENSOR_STATUS_ACCURACY_MEDIUM;

	APS_LOG("em30918 als_get_data, value = %d\n",*value);
	//0112 end
	return err;
}

/* if use  this typ of enable , Gsensor should report inputEvent(x, y, z ,stats, div) to HAL */
static int ps_open_report_data(int open)
{
	/* should queuq work to report event if  is_report_input_direct=true */
	return 0;
}

/* if use  this typ of enable , Gsensor only enabled but not report inputEvent to HAL */

static int ps_enable_nodata(int en)
{
	//int res = 0;
	int err = 0;
	APS_LOG("em30918 als enable value = %d\n", en);
	if(em30918_obj == NULL){
		APS_ERR("em30918_obj is null\n");
		return -1;
	}
	if(en)
	{
		mutex_lock(&sensor_lock);
		err = em30918_enable_ps(em30918_obj->client, 1);
		if(err)
		{
			APS_ERR("enable ps fail: %d\n", err); 
			
			mutex_unlock(&sensor_lock);
			return -1;
		}
		mutex_unlock(&sensor_lock);
		
		set_bit(CMC_BIT_PS, &em30918_obj->enable);
		
	}
	else
	{
	
		mutex_lock(&sensor_lock);
		err = em30918_enable_ps(em30918_obj->client, 0);
		if(err)
		{
			APS_ERR("disable ps fail: %d\n", err); 
			
			mutex_unlock(&sensor_lock);
			return -1;
		}
		mutex_unlock(&sensor_lock);
		clear_bit(CMC_BIT_PS, &em30918_obj->enable);
		
	}
	return 0;

}

static int ps_set_delay(u64 ns)
{
	return 0;
}

static int ps_get_data(int *value, int *status)
{
	int err = 0;
	APS_LOG("em30918 ps get data \n");

	if(em30918_obj == NULL){
		APS_ERR("em30918_obj is null\n");
		return -1;
	}
	mutex_lock(&sensor_lock);
	err = em30918_read_ps(em30918_obj->client, &em30918_obj->ps);			
	mutex_unlock(&sensor_lock);
	if( err == 0){
		*value = em30918_get_ps_value(em30918_obj, em30918_obj->ps);
		if(*value < 0 )
			err = -1;
		*status = SENSOR_STATUS_ACCURACY_MEDIUM;
	}
	APS_LOG("em30918 ps_get_data, value = %d\n",*value);

	return err;
}

//Gionee BSP1 yaoyc 20151215 add for em30918 end

static int em30918_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct em30918_priv *obj;
	//struct hwmsen_object obj_ps, obj_als;
	int err = 0;
	//Gionee BSP1 yaoyc 20151215 add for em30918 before
	struct als_control_path als_ctl = { 0 };
	struct als_data_path als_data = { 0 };
	struct ps_control_path ps_ctl = { 0 };
	struct ps_data_path ps_data = { 0 };
	//Gionee BSP1 yaoyc 20151215 add for em30918 end

	if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL)))
	{
		err = -ENOMEM;
		goto exit;
	}
	memset(obj, 0, sizeof(*obj));
	em30918_obj = obj;
	
	APS_LOG("em30918_init_client() in!\n");
	obj->hw = get_cust_alsps();
	em30918_get_addr(obj->hw, &obj->addr);
	printk("em30918 probe %s\n",__func__);
	/*for interrup work mode support --*/
	INIT_WORK(&obj->eint_work, em30918_eint_work);
	obj->client = client;
	i2c_set_clientdata(client, obj);	
	atomic_set(&obj->als_debounce, 300);
	atomic_set(&obj->als_deb_on, 0);
	atomic_set(&obj->als_deb_end, 0);
	atomic_set(&obj->ps_debounce, 300);
	atomic_set(&obj->ps_deb_on, 0);
	atomic_set(&obj->ps_deb_end, 0);
	atomic_set(&obj->ps_mask, 0);
	atomic_set(&obj->als_suspend, 0);
	atomic_set(&obj->als_cmd_val, 0xDF);
	atomic_set(&obj->ps_cmd_val,  0xC1);
	
	atomic_set(&obj->ps_thd_val,  0x0A);   //0x50
	atomic_set(&obj->ps_thd_val_high, obj->hw->ps_threshold_high);
	atomic_set(&obj->ps_thd_val_low, obj->hw->ps_threshold_low);
	atomic_set(&obj->ps_thd_val_ld_high, obj->hw->ps_threshold_ld_high);
	atomic_set(&obj->ps_thd_val_ld_low, obj->hw->ps_threshold_ld_low);
	obj->enable = 0;
	obj->pending_intr = 0;
	obj->als_level_num = sizeof(obj->hw->als_level)/sizeof(obj->hw->als_level[0]);
	obj->als_value_num = sizeof(obj->hw->als_value)/sizeof(obj->hw->als_value[0]);  
	
	obj->irq_node = of_find_compatible_node(NULL, NULL, "mediatek, ALS-eint");
	
	BUG_ON(sizeof(obj->als_level) != sizeof(obj->hw->als_level));
	memcpy(obj->als_level, obj->hw->als_level, sizeof(obj->als_level));
	BUG_ON(sizeof(obj->als_value) != sizeof(obj->hw->als_value));
	memcpy(obj->als_value, obj->hw->als_value, sizeof(obj->als_value));
	atomic_set(&obj->i2c_retry, 3);
	set_bit(CMC_BIT_ALS, &obj->enable);
	set_bit(CMC_BIT_PS, &obj->enable);
	
	em30918_i2c_client = client;
	err = em30918_init_client(client);
	if(err)
	{
		goto exit_init_failed;
	}
	APS_LOG("em30918_init_client() OK!\n");
	

	err = misc_register(&em30918_device);

	if(err)
	{
		APS_ERR("em30918_device register failed\n");
		goto exit_misc_device_register_failed;
	}
	err = em30918_create_attr(&(em30918_init_info.platform_diver_addr->driver));
	if(err)
	{
		goto exit_create_attr_failed;
	}
	

//Gionee BSP1 yaoyc 20151215 add for em30918 before
	als_ctl.open_report_data = als_open_report_data;
	als_ctl.enable_nodata = als_enable_nodata;
	als_ctl.set_delay = als_set_delay;
	als_ctl.is_report_input_direct = false;
	als_ctl.is_support_batch = false;

	err = als_register_control_path(&als_ctl);
	if (err) {
		APS_ERR("register fail = %d\n", err);
		goto exit;
	}

	als_data.get_data = als_get_data;
	als_data.vender_div = 100;
	err = als_register_data_path(&als_data);
	if (err) {
		APS_ERR("tregister fail = %d\n", err);
		goto exit;
	}

	ps_ctl.open_report_data = ps_open_report_data;
	ps_ctl.enable_nodata = ps_enable_nodata;
	ps_ctl.set_delay = ps_set_delay;
	ps_ctl.is_report_input_direct = false;
	ps_ctl.is_support_batch = false;
	ps_ctl.is_polling_mode = obj->hw->polling_mode_ps;

	err = ps_register_control_path(&ps_ctl);
	if (err) {
		APS_ERR("register fail = %d\n", err);
		goto exit;
	}

	ps_data.get_data = ps_get_data;
	ps_data.vender_div = 100;
	err = ps_register_data_path(&ps_data);
	if (err) {
		APS_ERR("tregister fail = %d\n", err);
		goto exit;
	}

	err = batch_register_support_info(ID_LIGHT, als_ctl.is_support_batch, 1, 0);
	if (err)
		APS_ERR("register light batch support err = %d\n", err);

	err = batch_register_support_info(ID_PROXIMITY, ps_ctl.is_support_batch, 1, 0);
	if (err)
		APS_ERR("register proximity batch support err = %d\n", err);

//Gionee BSP1 yaoyc 20151215 add for em30918 end


#if defined(CONFIG_HAS_EARLYSUSPEND)
	obj->early_drv.level    = EARLY_SUSPEND_LEVEL_DISABLE_FB - 1,
	obj->early_drv.suspend  = em30918_early_suspend,
	obj->early_drv.resume   = em30918_late_resume,    
	register_early_suspend(&obj->early_drv);
#endif
	em30918_init_flag = 0;
	printk("em30918 probe %s OK\n",__func__);
	APS_LOG("%s: OK\n", __func__);
	return 0;

	exit_create_attr_failed:
	misc_deregister(&em30918_device);
	exit_misc_device_register_failed:
	exit_init_failed:
	exit:
	em30918_i2c_client = NULL;    
	em30918_init_flag = -1;
//	MT6516_EINTIRQMask(CUST_EINT_ALS_NUM);  /*mask interrupt if fail*/


	APS_ERR("%s: err = %d\n", __func__, err);
	return err;
}
/*----------------------------------------------------------------------------*/
static int em30918_i2c_remove(struct i2c_client *client)
{
	int err;
	err = em30918_delete_attr(&em30918_alsps_driver.driver);
	if(err)
	{
		APS_ERR("em30918_delete_attr fail: %d\n", err);
	} 	
	err = misc_deregister(&em30918_device);
	if(err)
	{
		APS_ERR("misc_deregister fail: %d\n", err);    
	}
	
	em30918_i2c_client = NULL;
	i2c_unregister_device(client);
	kfree(i2c_get_clientdata(client));

	return 0;
}
/*----------------------------------------------------------------------------*/
#if 0

static int em30918_probe(struct platform_device *pdev) 
{
	struct alsps_hw *hw = get_cust_alsps();

	em30918_power(hw, 1);    
	if(i2c_add_driver(&em30918_i2c_driver))
	{
		APS_ERR("add driver error\n");
		return -1;
	} 
	return 0;
}
/*----------------------------------------------------------------------------*/
static int em30918_remove(void)
{
	struct alsps_hw *hw = get_cust_alsps();
	APS_FUN();    
	em30918_power(hw, 0);    
	i2c_del_driver(&em30918_i2c_driver);
	return 0;
}
/*----------------------------------------------------------------------------*/

static struct platform_driver em30918_alsps_driver = {
	.probe      = em30918_probe,
	.remove     = em30918_remove,    
	.driver     = {
		.name  = "als_ps",
		.owner = THIS_MODULE,
	}
};
#endif
/*----------------------------------------------------------------------------*/

static int em30918_local_init(void) 
{
	struct alsps_hw *hw = get_cust_alsps();
	struct em30918_i2c_addr addr;
	
	em30918_power(hw, 1);    
	em30918_get_addr(hw, &addr);
	if(i2c_add_driver(&em30918_i2c_driver))
	{
		APS_ERR("add driver error\n");
		return -1;
	} 
	if(-1 == em30918_init_flag)
	{
	   return -1;
	}
	
	return 0;
}

static int em30918_local_uninit(void)
{
	struct alsps_hw *hw = get_cust_alsps();
	APS_FUN();    
	em30918_power(hw, 0);    
	i2c_del_driver(&em30918_i2c_driver);
	return 0;
}

static int __init em30918_init(void)
{
	const char *name = "mediatek,em30918";

	hw = get_alsps_dts_func(name, hw);
	
	//APS_LOG("%s: i2c_number=%d\n", __func__,hw->i2c_num); 

	//i2c_register_board_info(hw->i2c_num, &i2c_em30918, 1);
	alsps_driver_add(&em30918_init_info);

	return 0;
}
/*----------------------------------------------------------------------------*/
static void __exit em30918_exit(void)
{
	APS_FUN();
	//platform_driver_unregister(&em30918_alsps_driver);  
}
/*----------------------------------------------------------------------------*/
module_init(em30918_init);     
module_exit(em30918_exit);   
/*----------------------------------------------------------------------------*/
MODULE_AUTHOR("binghua chen@epticore.com");
MODULE_DESCRIPTION("em30918 driver");
MODULE_LICENSE("GPL");
