#include "fc37.h"


void FC37_Init(FC37_HandleTypeDef *hfc37)
{
	hfc37->rain_intensity = RAIN_INTENSITY_NONE;
	hfc37->raw_value = 0;
}


void FC37_AssessRain(FC37_HandleTypeDef *hfc37)
{
	if (hfc37->raw_value > THRESHOLD_RAIN_LIGHT)
		hfc37->rain_intensity = RAIN_INTENSITY_NONE;
	else if (hfc37->raw_value > THRESHOLD_RAIN_MODERATE)
		hfc37->rain_intensity = RAIN_INTENSITY_LIGHT;
	else if (hfc37->raw_value > THRESHOLD_RAIN_MODERATE)
		hfc37->rain_intensity = RAIN_INTENSITY_MODERATE;
	else
		hfc37->rain_intensity = RAIN_INTENSITY_HEAVY;
}


void FC37_ToJson_Partial(char *buffer, FC37_HandleTypeDef *hfc37)
{
	memset(buffer, 0, FC37_JSON_LENGTH);
	snprintf(buffer, FC37_JSON_LENGTH, "\"rain\":\"%c\"", _tochar_rain_intensity(hfc37->rain_intensity));
}


char _tochar_rain_intensity(uint8_t intensity)
{
	switch (intensity) {
	case RAIN_INTENSITY_NONE:
		return 'N';
	case RAIN_INTENSITY_LIGHT:
		return 'L';
	case RAIN_INTENSITY_MODERATE:
		return 'M';
	case RAIN_INTENSITY_HEAVY:
		return 'H';
	default:
		return 'N';
	}
}
