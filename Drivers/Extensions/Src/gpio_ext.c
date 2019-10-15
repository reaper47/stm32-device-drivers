#include "gpio_ext.h"


void GPIO_SetOutput(GPIO_TypeDef *port, uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}


void GPIO_SetInput(GPIO_TypeDef *port, uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}
