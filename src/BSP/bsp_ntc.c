/*
*********************************************************************************************************
*                                        BOARD SUPPORT PACKAGE
*
* Name    : NTC温度传感器
* Version : V1.0
* Author  : 空格键
* ----------------------------
* Note(s) : ADC: 10位
*           NTC：R25=100K/B=3950
*             电路设计：VCC----NTC传感器----10K1%电阻----GND。
*             开尔文公式：B=(lnR25 - lnRntc)/(1/T25 - 1/Tn)    Tn为要测量的温度（单位开尔文）
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp_ntc.h"



/*
*********************************************************************************************************
*                                               DEFINE
*********************************************************************************************************
*/

#define NTC = P16;  //ADC 通道6

#define TMP_ADC_LEN 10
u16 xdata tmp_adc[TMP_ADC_LEN]; //存储临时电压

#define ADC_BIT     1024    //ADC 10位输出
/**定义电压转换常数**/
#define R0          10      //串联电阻10K 1%
/**定义NTC常数**/
#define NTC_R25     100     //NTC 25度时标准阻值 单位 K
#define NTC_B	    3950    //NTC B值
/**定义开尔文常数**/
#define K_T0        273.15  //0℃下的开尔文温度
#define K_T25       298.15  //25℃下的开尔文温度


/*
*********************************************************************************************************
*                                         PRIVATE FUNCTION
*********************************************************************************************************
*/

//读取ADC结果。channel=[0,15]
static u16 _ADC_Read(u8 channel)
{
    u16 res;

#if (DEBUG_USE_SIMULATOR)
    res = (u16)(channel + 1);
#else
    channel &= 0x0F;        //入参过滤
    ADC_CONTR &= ~0x0F;     //设置通道，先清理，再设置
    ADC_CONTR |= channel;

    ADC_CONTR |= 0x40;              //启动AD转换
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & 0x20));    //查询ADC完成标志
    ADC_CONTR &= ~0x20;             //清完成标志
    res = (ADC_RES << 8) | ADC_RESL;//读取ADC结果
#endif

    return res;
}

//获取ADC测量值，滤波后的值
static u16 _ADC_GetVal_By_Filter(u8 channel) compact
{
    u8 i;
    for (i = 0; i < TMP_ADC_LEN; i++)
    {
        tmp_adc[i] = _ADC_Read(channel);  //读ADC数值
        delay_us(10); //加个延迟，消除连续读取干扰
        // BSP_UART_Println(UART1, "tmp_adc %bu=%u", i, tmp_adc[i]);
    }
    return math_filter_median_average(tmp_adc, TMP_ADC_LEN);//滤波计算ADC值
}

//转换成温度值
static float _Convert_Rntc_2_Temp(float Rntc) compact
{
    float t;
    //NTC开尔文温度t
    t = log(Rntc / NTC_R25) / NTC_B;
    t = 1 / K_T25 + t;
    t = 1 / t;
    //转成华氏度
    return t - K_T0;
}

/*
*********************************************************************************************************
* Description : NTC初始化
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_NTC_Init(void) large
{
    u8 i;

    P1M1 |= 0x40;   //NTC ADC高阻输入
    P1M0 &= ~0x40;

    //ADC初始化
    ADCTIM = 0x3F;      //设置ADC内部时序
    ADCCFG = 0x2F;      //设置ADC结果右对齐，时钟为SYSclk/2/16
    ADC_CONTR = 0x80;   //打开ADC电源
    delay_ms(1);        //延时1ms，等待ADC供电稳定

    for (i = 0; i < 5; i++)
        _ADC_GetVal_By_Filter(6); //初始化读取几次ADC值，让ADC稳定
}

/*
*********************************************************************************************************
* Description : 获取温度
*
* Argument(s) : none.
*
* Return(s)   : 温度测量值（°C）。int 型，小数点保留2位（返回值÷100才是真实温度）。
*
* Note(s)     : 返回INT16_MIN表示NTC未接入
*********************************************************************************************************
*/
s16 BSP_NTC_GetTemp() large
{
    float Rntc = 0, t;
    u16 adc_val;

    adc_val = _ADC_GetVal_By_Filter(6);     // 获取第6通道ADC测量值
    BSP_UART_Println(UART1, "[BSP_NTC_GetTemp] adc_val=%u", adc_val);
    if (adc_val <= 1 || adc_val == ADC_BIT) // 认为NTC未接入
        return INT16_MIN;

    Rntc = (ADC_BIT - adc_val) * R0 * 1.0 / adc_val; // 计算NTC阻值
    t = _Convert_Rntc_2_Temp(Rntc);                  // 计算温度
    return (s16)((t + 0.005) * 100);                 // 四舍五入，成100取整
}

/**********************************************RND******************************************************/