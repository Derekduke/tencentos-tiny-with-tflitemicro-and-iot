#ifndef __LORA_DEMO_H__
#define __LORA_DEMO_H__

#include "mcu_init.h"
#include "tos_at.h"
#include "string.h"
#include "tos_k.h"
#include "lora_module_wrapper.h"
#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
    char data[32];
}ReportData_TypeDef;

void application_entry(void *arg);
#ifdef __cplusplus
}
#endif

#endif /* __LORA_DEMO_H__ */

