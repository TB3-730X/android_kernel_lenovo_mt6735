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
extern int lcm_vgp_supply_enable(void);
extern int lcm_vgp_supply_disable(void);
extern void lcm_set_gpio_output(unsigned int GPIO, unsigned int output);
extern  unsigned int GPIO_LCD_RST_EN;





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

	data_array[0] = 0x00023902;
	data_array[1] = 0x00000083;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
		data_array[0] = 0x00023902;
	data_array[1] = 0x00000084;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
		data_array[0] = 0x00023902;
	data_array[1] = 0x00000485;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
		data_array[0] = 0x00023902;
	data_array[1] = 0x00000886;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00008e8c;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002bc5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002bc7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000aa83;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001184;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004ba9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000083;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000084;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005bfd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000014fa;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000aa83;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001184;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001fc0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000020c1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000028c2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000036c3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000046c4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004ec5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000054c6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000058c7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005ec8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d1c9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000dbca;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000f8cb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000005cc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000acd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


    data_array[0] = 0x00023902;
	data_array[1] = 0x000008ce;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000acf;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000dd0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001ed1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002fd2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004bd3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000051d4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000b1d5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000b5d6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000bdd7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000c5d8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000ced9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d8da;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000e3db;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


   data_array[0] = 0x00023902;
	data_array[1] = 0x0000fedc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000ffdd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000f8de;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002fdf;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000020e0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000021e1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002ae2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000039e3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000048e4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004fe5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000057e6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005be7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000061e8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d8e9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);

   data_array[0] = 0x00023902;
	data_array[1] = 0x0000e4ea;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002eb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000010ec;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000015ed;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000014ee;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000016ef;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001df0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002ff1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000040f2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005ff3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00006af4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000bdf5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000bef6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000c9f7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d1f8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000daf9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000e4fa;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000eefb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fdfc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fffd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fcfe;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002fff;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	//
		
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000bb83;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002284;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000019c0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001ec1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000026c2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000034c3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000044c4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004cc5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000052c6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000056c7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005cc8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000cfc9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d6ca;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000f2cb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fdcc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000000cd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


    data_array[0] = 0x00023902;
	data_array[1] = 0x0000fbce;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fdcf;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fbd0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000ffd1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000006d2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000011d3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000cd4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000088d5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00008ad6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00008ed7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000092d8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000098d9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00009dda;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000a4db;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


    data_array[0] = 0x00023902;
	data_array[1] = 0x0000abdc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000b9dd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000010de;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002edf;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001ee0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001fe1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000028e2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000037e3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000046e4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004de5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000055e6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000059e7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005fe8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d5e9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);

   data_array[0] = 0x00023902;
	data_array[1] = 0x0000deea;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fbeb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000007ec;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000aed;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000006ee;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000008ef;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000af0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000ff1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000016f2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000022f3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001df4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00008ef5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000092f6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000099f7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


	data_array[0] = 0x00023902;
	data_array[1] = 0x00009df8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000a3f9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000a8fa;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000adfb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000b5fc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000b7fd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x0000f8fe;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002fff;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x0000cc83;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003384;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001fc0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000020c1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000028c2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000036c3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000046c4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004ec5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000054c6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000058c7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005ec8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d1c9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000dbca;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000f8cb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000005cc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000acd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


    data_array[0] = 0x00023902;
	data_array[1] = 0x000008ce;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000acf;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000dd0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001ed1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002fd2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004bd3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000051d4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000b1d5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000b5d6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000bdd7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000c5d8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000ced9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d8da;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000e3db;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


    data_array[0] = 0x00023902;
	data_array[1] = 0x0000f1dc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000ffdd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000f8de;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002fdf;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000020e0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000021e1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002ae2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000039e3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000048e4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004fe5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000057e6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005be7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000061e8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d8e9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);

   data_array[0] = 0x00023902;
	data_array[1] = 0x0000e4ea;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002eb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000010ec;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000015ed;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000014ee;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000016ef;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001df0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002ff1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000040f2;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005ff3;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00006af4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000bdf5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000bef6;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000c9f7;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


	data_array[0] = 0x00023902;
	data_array[1] = 0x0000d1f8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000daf9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000e4fa;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000eefb;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fdfc;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fffd;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x0000fcfe;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002fff;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);


	}

static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	{0x28, 0, {}},
	{REGFLAG_DELAY, 60, {}},
	/* Sleep Mode On */
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for(i = 0; i < count; i++)
	{
		unsigned cmd;

		cmd = table[i].cmd;
		switch (cmd) {

		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

			default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

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

	params->dsi.word_count=1200*3;
	params->dsi.mode   = BURST_VDO_MODE;

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;//LCM_DSI_FORMAT_RGB666;
	//  params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;

	// Video mode setting		
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;//LCM_PACKED_PS_18BIT_RGB666;

	params->dsi.vertical_sync_active				= 1;//50;
	params->dsi.vertical_backporch					= 25;//30;
	params->dsi.vertical_frontporch 				= 35;//36;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 1;//64;
	params->dsi.horizontal_backporch				= 60;//56;
	params->dsi.horizontal_frontporch				= 80;//60;
	params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 170;
	
	params->dsi.cont_clock = 1;
	params->dsi.ssc_disable = 1;
	
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

static void lcm_init(void)
{
//	lcm_vgp_supply_enable();
   // lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
//	SET_RESET_PIN(1);
//	MDELAY(60);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
//	SET_RESET_PIN(0);
	MDELAY(20);
   lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
//	SET_RESET_PIN(1);
	MDELAY(20);
//	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
init_lcm_registers();	//init_lcm_registers();
}

static void lcm_suspend(void)
{
	push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);  
	MDELAY(10);
//	lcm_vgp_supply_disable();
	//SET_RESET_PIN(0);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
       MDELAY(150);
}

static void lcm_resume(void)
{
	lcm_init();
}

static unsigned int lcm_compare_id(void)
{
return 0;

}



LCM_DRIVER nt51021b_wsvga_dsi_vdo_innolux_lcm_drv = {
	.name		= "nt51021b_wsvga_dsi_vdo_innolux",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
       .compare_id     = lcm_compare_id,
  	.init_power		= lcm_init_power,
	.resume_power = lcm_resume_power,
	.suspend_power = lcm_suspend_power,

};
