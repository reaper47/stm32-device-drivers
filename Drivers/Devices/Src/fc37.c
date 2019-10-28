#include "fc37.h"

FC37_t fc37;

void FC37_Init()
{
	fc37.rain_intensity = RAIN_INTENSITY_NONE;
	fc37.raw_value = 0;
}


void FC37_Sample()
{
	if (fc37.raw_value > THRESHOLD_RAIN_LIGHT)
		fc37.rain_intensity = RAIN_INTENSITY_NONE;
	else if (fc37.raw_value > THRESHOLD_RAIN_MODERATE)
		fc37.rain_intensity = RAIN_INTENSITY_LIGHT;
	else if (fc37.raw_value > THRESHOLD_RAIN_MODERATE)
		fc37.rain_intensity = RAIN_INTENSITY_MODERATE;
	else
		fc37.rain_intensity = RAIN_INTENSITY_HEAVY;
}


void FC37_ToJson_Partial(char *buffer)
{
	memset(buffer, 0, FC37_JSON_LENGTH);
	snprintf(buffer, FC37_JSON_LENGTH, "\"FC37\":{\"rain\":\"%c\"}", _tochar_rain_intensity(fc37.rain_intensity));
}


char _tochar_rain_intensity(uint8_t intensity)
{
	switch (intensity) {
	case RAIN_INTENSITY_NONE:
		return RAIN_NONE_MESSAGE;
	case RAIN_INTENSITY_LIGHT:
		return RAIN_LIGHT_MESSAGE;
	case RAIN_INTENSITY_MODERATE:
		return RAIN_MODERATE_MESSAGE;
	case RAIN_INTENSITY_HEAVY:
		return RAIN_HEAVY_MESSAGE;
	default:
		return RAIN_NONE_MESSAGE;
	}
}


void FC37_SetRawValue(uint16_t raw_value)
{
	fc37.raw_value = raw_value;
}
