#include "weather_utils.h"


float calculate_heat_index_fahrenheit(float T, float RH)
{
	float heat_index =  0.5*(T + 61.0 + ((T - 68.0)*1.2) + (RH*0.094));

	if (heat_index >= 80) {
		heat_index = -42.379 + 2.04901523*T + 10.14333127*RH - .22475541*T*RH - .00683783*T*T - \
					 .05481717*RH*RH + .00122874*T*T*RH + .00085282*T*RH*RH - .00000199*T*T*RH*RH;

		if (RH < 13 && T >= 80 && T <= 112) {
			float adjustment = ((13 - RH)/4)*sqrt((17 - fabs(T - 95.))/17);
			heat_index -= adjustment;
		} else if (RH < 85 && T >= 80 && T <= 87) {
			float adjustment = ((RH - 85)/10)*((87 - T)/5);
			heat_index += adjustment;
		}
	}

	return heat_index;
}


float to_fahrenheit(float T)
{
	return T*1.8 + 32;
}


float to_celsius(float T)
{
	return (T - 32)/1.8;
}
