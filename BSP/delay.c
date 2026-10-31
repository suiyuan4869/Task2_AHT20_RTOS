#include "delay.h"
#include "dwt.h"
#include "includes.h"

/*
 * SysTick 配置：
 *   HCLK = 72 MHz，SysTick 时钟 = HCLK/8 = 9 MHz
 *   uC/OS-III 节拍 = 200 Hz，因此重装值为 9 MHz / 200 = 45000
 *
 * 注意：
 *   1. SysTick 只给 uC/OS-III 使用，不要再用 SysTick 做 HAL_Delay 之类的延时。
 *   2. SysTick 优先级必须设为最低（15），避免打断内核临界区。
 *   3. DWT 用于 delay_us() 和软件 I2C 的位延时。
 */
void delay_init(void)
{
    uint32_t reload;

    /* SysTick 时钟源选择 HCLK/8 */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    reload = SystemCoreClock / 8000000u;          /* 9 MHz 下每微秒的计数个数 */
    reload *= (1000000u / OSCfg_TickRate_Hz);     /* 每个 OS 节拍的计数个数 */

    SysTick->LOAD = reload - 1u;
    SysTick->VAL  = 0u;

    /* Cortex-M3 只有 4 位优先级，15 为最低优先级 */
    NVIC_SetPriority(SysTick_IRQn, 15u);

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;    /* 开节拍中断 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;     /* 启动 SysTick */

    /* 使能 DWT 周期计数器，供 delay_us() 使用 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0u;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/*
 * 微秒级忙等待。
 * 72 MHz 下 1 us = 72 个内核周期，DWT 每个周期加 1。
 * 该函数不关闭中断，但会占用 CPU；只用于 I2C 的短延时。
 */
void delay_us(uint32_t nus)
{
    uint32_t start;
    uint32_t ticks;

    start = DWT->CYCCNT;
    ticks = nus * (SystemCoreClock / 1000000u);

    while ((DWT->CYCCNT - start) < ticks)
    {
        __NOP();
    }
}

/*
 * 毫秒级延时。
 * OS 已经运行时，调用 OSTimeDlyHMSM() 让当前任务进入阻塞态，
 * 这样低优先级任务仍可运行；OS 未运行时才使用忙等待。
 */
void delay_ms(uint16_t nms)
{
    OS_ERR err;

    if ((OSRunning == 1u) && (OSIntNestingCtr == 0u))
    {
        OSTimeDlyHMSM(0u, 0u, 0u, nms, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    else
    {
        delay_us((uint32_t)nms * 1000u);
    }
}

/*
 * SysTick 中断服务函数。
 * uC/OS-III 的系统节拍在这里递增；OSStart() 之前 OSRunning 为 0，
 * 此时即使 SysTick 已经启动也不会调用 OS API。
 */
void SysTick_Handler(void)
{
    if (OSRunning == 1u)
    {
        OSIntEnter();
        OSTimeTick();
        OSIntExit();
    }
}
