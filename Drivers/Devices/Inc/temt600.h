#ifndef __TEMT600_H
#define __TEMT600_H

#include <string.h>
#include "stm32f3xx_hal.h"

#define TEMT600_ADC_VOLTAGE      5.0
#define TEMT600_RESISTANCE_OHMS  10000.0
#define TEMT600_RESOLUTION       4096.0   // 12-bit
#define TEMT600_CURRENT(adc_val) ((adc_val/TEMT600_RESOLUTION)*TEMT600_ADC_VOLTAGE)
#define CURRENT_uA_ONE_LUX       2000000.0
#define TEMT600_N_SAMPLES        8
#define TEMT600_JSON_LENGTH      30

// Structures
typedef struct {
	ADC_HandleTypeDef *hadc;
	uint8_t is_conversion_completed;
	uint16_t buffer[TEMT600_N_SAMPLES];
	float percentage_brightness;
	uint16_t lux;
} TEMT600_HandleTypeDef;

// Public Functions
void TEMT600_Init(TEMT600_HandleTypeDef *htemt, ADC_HandleTypeDef *hadc);
void TEMT600_Start(TEMT600_HandleTypeDef *htemt);
void TEMT600_Stop(TEMT600_HandleTypeDef *htemt);
void TEMT600_Sample(TEMT600_HandleTypeDef *htemt);

void TEMT600_Sample_DMA(TEMT600_HandleTypeDef *htemt);

void TEMT600_Init_DMATimer(TEMT600_HandleTypeDef *htemt, ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim);
void TEMT600_Sample_DMATimer(TEMT600_HandleTypeDef *htemt);

void TEMT600_ToJson_Partial(char *buffer, TEMT600_HandleTypeDef *htemt);

// Private Functions
void _calculate_lux(TEMT600_HandleTypeDef *htemt);
void _start_dma(TEMT600_HandleTypeDef *htemt);

#endif /* __TEMT600_H */
