#include <linux/string.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <asm-generic/gpio.h>

#include "lcm_drv.h"
#include "ddp_irq.h"

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/gpio.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>
#endif

static struct regulator *lcm_vgp;
 unsigned int GPIO_LCD_RST_EN;

/* get LDO supply */
static int lcm_get_vgp_supply(struct device *dev)
{
	int ret;
	struct regulator *lcm_vgp_ldo;

	pr_debug("LCM: lcm_get_vgp_supply is going\n");

	lcm_vgp_ldo = devm_regulator_get(dev, "reg-lcm");
	if (IS_ERR(lcm_vgp_ldo)) {
		ret = PTR_ERR(lcm_vgp_ldo);
		dev_err(dev, "failed to get reg-lcm LDO, %d\n", ret);
		return ret;
	}

	pr_debug("LCM: lcm get supply ok.\n");
	ret = regulator_enable(lcm_vgp_ldo);
	/* get current voltage settings */
	ret = regulator_get_voltage(lcm_vgp_ldo);
	pr_debug("lcm LDO voltage = %d in LK stage\n", ret);

	lcm_vgp = lcm_vgp_ldo;

	return ret;
}

 int lcm_vgp_supply_enable(void)
{
	int ret;
	unsigned int volt;

	pr_debug("LCM: lcm_vgp_supply_enable\n");

	if (NULL == lcm_vgp)
		return 0;

	pr_debug("LCM: set regulator voltage lcm_vgp voltage to 2.8V\n");
	/* set voltage to 1.8V */
	ret = regulator_set_voltage(lcm_vgp, 3300000, 3300000);
	if (ret != 0) {
		pr_err("LCM: lcm failed to set lcm_vgp voltage: %d\n", ret);
		return ret;
	}

	/* get voltage settings again */
	volt = regulator_get_voltage(lcm_vgp);
	if (volt == 3300000)
		pr_err("LCM: check regulator voltage=3300000 pass!\n");
	else
		pr_err("LCM: check regulator voltage=3300000 fail! (voltage: %d)\n", volt);

	ret = regulator_enable(lcm_vgp);
	if (ret != 0) {
		pr_err("LCM: Failed to enable lcm_vgp: %d\n", ret);
		return ret;
	}

	return ret;
}

 int lcm_vgp_supply_disable(void)
{
	int ret = 0;
	unsigned int isenable;

	if (NULL == lcm_vgp)
		return 0;
	pr_debug("LCM: lcm_vgp_supply_disable\n");

	/* disable regulator */
	isenable = regulator_is_enabled(lcm_vgp);

	pr_debug("LCM: lcm query regulator enable status[0x%d]\n", isenable);

//	if (isenable) {
		ret = regulator_disable(lcm_vgp);
		if (ret != 0) {
			pr_err("LCM: lcm failed to disable lcm_vgp: %d\n", ret);
			return ret;
		}
		/* verify */
		isenable = regulator_is_enabled(lcm_vgp);
		if (!isenable)
			pr_err("LCM: lcm regulator disable pass\n");
//	}

	return ret;
}

static void lcm_get_gpio_infor(void)
{
	static struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "mediatek,lcm");

  GPIO_LCD_RST_EN = of_get_named_gpio(node, "lcm_reset_gpio", 0);
}

void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	gpio_direction_output(GPIO, output);
	gpio_set_value(GPIO, output);
}

static int lcm_probe(struct device *dev)
{
	lcm_get_vgp_supply(dev);
	lcm_get_gpio_infor();
	return 0;
}

static const struct of_device_id lcm_of_ids[] = {
	{.compatible = "mediatek,lcm",},
	{}
};

static struct platform_driver lcm_driver = {
	.driver = {
		   .name = "mtk_lcm",
		   .owner = THIS_MODULE,
		   .probe = lcm_probe,
#ifdef CONFIG_OF
		   .of_match_table = lcm_of_ids,
#endif
		   },
};

