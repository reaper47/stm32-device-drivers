#include "temt600.h"


void TEMT600_Init(TEMT600_HandleTypeDef *htemt)
{
	htemt->percentage_brightness = 0.0;
	htemt->lux = 0;
	htemt->raw_value = 0;
}

void TEMT600_CalculateLux(TEMT600_HandleTypeDef *htemt)
{
	htemt->percentage_brightness = (htemt->raw_value/TEMT600_RESOLUTION)*100;
	htemt->lux = (TEMT600_CURRENT(htemt->raw_value)/TEMT600_RESISTANCE_OHMS)*CURRENT_uA_ONE_LUX;
}


// Other
void TEMT600_ToJson_Partial(char *buffer, TEMT600_HandleTypeDef *htemt)
{
	memset(buffer, 0, TEMT600_JSON_LENGTH);
	snprintf(buffer, TEMT600_JSON_LENGTH, "\"lux\":%hu", htemt->lux);
}
