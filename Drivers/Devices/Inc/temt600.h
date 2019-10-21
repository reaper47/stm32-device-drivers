#ifndef __TEMT600_H
#define __TEMT600_H

#include <string.h>
#include "stm32f3xx_hal.h"

#define TEMT600_ADC_VOLTAGE      5.0
#define TEMT600_RESISTANCE_OHMS  10000.0
#define TEMT600_RESOLUTION       4096.0   // 12-bit
#define TEMT600_CURRENT(adc_val) ((adc_val/TEMT600_RESOLUTION)*TEMT600_ADC_VOLTAGE)
#define CURRENT_uA_ONE_LUX       2000000.0
#define TEMT600_JSON_LENGTH      12

// Structures
typedef struct {
	float percentage_brightness;
	uint16_t lux;
	uint16_t raw_value;
} TEMT600_HandleTypeDef;

// Public Functions
void TEMT600_Init(TEMT600_HandleTypeDef *htemt);
void TEMT600_CalculateLux(TEMT600_HandleTypeDef *htemt);
void TEMT600_ToJson_Partial(char *buffer, TEMT600_HandleTypeDef *htemt);

#endif /* __TEMT600_H */