static int __init lcm_init(void)
{
	pr_debug("LCM: Register lcm driver\n");
	if (platform_driver_register(&lcm_driver)) {
		pr_err("LCM: failed to register disp driver\n");
		return -ENODEV;
	}

	return 0;
}

static void __exit lcm_exit(void)
{
	platform_driver_unregister(&lcm_driver);
	pr_debug("LCM: Unregister lcm driver done\n");
}
late_initcall(lcm_init);
module_exit(lcm_exit);
MODULE_AUTHOR("mediatek");
MODULE_DESCRIPTION("Display subsystem Driver");
MODULE_LICENSE("GPL");


/**
 * Local Constants
 */
#define FRAME_WIDTH		(600)
#define FRAME_HEIGHT		(1024)

#define REGFLAG_DELAY		0xFE
#define REGFLAG_END_OF_TABLE	0xFF   /* END OF REGISTERS MARKER */


#define LCM_DSI_CMD_MODE	0


static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)	(lcm_util.set_reset_pin((v)))
#define MDELAY(n) (lcm_util.mdelay(n))
#define UDELAY(n) (lcm_util.udelay(n))



/**
 * Local Functions
 */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)	lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)				lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)	lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg				lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)	lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#ifndef ASSERT
#define ASSERT(expr)					\
	do {						\
		if (expr)				\
			break;				\
		pr_debug("DDP ASSERT FAILED %s, %d\n",	\
		       __FILE__, __LINE__);		\
		BUG();					\
	} while (0)
#endif

struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[64];
};

