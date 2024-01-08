# 1. Thermostat

温控器

# 2. 清单

```
MCU：STC8G1K08 (1T)
FOSC：内部时钟 12.000M Hz 
EEPROM：设置 1K

OS：EasyOS  固定时间片

显示：3位数码管    驱动IC: TM1620
温度控制：继电器  GYB-1A-12D
```

# 3. 目录结构

代码在 ./src 目录下
```
+---APP - 程序入口
|       app.c
|       app.h
|       config.h
|       STARTUP.A51
|
+---BSP - 板载支持包。bsp_xxx 为BSP各个子模块
|       bsp.c - bsp入口文件
|       bsp.h - bsp入口头文件
|       bsp_buzzer.c
|       bsp_buzzer.h
|       bsp_eeprom.c
|       bsp_eeprom.h
|       bsp_heater.c
|       bsp_heater.h
|       bsp_kbd.c
|       bsp_kbd.h
|       bsp_led.c
|       bsp_led.h
|       bsp_ntc.c
|       bsp_ntc.h
|       bsp_oled.c
|       bsp_oled.h
|       bsp_uart.c
|       bsp_uart.h
|
+---EXT-LIB - 扩展的公共包
|       lib_delay.c - 延迟
|       lib_delay.h
|       lib_font_table.h - 自定义字符字典表
|       lib_stdint.h - C99引入的C标准库，C51没有，这里单独引入
|       lib_str.c - 自定义字符串操作
|       lib_str.h
|       lib_mem.c - 内存堆栈
|       lib_mem.h
|
\---RTX51 - RTX51 tiny 系统包
        Conf_tny.A51
        RTX51TNY.LIB
```

---
---

# 4. 设计说明

## 4.1 RTX51
