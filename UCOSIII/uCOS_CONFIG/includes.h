#ifndef INCLUDES_H
#define INCLUDES_H

/*
 * uC/OS-III 工程的总包含文件。
 * 所有 .c 文件通过包含本文件来获得 OS、CPU、LIB 以及板级支持包的声明。
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <os.h>

#include <cpu.h>
#include <lib_def.h>
#include <lib_ascii.h>
#include <lib_math.h>
#include <lib_mem.h>
#include <lib_str.h>

#include "app_cfg.h"
#include "bsp.h"

#endif