static void init_lcm_registers(void)
{
	
		unsigned int data_array[16];
	//------------------B9h----------------//
	data_array[0] = 0x00043902;
	data_array[1] = 0x9483FFB9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	//------------------BAh----------------//
	data_array[0] = 0x00033902;
	data_array[1] = 0x008373BA;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	//------------------B1h----------------//
	data_array[0] = 0x00103902;
	data_array[1] = 0x0E0E6CB1;
	data_array[2] = 0xEF0F0425;
	data_array[3] = 0x23DA6801;
	data_array[4] = 0x588AC086;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(5);
	//------------------B2h----------------//
	data_array[0] = 0x00103902;
	data_array[1] = 0x0F4485B2;
	data_array[2] = 0x081C2409;
	data_array[3] = 0x004D1C08;
	data_array[4] = 0x48443000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	//------------------B4h----------------//
	data_array[0] = 0x000D3902;
	data_array[1] = 0x59FF00B4;
	data_array[2] = 0x595A595A;
	data_array[3] = 0x0176015A;
	data_array[4] = 0x00000076;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
			//------------------BFh----------------//
	data_array[0] = 0x00043902; 										 
	data_array[1] = 0x010E41BF; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
			//------------------D2h----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000000D2; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
			//------------------D3h----------------//
	data_array[0] = 0x001F3902; 				 
	data_array[1] = 0x000000D3;   
	data_array[2] = 0x0C0C0701;
	data_array[3] = 0x00071032;
	data_array[4] = 0x0A200005;
	data_array[5] = 0x32000905;
	data_array[6] = 0x21000810;
	data_array[7] = 0x23070821;
	data_array[8] = 0x0047070D;
	dsi_set_cmdq(data_array,9, 1);
	MDELAY(1);
			//------------------D5h----------------//
	data_array[0] = 0x002D3902; 				 
	data_array[1] = 0x010001D5;   
	data_array[2] = 0x03020300;
	data_array[3] = 0x21202102;
	data_array[4] = 0x18021820;
	data_array[5] = 0x18181802;
	data_array[6] = 0x18181818;
	data_array[7] = 0x18181818;
	data_array[8] = 0x18181818;
	data_array[9] = 0x18181818;
	data_array[10] = 0x18181818;
	data_array[11] = 0x18181818;
	data_array[12] = 0x00000018;
	dsi_set_cmdq(data_array,13, 1);
	MDELAY(1);
			 //------------------BDh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000002BD; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
			 //------------------D8h----------------//
	data_array[0] = 0x000D3902; 				 
	data_array[1] = 0xAAFFFFD8;   
	data_array[2] = 0xFFA0AAAA;
	data_array[3] = 0xAAAAAAFF;
	data_array[4] = 0x000000A0;
	dsi_set_cmdq(data_array,5, 1);
	MDELAY(1);
			 //------------------E0h----------------//
	data_array[0]=0x002B3902;
data_array[1]=0x0A0500E0;
data_array[2]=0x183D2E28;
data_array[3]=0x0C0A0736;
data_array[4]=0x14110E17;
data_array[5]=0x12071312;
data_array[6]=0x05001813;
data_array[7]=0x3D2E280A;
data_array[8]=0x0A073618;
data_array[9]=0x110E170C;
data_array[10]=0x07131214;
data_array[11]=0x00181312;
	dsi_set_cmdq(data_array,12, 1);
	MDELAY(1);
		   //------------------BDh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000000BD; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
			//------------------C1h----------------//
	data_array[0]=0x002C3902;
data_array[1]=0x080001C1;
data_array[2]=0x28211910;
data_array[3]=0x48403830;
data_array[4]=0x675F574F;
data_array[5]=0x87807870;
data_array[6]=0xA79F978F;
data_array[7]=0xC8C0B8B0;
data_array[8]=0xE8E0D8D0;
data_array[9]=0x2CFFF8F0;
data_array[10]=0x2AF2C67B;
data_array[11]=0xC0593AA0;
dsi_set_cmdq(data_array,12, 1);
	MDELAY(1);
		   //------------------BDh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000001BD; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
		  //------------------C1h----------------//
	data_array[0]=0x002B3902;
data_array[1]=0x0F0700C1;
data_array[2]=0x2C251E16;
data_array[3]=0x48413A32;
data_array[4]=0x655D564F;
data_array[5]=0x827B746C;
data_array[6]=0x9E979089;
data_array[7]=0xBCB5ADA6;
data_array[8]=0xD9D2CCC4;
data_array[9]=0x0722E9E1;
data_array[10]=0x9310B2F0;
data_array[11]=0x00003DD8;

	dsi_set_cmdq(data_array,12, 1);
	MDELAY(1);
		  //------------------BDh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000002BD; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
		  //------------------C1h----------------//
	data_array[0]=0x002B3902;
data_array[1]=0x0D0700C1;
data_array[2]=0x2A231B14;
data_array[3]=0x47413931;
data_array[4]=0x645D554E;
data_array[5]=0x817A736C;
data_array[6]=0x9E978F88;
data_array[7]=0xBCB4ADA5;
data_array[8]=0xD8D1CBC3;
data_array[9]=0xEF0FE6DF;
data_array[10]=0xC67AC4C4;
data_array[11]=0x00407E71;
	dsi_set_cmdq(data_array,12, 1);
	MDELAY(1);
			//------------------CCh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000009CC; 					   
	dsi_set_cmdq(data_array, 2, 1); 	  
	MDELAY(1);
		  //------------------C7h----------------//
	data_array[0] = 0x00053902; 										 
	data_array[1] = 0x40C000C7;
	data_array[2] = 0x000000C0; 							   
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
			//------------------C0h----------------//
	data_array[0] = 0x00033902; 										 
	data_array[1] = 0x001430C0; 					   
	dsi_set_cmdq(data_array, 2, 1); 	  
	MDELAY(1);
		  //------------------BCh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000007BC; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(10);

				//------------------35h----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000035; 					   
	dsi_set_cmdq(data_array, 2, 1); 	  
	MDELAY(5);
			//------------------11h----------------//				
	data_array[0] = 0x00110500; 						 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(200);	   
		  //------------------C6h----------------//
	data_array[0] = 0x00033902; 										 
	data_array[1] = 0x0000BDC6; 					   
	dsi_set_cmdq(data_array, 2, 1); 	  
	MDELAY(5);
			//------------------29h----------------//	
	data_array[0] = 0x00290500; 						 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);
	}
