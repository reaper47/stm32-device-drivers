#ifndef __FC37_H
#define __FC37_H

#include <string.h>
#include "stm32f3xx_hal.h"

#define RAIN_COUNTER_N_READINGS 10

#define RAIN_INTENSITY_NONE     0
#define RAIN_INTENSITY_LIGHT    1
#define RAIN_INTENSITY_MODERATE 2
#define RAIN_INTENSITY_HEAVY    3

#define THRESHOLD_RAIN_LIGHT    2500
#define THRESHOLD_RAIN_MODERATE 1750
#define THRESHOLD_RAIN_HEAVY    1000

#define FC37_JSON_LENGTH 30

typedef struct {
	uint8_t rain_intensity;
	uint16_t raw_value;
} FC37_HandleTypeDef;

// Public Functions
void FC37_Init(FC37_HandleTypeDef *hfc37);
void FC37_AssessRain(FC37_HandleTypeDef *hfc37);
void FC37_ToJson_Partial(char *buffer, FC37_HandleTypeDef *hfc37);

// Private Functions
char _tochar_rain_intensity(uint8_t intensity);

#endif /* __FC37_H */
