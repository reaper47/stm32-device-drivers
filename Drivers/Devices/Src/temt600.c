#include "temt600.h"

// Usage: Normal
void TEMT600_Init(TEMT600_HandleTypeDef *htemt, ADC_HandleTypeDef *hadc)
{
	htemt->hadc = hadc;
	htemt->is_conversion_completed = 0;
	memset(htemt->buffer, 0, sizeof htemt->buffer);
	htemt->percentage_brightness = 0.0;
	htemt->lux = 0;
}


void TEMT600_Start(TEMT600_HandleTypeDef *htemt)
{
	HAL_ADCEx_Calibration_Start(htemt->hadc, ADC_SINGLE_ENDED);
	HAL_ADC_Start(htemt->hadc);
}


void TEMT600_Stop(TEMT600_HandleTypeDef *htemt)
{
	HAL_ADC_Stop(htemt->hadc);
}


void TEMT600_Sample(TEMT600_HandleTypeDef *htemt)
{
	uint16_t sensor_value = HAL_ADC_GetValue(htemt->hadc);
	uint32_t lux = (TEMT600_CURRENT(sensor_value)/TEMT600_RESISTANCE_OHMS)*CURRENT_uA_ONE_LUX;

	htemt->percentage_brightness = (sensor_value/TEMT600_RESOLUTION)*100;
	htemt->lux = lux;
}


// Usage: DMA
void TEMT600_Sample_DMA(TEMT600_HandleTypeDef *htemt)
{
	_start_dma(htemt);
	while (!htemt->is_conversion_completed);
	HAL_ADC_Stop_DMA(htemt->hadc);
	htemt->is_conversion_completed = 0;
	_calculate_lux(htemt);
}


void _start_dma(TEMT600_HandleTypeDef *htemt)
{
	HAL_ADC_Start_DMA(htemt->hadc, (uint32_t*)htemt->buffer, TEMT600_N_SAMPLES);
}


// Usage: DMA with a Timer
void TEMT600_Init_DMATimer(TEMT600_HandleTypeDef *htemt, ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim)
{
	TEMT600_Init(htemt, hadc);
	HAL_TIM_Base_Start(htim);
	_start_dma(htemt);
}


void TEMT600_Sample_DMATimer(TEMT600_HandleTypeDef *htemt)
{
	htemt->is_conversion_completed = 0;
	_calculate_lux(htemt);
}


void _calculate_lux(TEMT600_HandleTypeDef *htemt)
{
	uint16_t sum_buffer = 0;
	for (uint8_t i = 0; i < TEMT600_N_SAMPLES; i++)
		sum_buffer += htemt->buffer[i];

	uint16_t average = sum_buffer/TEMT600_N_SAMPLES;
	uint32_t lux = (TEMT600_CURRENT(average)/TEMT600_RESISTANCE_OHMS)*CURRENT_uA_ONE_LUX;

	htemt->percentage_brightness = (average/TEMT600_RESOLUTION)*100;
	htemt->lux = lux;
}


// Other
void TEMT600_ToJson_Partial(char *buffer, TEMT600_HandleTypeDef *htemt)
{
	memset(buffer, 0, TEMT600_JSON_LENGTH);
	snprintf(buffer, TEMT600_JSON_LENGTH, "\"lux\":%hu,\"bright:%.2f", htemt->lux, htemt->percentage_brightness);
}
