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
|       cfg.h
|       STARTUP.A51
|
+---BSP - 板载支持包。bsp_xxx 为BSP各个子模块
|       bsp.c - bsp入口文件
|       bsp.h - bsp入口头文件
|       bsp_buzzer.c
|       bsp_buzzer.h
|       bsp_eeprom.c
|       bsp_eeprom.h
|       bsp_kbd.c
|       bsp_kbd.h
|       bsp_ntc.c
|       bsp_ntc.h
|       bsp_relay.c
|       bsp_relay.h
|       bsp_seg.c
|       bsp_seg.h
|       bsp_uart.c
|       bsp_uart.h
|
+---EXT-LIB - 扩展的公共包
|       lib_delay.c - 延迟
|       lib_delay.h
|       lib_math.c - 延迟
|       lib_math.h
|       lib_stdint.h - C99引入的C标准库，C51没有，这里单独引入
|       lib_str.c - 自定义字符串操作
|       lib_str.h
|       lib_mem.c - 内存堆栈
|       lib_mem.h
|
\---OS - 系统包
        os_timer.c
        os_timer.h
        os.c
        os.h
```

---
---

# 4. 设计说明
RTX51
## 4.1 温度精度

传感器测量精度控制在0.01，显示温度精度控制在0.1




# 5. 菜单&功能

## 菜单

```
运行模式
1. 指示灯：继电器吸合工作，指示灯亮起或闪烁。
2. 数码管：
    2.1 测量显示
        + 显示LL：表示传感器开路，接好传感器，正常显示当前温度；
        + 显示HH：表示超出测量范围，同时断开继电器；
        + 显示---：高温报警
    2.2 目标设定：默认28，设定范围 [P3, P2] 之间。

设置模式
1. P0 ———— 模式。默认值C。
    ┣ C ———— 制冷。当前 测量值>=设定值，继电器吸合
    ┗ H ———— 加热。温度 测量值<=设定值，继电器吸合
2. P1 ———— 回差设定。默认值0.1
    ┗ [0.1, 15] ———— 制冷模式下：测量值<=设定值-回差值，继电器断开；制冷模式下：测量值>=设定值+回差值，继电器断开
3. P2 ———— 温度上限。默认值250。
    ┗ [P3, 300] ———— 单位°C。目标温度<=P2
4. P3 ———— 温度下限。默认值-20。
    ┗ [-40, P2] ———— 单位°C。目标温度>=P2
5. P4 ———— 延时启动。默认值0
    ┗ [0, 30] ———— 单位：分钟
6. P5 ———— 温度校准。默认值0.0
    ┗ [-9.9, 9.9] ———— 单位°C。
7. P6 ———— 高温报警。默认值OFF
    ┗ [0, 110] ———— 单位°C。测量值>=P6，蜂鸣器报警
8. P7 ———— 预留
8. P8 ———— 预留
10. P9 ———— 回复出厂设置
    ┗ --- ———— 确认操作后，闪烁三下并伴随“嘀”声。
```

## 功能

```
1. 长按「设置」，在"运行模式"和"设置模式"来回切换。
2. 进入工作模式：
    2.1 短按「设置」，显示 温度传感器测量的当前温度 和 设置目标温度 来回切换；
    2.2 停留在 设置目标温度 5s没有操作，自动返回显示 温度传感器测量的当前温度；
    2.3 停留在 设置目标温度，可以按「加」「减」调节，调节范围 [P3, P2] 之间。
3. 进入设置模式：
    3.1 短按「设置」，Px <-> 参数值 来回切换。比如显示P0，按一下设置，进入H/C选择，再按一下设置，退回，显示P0；
    3.2 Px 页面，可以按「加」「减」进行P0-P5之间循环切换；
    3.3 Px子页面，可以按「加」「减」进行数值调节；
    3.4 P6子页面，长按「加」和「减」，超过6s，回复出厂设置。
```
