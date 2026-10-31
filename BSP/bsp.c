#include "bsp.h"
#include "led.h"
#include "uart.h"
#include "delay.h"

/*
 * DWT 寄存器定义。uC/OS-III 的时间戳和软件 I2C 的微秒延时都使用 DWT。
 * 这里直接给出地址，避免不同 CMSIS 版本宏名称差异。
 */
#define BSP_REG_DEM_CR          (*(volatile CPU_INT32U *)0xE000EDFC)
#define BSP_REG_DWT_CR          (*(volatile CPU_INT32U *)0xE0001000)
#define BSP_REG_DWT_CYCCNT      (*(volatile CPU_INT32U *)0xE0001004)

#define BSP_BIT_DEM_CR_TRCENA   ((CPU_INT32U)0x01000000)   /* DEMCR.TRCENA，bit24 */
#define BSP_BIT_DWT_CR_CYCCNTENA ((CPU_INT32U)0x00000001)  /* DWT_CTRL.CYCCNTENA，bit0 */

/*
 * 函数名：BSP_CPU_ClkFreq
 * 功能  ：返回 HCLK 频率（Hz）
 * 说明  ：uC/OS-III 的 CPU 时间戳需要知道 CPU 时钟。
 */
CPU_INT32U BSP_CPU_ClkFreq(void)
{
    RCC_ClocksTypeDef rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);
    return (CPU_INT32U)rcc_clocks.HCLK_Frequency;
}

/*
 * 函数名：BSP_Init
 * 功能  ：板级初始化
 * 说明  ：必须在 OSInit() 之前调用。SysTick 在 delay_init() 中配置为
 *         uC/OS-III 的系统节拍，因此不要再在别处重复配置 SysTick。
 */
void BSP_Init(void)
{
    /* 全部 4 位都作为抢占优先级，避免子优先级影响 OS 的中断嵌套判断 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    Uart1_Init();      /* PA9=TX, PA10=RX, 115200-8-N-1 */
    delay_init();      /* SysTick：200 Hz，优先级 15；同时使能 DWT */
    LED_Init();        /* PC13、PB0 */
}

/*
 * 以下三个函数是 uC/OS-III CPU 模块的时间戳移植接口。
 * 如果 cpu_cfg.h 中 CPU_CFG_TS_TMR_EN 打开，则必须由 BSP 提供。
 */
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void CPU_TS_TmrInit(void)
{
    CPU_INT32U fclk_freq;

    fclk_freq = BSP_CPU_ClkFreq();

    BSP_REG_DEM_CR |= BSP_BIT_DEM_CR_TRCENA;   /* 使能 DWT 访问 */
    BSP_REG_DWT_CYCCNT = 0u;                   /* 计数器清零 */
    BSP_REG_DWT_CR |= BSP_BIT_DWT_CR_CYCCNTENA;/* 启动周期计数器 */

    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}

CPU_TS_TMR CPU_TS_TmrRd(void)
{
    return (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;
}
#endif

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U CPU_TS32_to_uSec(CPU_TS32 ts_cnts)
{
    CPU_INT64U ts_us;
    CPU_INT64U fclk_freq;

    fclk_freq = BSP_CPU_ClkFreq();
    ts_us = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return ts_us;
}
#endif

#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U CPU_TS64_to_uSec(CPU_TS64 ts_cnts)
{
    CPU_INT64U ts_us;
    CPU_INT64U fclk_freq;

    fclk_freq = BSP_CPU_ClkFreq();
    ts_us = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return ts_us;
}
#endif
