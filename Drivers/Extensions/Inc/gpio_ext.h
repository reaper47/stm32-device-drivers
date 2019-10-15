#ifndef __GPIO_EXT_H
#define __GPIO_EXT_H

#include "stm32f3xx_hal.h"

void GPIO_SetOutput(GPIO_TypeDef *port, uint16_t pin);
void GPIO_SetInput(GPIO_TypeDef *port, uint16_t pin);

#endif /* __GPIO_EXT_H */