static void init_lcm_registers_delay_1000ms(void)
{
	
		unsigned int data_array[16];
	//------------------B9h----------------//
	data_array[0] = 0x00043902;
	data_array[1] = 0x9483FFB9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	//------------------BAh----------------//
	data_array[0] = 0x00033902;
	data_array[1] = 0x008373BA;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	//------------------B1h----------------//
	data_array[0] = 0x00103902;
	data_array[1] = 0x0E0E6CB1;
	data_array[2] = 0xEF0F0425;
	data_array[3] = 0x23DA6801;
	data_array[4] = 0x588AC086;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(5);
	//------------------B2h----------------//
	data_array[0] = 0x00103902;
	data_array[1] = 0x0F4485B2;
	data_array[2] = 0x081C2409;
	data_array[3] = 0x004D1C08;
	data_array[4] = 0x48443000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	//------------------B4h----------------//
	data_array[0] = 0x000D3902;
	data_array[1] = 0x59FF00B4;
	data_array[2] = 0x595A595A;
	data_array[3] = 0x0176015A;
	data_array[4] = 0x00000076;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
			//------------------BFh----------------//
	data_array[0] = 0x00043902; 										 
	data_array[1] = 0x010E41BF; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
			//------------------D2h----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000000D2; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
			//------------------D3h----------------//
	data_array[0] = 0x001F3902; 				 
	data_array[1] = 0x000000D3;   
	data_array[2] = 0x0C0C0701;
	data_array[3] = 0x00071032;
	data_array[4] = 0x0A200005;
	data_array[5] = 0x32000905;
	data_array[6] = 0x21000810;
	data_array[7] = 0x23070821;
	data_array[8] = 0x0047070D;
	dsi_set_cmdq(data_array,9, 1);
	MDELAY(1);
			//------------------D5h----------------//
	data_array[0] = 0x002D3902; 				 
	data_array[1] = 0x010001D5;   
	data_array[2] = 0x03020300;
	data_array[3] = 0x21202102;
	data_array[4] = 0x18021820;
	data_array[5] = 0x18181802;
	data_array[6] = 0x18181818;
	data_array[7] = 0x18181818;
	data_array[8] = 0x18181818;
	data_array[9] = 0x18181818;
	data_array[10] = 0x18181818;
	data_array[11] = 0x18181818;
	data_array[12] = 0x00000018;
	dsi_set_cmdq(data_array,13, 1);
	MDELAY(1);
			 //------------------BDh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000002BD; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
			 //------------------D8h----------------//
	data_array[0] = 0x000D3902; 				 
	data_array[1] = 0xAAFFFFD8;   
	data_array[2] = 0xFFA0AAAA;
	data_array[3] = 0xAAAAAAFF;
	data_array[4] = 0x000000A0;
	dsi_set_cmdq(data_array,5, 1);
	MDELAY(1);
			 //------------------E0h----------------//
	data_array[0]=0x002B3902;
data_array[1]=0x0A0500E0;
data_array[2]=0x183D2E28;
data_array[3]=0x0C0A0736;
data_array[4]=0x14110E17;
data_array[5]=0x12071312;
data_array[6]=0x05001813;
data_array[7]=0x3D2E280A;
data_array[8]=0x0A073618;
data_array[9]=0x110E170C;
data_array[10]=0x07131214;
data_array[11]=0x00181312;
	dsi_set_cmdq(data_array,12, 1);
	MDELAY(1);
		   //------------------BDh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000000BD; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
			//------------------C1h----------------//
	data_array[0]=0x002C3902;
data_array[1]=0x080001C1;
data_array[2]=0x28211910;
data_array[3]=0x48403830;
data_array[4]=0x675F574F;
data_array[5]=0x87807870;
data_array[6]=0xA79F978F;
data_array[7]=0xC8C0B8B0;
data_array[8]=0xE8E0D8D0;
data_array[9]=0x2CFFF8F0;
data_array[10]=0x2AF2C67B;
data_array[11]=0xC0593AA0;
dsi_set_cmdq(data_array,12, 1);
	MDELAY(1);
		   //------------------BDh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000001BD; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
		  //------------------C1h----------------//
	data_array[0]=0x002B3902;
data_array[1]=0x0F0700C1;
data_array[2]=0x2C251E16;
data_array[3]=0x48413A32;
data_array[4]=0x655D564F;
data_array[5]=0x827B746C;
data_array[6]=0x9E979089;
data_array[7]=0xBCB5ADA6;
data_array[8]=0xD9D2CCC4;
data_array[9]=0x0722E9E1;
data_array[10]=0x9310B2F0;
data_array[11]=0x00003DD8;

	dsi_set_cmdq(data_array,12, 1);
	MDELAY(1);
		  //------------------BDh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000002BD; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
		  //------------------C1h----------------//
	data_array[0]=0x002B3902;
data_array[1]=0x0D0700C1;
data_array[2]=0x2A231B14;
data_array[3]=0x47413931;
data_array[4]=0x645D554E;
data_array[5]=0x817A736C;
data_array[6]=0x9E978F88;
data_array[7]=0xBCB4ADA5;
data_array[8]=0xD8D1CBC3;
data_array[9]=0xEF0FE6DF;
data_array[10]=0xC67AC4C4;
data_array[11]=0x00407E71;
	dsi_set_cmdq(data_array,12, 1);
	MDELAY(1);
			//------------------CCh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000009CC; 					   
	dsi_set_cmdq(data_array, 2, 1); 	  
	MDELAY(1);
		  //------------------C7h----------------//
	data_array[0] = 0x00053902; 										 
	data_array[1] = 0x40C000C7;
	data_array[2] = 0x000000C0; 							   
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
			//------------------C0h----------------//
	data_array[0] = 0x00033902; 										 
	data_array[1] = 0x001430C0; 					   
	dsi_set_cmdq(data_array, 2, 1); 	  
	MDELAY(1);
		  //------------------BCh----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000007BC; 					   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(10);

				//------------------35h----------------//
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000035; 					   
	dsi_set_cmdq(data_array, 2, 1); 	  
	MDELAY(5);
			//------------------11h----------------//				
	data_array[0] = 0x00110500; 						 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1000);	   
		  //------------------C6h----------------//
	data_array[0] = 0x00033902; 										 
	data_array[1] = 0x0000BDC6; 					   
	dsi_set_cmdq(data_array, 2, 1); 	  
	MDELAY(5);
			//------------------29h----------------//	
	data_array[0] = 0x00290500; 						 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);
	}
