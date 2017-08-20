ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9271/
ifneq (,$(findstring config,$(CONFIG_TOUCHSCREEN_MTK_GT9271)))
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9271/include/$(CONFIG_TOUCHSCREEN_MTK_GT9271)/
else
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9271/include/config_default/
endif
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/

obj-y	+=  gt9xx_driver.o
obj-y	+=  goodix_tool.o
obj-y	+=  gt9xx_update.o
obj-y	+=  gt9xx_openshort.o

