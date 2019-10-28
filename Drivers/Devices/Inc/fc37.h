#ifndef __FC37_H
#define __FC37_H

#include <string.h>
#include "stm32f3xx_hal.h"

#define RAIN_INTENSITY_NONE     0
#define RAIN_INTENSITY_LIGHT    1
#define RAIN_INTENSITY_MODERATE 2
#define RAIN_INTENSITY_HEAVY    3

#define THRESHOLD_RAIN_LIGHT    2500
#define THRESHOLD_RAIN_MODERATE 1750
#define THRESHOLD_RAIN_HEAVY    1000

#define RAIN_NONE_MESSAGE     'N'
#define RAIN_LIGHT_MESSAGE    'L'
#define RAIN_MODERATE_MESSAGE 'M'
#define RAIN_HEAVY_MESSAGE    'H'

#define FC37_JSON_LENGTH 30

typedef struct {
	uint8_t rain_intensity;
	uint16_t raw_value;
} FC37_t;

// Public Functions
void FC37_Init();
void FC37_Sample();
void FC37_ToJson_Partial(char *buffer);
void FC37_SetRawValue(uint16_t raw_value);

// Private Functions
char _tochar_rain_intensity(uint8_t intensity);

#endif /* __FC37_H */