/*static void push_table(struct LCM_setting_table *table, unsigned int count,
		       unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {
		unsigned cmd;

		cmd = table[i].cmd;
		switch (cmd) {

		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count,
					table[i].para_list, force_update);
		}
	}
}
 * LCM Driver Implementations
 */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode = CMD_MODE;
		params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
	params->dsi.mode   = BURST_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
#endif

		/* DSI */
		/* Command mode setting */
		params->dsi.LANE_NUM = LCM_FOUR_LANE;
		/* The following defined the fomat for data coming from LCD engine. */
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
		params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

		/* Highly depends on LCD driver capability. */
		/* Not support in MT6573 */
		params->dsi.packet_size = 256;
		params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.vertical_sync_active = 2;
		params->dsi.vertical_backporch = 8;
		params->dsi.vertical_frontporch = 6;
		params->dsi.vertical_active_line = FRAME_HEIGHT;

		params->dsi.horizontal_sync_active = 32;
		params->dsi.horizontal_backporch = 93;
		params->dsi.horizontal_frontporch = 93;
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;

		params->dsi.ssc_disable = 1;
		params->dsi.PLL_CLOCK = 168;
		params->dsi.HS_TRAIL = 4;

		params->dsi.cont_clock= 0;
		params->dsi.clk_lp_per_line_enable = 1;

        //zhangjianming add 
		params->dsi.esd_check_enable = 1;
		params->dsi.customization_esd_check_enable = 1;
		//params->dsi.lcm_esd_check_table[0].cmd		= 0xd9;
	//	params->dsi.lcm_esd_check_table[0].count	= 1;
		//params->dsi.lcm_esd_check_table[0].para_list[0] = 0x80;//80

		params->dsi.lcm_esd_check_table[0].cmd			= 0x09;
		params->dsi.lcm_esd_check_table[0].count		= 4;
		params->dsi.lcm_esd_check_table[0].para_list[0] = 0x80;//80
		params->dsi.lcm_esd_check_table[0].para_list[1] = 0x73;//73
		params->dsi.lcm_esd_check_table[0].para_list[2] = 0x06;//04
		params->dsi.lcm_esd_check_table[0].para_list[3] = 0x00;//00 
 
		//del by cm-dingpeifei@wingtech.com 20160223
		/*
		params->dsi.lcm_esd_check_table[1].cmd			= 0x45;
		params->dsi.lcm_esd_check_table[1].count		= 2;
		params->dsi.lcm_esd_check_table[1].para_list[0] = 0x0f;//80
		params->dsi.lcm_esd_check_table[1].para_list[1] = 0xf0;//73
		*/
		params->physical_width = 90;
		params->physical_height = 152;


		

		
}
static void lcm_init_power(void)
{
#ifdef BUILD_LK
	pmic_set_register_value(PMIC_RG_VGP1_VOSEL,0x7);
	pmic_set_register_value(PMIC_RG_VGP1_EN,1);
#else
	lcm_vgp_supply_enable();
       MDELAY(10);

#endif
}

