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
	
	unsigned int data_array[16];
	//------------------Page3----------------//
	data_array[0] = 0x00043902;
	data_array[1] = 0x038198FF;
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000001; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000002; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00007303; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000D304; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000005; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000A06; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000F07; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000008; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000109; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000010A; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000010B; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000010C; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000010D; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000010E; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000260F; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002610; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000011; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000012; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000013; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000014; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000015; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000016; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000017; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000018; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000019; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000001A; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000001B; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000001C; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000001D; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000401E; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000801F; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000620; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000121; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000022; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000023; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000024; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000025; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000026; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000027; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00003328; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000329; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000002A; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000002B; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000002C; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000002D; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000002E; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000002F; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000030; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000031; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000032; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000033; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000334; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000035; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000336; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000037; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000038; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000039; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000003A; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000003B; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000003C; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000003D; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000003E; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000003F; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000040; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000041; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000042; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000043; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000044; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000150; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002351; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00004552; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00006753; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00008954; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000AB55; 					   
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000156; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002357; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00004558; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00006759; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000895A; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000AB5B; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000CD5C; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000EF5D; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000115E; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000F5F; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000E60; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000061; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000162; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000663; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000764; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000265; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000266; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000267; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000268; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000269; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000026A; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000026B; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000026C; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000026D; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000026E; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000026F; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000270; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000271; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000872; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000D73; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000C74; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000F75; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000E76; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000077; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000178; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000679; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000077A; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000027B; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000027C; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000027D; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000027E; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000027F; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000280; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000281; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000282; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000283; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000284; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000285; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000286; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000287; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000888; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000D89; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000C8A; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	//------------------Page4----------------//	
	data_array[0] = 0x00043902; 										 
	data_array[1] = 0x048198FF; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000156C; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002A6E; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000376F; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000158D; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000A43A; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000BA87; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00007626; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x0000D1B2; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	//------------------Page1----------------//	
	data_array[0] = 0x00043902; 										 
	data_array[1] = 0x018198FF; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000A22; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000031; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00009050; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00009051; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00006053; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00007455; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001460; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000000A0; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001BA1; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002BA2; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000012A3; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000017A4; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002BA5; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001FA6; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001FA7; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000095A8; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001CA9; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000029AA; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000082AB; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000021AC; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000022AD; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000056AE; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002BAF; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002EB0; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000044B1; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000050B2; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000019B3; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000000C0; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001BC1; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002BC2; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000012C3; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000017C4; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002BC5; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001FC6; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001FC7; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000095C8; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00001CC9; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000029CA; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000082CB; 					   
	dsi_set_cmdq(data_array, 2, 1);		
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000021CC; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000022CD; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000056CE; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002BCF; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002ED0; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000044D1; 					   
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000050D2; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x000019D3; 					   
	dsi_set_cmdq(data_array, 2, 1);		

	//------------------Page0----------------//	
	data_array[0] = 0x00043902; 										 
	data_array[1] = 0x008198FF; 					   
	dsi_set_cmdq(data_array, 2, 1);
				
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000035; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	/* CABC Control 
	data_array[0] = 0x00033902; 										 
	data_array[1] = 0x00FF0F51; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00002C53; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	
	data_array[0] = 0x00023902; 										 
	data_array[1] = 0x00000055; 					   
	dsi_set_cmdq(data_array, 2, 1);	
	*/
			//------------------11h----------------//				
	data_array[0] = 0x00110500; 						 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);	   

			//------------------29h----------------//	
	data_array[0] = 0x00290500; 						 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);
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

//add compare_id for diff IC  2016.3.11
static unsigned int lcm_compare_id(void)
{
	//unsigned char cmd = 0x0;
	//unsigned char data = 0x0a;
	unsigned int id = 0,id1 = 0,id2 = 0;
	//int ret=0;
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

	//init_lcm_registers();

	//push_table(lcm_initialization_setting,
	//	   sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

	printk("%s %d\n", __func__,__LINE__);
	data_array[0]=0x00043902;
	data_array[1]=0x018198ff;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(10);

	data_array[0]=0x00013700; 
	dsi_set_cmdq(data_array, 1,1);
	MDELAY(10);

	read_reg_v2(0x00, buffer, 1);
	id1= buffer[0]; //should be 0x01
	read_reg_v2(0x01, buffer, 1);
	id2= buffer[0]; //should be 0x01

	id=(id1 << 8) | id2;

#if defined(BUILD_LK)||defined(BUILD_UBOOT)
	printk(" hzs %s id=%x  \n",__func__,id);
#else
	printk("dzl------------- %s id=%x  \n",__func__,id);
#endif	

	if(0x9881==id)
		return 1;
	else
		return 0;
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

LCM_DRIVER ili9881c_dsi_vdo_lcm_drv = {
	.name		= "ili9881c_dsi_vdo",
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
