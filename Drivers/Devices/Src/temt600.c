#include "temt600.h"

TEMT600_t temt600;

void TEMT600_Init()
{
	temt600.percentage_brightness = 0.0;
	temt600.lux = 0.0;
	temt600.raw_value = 0.0;
}

void TEMT600_Sample()
{
	temt600.percentage_brightness = (temt600.raw_value/TEMT600_RESOLUTION)*100;
	temt600.lux = (TEMT600_CURRENT(temt600.raw_value)/TEMT600_RESISTANCE_OHMS)*CURRENT_uA_ONE_LUX;
}


void TEMT600_ToJson_Partial(char *buffer)
{
	memset(buffer, 0, TEMT600_JSON_LENGTH);
	snprintf(buffer, TEMT600_JSON_LENGTH, "\"TEMT6000\":{\"lux\":%hu}", temt600.lux);
}

void TEMT600_SetRawValue(uint16_t raw_value)
{
	temt600.raw_value = raw_value;
}
