#ifndef APP_H
#define APP_H

#include "includes.h"

extern OS_TCB  AppTaskStartTCB;
extern CPU_STK AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

void AppTaskStart(void *p_arg);
void AppTaskCreate(void);

#endif