static void lcm_suspend_power(void)
{
#ifdef BUILD_LK
	pmic_set_register_value(PMIC_RG_VGP1_EN,0);
#else
		lcm_vgp_supply_disable();
#endif
}

static void lcm_resume_power(void)
{
#ifdef BUILD_LK
	pmic_set_register_value(PMIC_RG_VGP1_VOSEL,0x7);
	pmic_set_register_value(PMIC_RG_VGP1_EN,1);
#else
		lcm_vgp_supply_enable();
#endif
}
static void lcm_init_lcm(void)
{
	//lcm_vgp_supply_enable();
    lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	SET_RESET_PIN(1);
	MDELAY(60);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	SET_RESET_PIN(0);
	MDELAY(60);
    lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	SET_RESET_PIN(1);
	MDELAY(240);
	//push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	init_lcm_registers();
}
static void lcm_init_lcm_delay_1000ms(void)
{
	       #ifdef BUILD_LK
			   printf("zz lcm_init_lcm_delay_1000ms  \n");
		#else
			   printk("zz lcm_init_lcm_delay_1000ms \n");
		#endif
	//lcm_vgp_supply_enable();
    lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	SET_RESET_PIN(1);
	MDELAY(60);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	SET_RESET_PIN(0);
	MDELAY(60);
    lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	SET_RESET_PIN(1);
	MDELAY(240);
	//push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	init_lcm_registers_delay_1000ms();
}

static void lcm_suspend(void)
{
	/*push_table(lcm_deep_sleep_mode_in_setting,
		   sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);*/
//	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
//	lcm_vgp_supply_disable();
//	SET_RESET_PIN(0);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	MDELAY(150);
}

static void lcm_resume(void)
{
	lcm_init_lcm();
}


LCM_DRIVER hx8394d_wsvga_dsi_vdo_guanzhen_lcm_drv = {
	.name		= "hx8394d_wsvga_dsi_guanzhen_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init_lcm,
	.init_delay1s   =lcm_init_lcm_delay_1000ms,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.init_power		= lcm_init_power,
	.resume_power = lcm_resume_power,
	.suspend_power = lcm_suspend_power,

};
