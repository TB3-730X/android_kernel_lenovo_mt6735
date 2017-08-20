#include <linux/string.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <asm-generic/gpio.h>

#include "lcm_drv.h"
#include "ddp_irq.h"

static unsigned int GPIO_LCD_PWR_ENP;
static unsigned int GPIO_LCD_PWR_ENN;
static unsigned int GPIO_LCD_RST;

/**
 * Local Constants
 */
#define FRAME_WIDTH		(720)
#define FRAME_HEIGHT		(1280)

#define REGFLAG_DELAY		0xFE
#define REGFLAG_END_OF_TABLE	0xFF   /* END OF REGISTERS MARKER */

#define LCM_DSI_CMD_MODE	0

/**
 * Local Variables
 */
static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)	(lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


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

/**
 * Note :
 *
 * Data ID will depends on the following rule.
 *
 * count of parameters > 1	=> Data ID = 0x39
 * count of parameters = 1	=> Data ID = 0x15
 * count of parameters = 0	=> Data ID = 0x05
 *
 * Structure Format :
 *
 * {DCS command, count of parameters, {parameter list}}
 * {REGFLAG_DELAY, milliseconds of time, {} },
 * ...
 *
 * Setting ending by predefined flag
 *
 * {REGFLAG_END_OF_TABLE, 0x00, {}}
 */
#if 0
static struct LCM_setting_table lcm_initialization_setting[] = {
	/* sleep out */
	{0x11, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	{0x29, 0, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static struct LCM_setting_table lcm_set_window[] = {
	{0x2A, 4, {0x00, 0x00, (FRAME_WIDTH >> 8), (FRAME_WIDTH&0xFF) } },
	{0x2B, 4, {0x00, 0x00, (FRAME_HEIGHT >> 8), (FRAME_HEIGHT&0xFF)} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
	/* Sleep Out */
	{0x11, 0, {} },
	{REGFLAG_DELAY, 120, {} },

	/* Display ON */
	{0x29, 0, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	/* Sleep Mode On */
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table, unsigned int count,
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
#endif

static void init_lcm_registers(void)
	{
		 unsigned int data_array[20];
	
			//------------------B9h----------------//
			 data_array[0] = 0x00043902;						 
			 data_array[1] = 0x9483FFB9;				
				 dsi_set_cmdq(data_array, 2, 1); 
					 MDELAY(1);
	
		//------------------BAh----------------//
			 data_array[0] = 0x00073902;						 
			 data_array[1] = 0x680363BA;
			 data_array[2] = 0x00C0B26B;
				 dsi_set_cmdq(data_array, 3, 1);
					 MDELAY(20); 
	
		
		//------------------B1h----------------//
			 data_array[0] = 0x000B3902;			   
			 data_array[1] = 0x6E0E48B1;   
			 data_array[2] = 0x71243309; 
			 data_array[3] = 0x00433071;  
			 dsi_set_cmdq(data_array, 4, 1);
					 MDELAY(5);
	
	
			 //------------------D2h----------------//
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x000033D2; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(20);
	
		//------------------B2h----------------//
					 data_array[0] = 0x00073902;					   
			 data_array[1] = 0x648000B2; 
			 data_array[2] = 0x002F060C;				
			 dsi_set_cmdq(data_array, 3, 1);
					 MDELAY(1); 
	
		//------------------B4h----------------//
			 data_array[0] = 0x00163902;			   
			 data_array[1] = 0x767476B4;   
			 data_array[2] = 0x01747674; 
			 data_array[3] = 0x00357E05; 
			 data_array[4] = 0x7674763F; 
					 data_array[5] = 0x01747674;
					 data_array[6] = 0x00007E05;
			 dsi_set_cmdq(data_array, 7, 1);
					 MDELAY(5); 
	
		   //------------------D3h----------------//
			 data_array[0] = 0x00223902;		   
			 data_array[1] = 0x0F0000D3;   
			 data_array[2] = 0x1200000F; 
			 data_array[3] = 0x00103210; 
			 data_array[4] = 0x13320000; 
			 data_array[5] = 0x320000C0; 
			 data_array[6] = 0x00000810; 
			 data_array[7] = 0x02020447; 
			 data_array[8] = 0x47000447;
					 data_array[9] = 0x0000400C;
			 dsi_set_cmdq(data_array,10, 1);
					 MDELAY(10); 
	
		   //------------------D5h----------------//
			 data_array[0] = 0x002D3902;		   
			 data_array[1] = 0x020100D5;   
			 data_array[2] = 0x06050403; 
			 data_array[3] = 0x22212007; 
			 data_array[4] = 0x18181823; 
			 data_array[5] = 0x18181818; 
			 data_array[6] = 0x18181818; 
			 data_array[7] = 0x18181818; 
			 data_array[8] = 0x18181818; 
				 data_array[9] = 0x18181818; 
			 data_array[10] = 0x18181818; 
			 data_array[11] = 0x18191918; 
			 data_array[12] = 0x00000018; 
			 dsi_set_cmdq(data_array,13, 1);
					 MDELAY(10); 
	
			 //------------------D6h----------------//
			 data_array[0] = 0x002D3902;		   
			 data_array[1] = 0x050607D6;   
			 data_array[2] = 0x01020304; 
			 data_array[3] = 0x21222300; 
			 data_array[4] = 0x18181820; 
			 data_array[5] = 0x18181818; 
			 data_array[6] = 0x18181818; 
			 data_array[7] = 0x18181818; 
			 data_array[8] = 0x18181818; 
				 data_array[9] = 0x18181818; 
			 data_array[10] = 0x18181818; 
			 data_array[11] = 0x19181818; 
			 data_array[12] = 0x00000019; 
			 dsi_set_cmdq(data_array,13, 1);
					 MDELAY(10); 
	
	
			//------------------B6h----------------//
					data_array[0] = 0x00033902; 				   
			data_array[1] = 0x005656B6; 			
			dsi_set_cmdq(data_array, 2, 1); 
					MDELAY(1);
	
		   //------------------E0h----------------//
			 data_array[0] = 0x003B3902;		   
			 data_array[1] = 0x1A0F00E0;   
			 data_array[2] = 0x2B272421; 
			 data_array[3] = 0x72635429; 
			 data_array[4] = 0x8E88786F; 
			 data_array[5] = 0x999D9D91; 
			 data_array[6] = 0x595AB5A6; 
			 data_array[7] = 0x6C65615D; 
			 data_array[8] = 0x0F007F78; 
				 data_array[9] = 0x2724211A; 
			 data_array[10] = 0x6354292B; 
			 data_array[11] = 0x88786F72; 
					 data_array[12] = 0x9D9D918E; 
			 data_array[13] = 0x5AB5A699; 
			 data_array[14] = 0x65615D59;
					 data_array[15] = 0x007F786C; 
			 dsi_set_cmdq(data_array,16, 1);
					 MDELAY(20);
	
	
	
		  //------------------CCh----------------// 
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x00000BCC; 			
			dsi_set_cmdq(data_array, 2, 1); 
					MDELAY(1);
	
		 
		//------------------C0h----------------//
					data_array[0] = 0x00033902; 				   
			data_array[1] = 0x00311FC0; 			
			dsi_set_cmdq(data_array, 2, 1); 
					MDELAY(1);
		  //------------------D4h----------------// 
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x000002D4; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(1); 
	
		  //------------------BDh----------------//
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x000001BD; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(1);
	
		  //------------------B1h----------------//
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x000060B1; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(1);
	
		 //------------------BDh----------------//
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x000000BD; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(1);
		//------------------51h----------------//
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x00007F51; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(1);
	   //------------------53h----------------//
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x00002453; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(1);
	   //------------------55h----------------//
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x00000055; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(1);
		//------------------35h----------------//
					data_array[0] = 0x00023902; 				   
			data_array[1] = 0x00000035; 			
			dsi_set_cmdq(data_array, 2, 1);
					MDELAY(1);
	
		//------------------11h----------------//		
			data_array[0] = 0x00110500; 		   
			dsi_set_cmdq(data_array, 1, 1);
			MDELAY(200);	
	
	
			 
		//------------------29h----------------//	
					data_array[0] = 0x00290500; 		   
			dsi_set_cmdq(data_array, 1, 1);
			MDELAY(10);
	
	}


/**
 * LCM Driver Implementations
 */
static void lcm_get_gpio_infor(void)
{
	static struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "mediatek,mt6735-dispsys");

	GPIO_LCD_PWR_ENP = of_get_named_gpio(node, "lcm_power_enp_gpio", 0);
	GPIO_LCD_PWR_ENN = of_get_named_gpio(node, "lcm_power_enn_gpio", 0);
	GPIO_LCD_RST = of_get_named_gpio(node, "lcm_power_rst_gpio", 0);

	printk("csq %x, %x, %x\n", GPIO_LCD_PWR_ENP, GPIO_LCD_PWR_ENN, GPIO_LCD_RST);
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	gpio_direction_output(GPIO, output);
	gpio_set_value(GPIO, output);
}

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;
	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
	params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
	params->dsi.mode = SYNC_PULSE_VDO_MODE;
#endif

	/* DSI */
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_FOUR_LANE;
	/* The following defined the fomat for data coming from LCD engine. */
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	/* Highly depends on LCD driver capability. */
	/* Not support in MT6573 */
	params->dsi.packet_size = 256;
	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.vertical_sync_active = 10;
	params->dsi.vertical_backporch = 20;
	params->dsi.vertical_frontporch = 20;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 10;
	params->dsi.horizontal_backporch = 150;
	params->dsi.horizontal_frontporch = 100;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	params->dsi.ssc_disable = 1;
	params->dsi.PLL_CLOCK = 235;
}


static void lcm_init(void)
{
	lcm_get_gpio_infor();
	lcm_set_gpio_output(GPIO_LCD_PWR_ENP, 1);
	lcm_set_gpio_output(GPIO_LCD_PWR_ENN, 1);
	MDELAY(5);

	lcm_set_gpio_output(GPIO_LCD_RST, 1);
	SET_RESET_PIN(1);
	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, 0);
	SET_RESET_PIN(0);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, 1);
	SET_RESET_PIN(1);
	MDELAY(120);

	init_lcm_registers();

	//push_table(lcm_initialization_setting,
	//	   sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	lcm_get_gpio_infor();

	lcm_set_gpio_output(GPIO_LCD_PWR_ENP, 0);
	lcm_set_gpio_output(GPIO_LCD_PWR_ENN, 0);
	lcm_set_gpio_output(GPIO_LCD_RST, 0);

	//push_table(lcm_deep_sleep_mode_in_setting,
	//	   sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_resume(void)
{
	lcm_init();
}

#if 0
static void lcm_setpwm(unsigned int divider)
{
	/* TBD */
}


static unsigned int lcm_getpwm(unsigned int divider)
{
	/* ref freq = 15MHz, B0h setting 0x80, so 80.6% * freq is pwm_clk; */
	/* pwm_clk / 255 / 2(lcm_setpwm() 6th params) = pwm_duration = 23706 */
	unsigned int pwm_clk = 23706 / (1 << divider);

	return pwm_clk;
}
#endif
static unsigned int lcm_compare_id(void)
{
	unsigned int id = 0,id1 = 0,id2 = 0,id3 = 0;
	unsigned char buffer[5];
	unsigned int data_array[16];
	
	lcm_get_gpio_infor();
	lcm_set_gpio_output(GPIO_LCD_PWR_ENP, 1);
	lcm_set_gpio_output(GPIO_LCD_PWR_ENN, 1);
	MDELAY(5);

	lcm_set_gpio_output(GPIO_LCD_RST, 1);
	SET_RESET_PIN(1);
	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, 0);
	SET_RESET_PIN(0);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, 1);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	data_array[0]=0x00043700; 
	dsi_set_cmdq(data_array, 1,1);
	MDELAY(10);
	
	printk("%s %d\n", __func__,__LINE__);
	read_reg_v2(0x04, buffer, 4);
	id1= buffer[0]; //should be 0x01
	id2= buffer[1];
	id3= buffer[3];
	id = (id1 << 8) | id2;
	printk("%s,id1=%d,id2=%d,id3=%d,id=%d\n",__func__,id1,id2,id3,id);	
	if(0x8394==id)
		return 1;
	else
		return 0;
}

LCM_DRIVER hx8394f_dsi_vdo_lcm_drv = {
	.name		= "hx8394f_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id 	= lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	/*.set_backlight	= lcm_setbacklight,*/
	/* .set_pwm        = lcm_setpwm, */
	/* .get_pwm        = lcm_getpwm, */
	/*.update         = lcm_update, */
#endif
};
