#ifndef __WEATHER_UTILS_H
#define __WEATHER_UTILS_H

#include <math.h>

typedef struct {
	float fahrenheit;
	float celsius;
} Temperature_t;

float calculate_heat_index_fahrenheit(float T, float RH);
float to_fahrenheit(float T);
float to_celsius(float T);

#endif /* __WEATHER_UTILS_H */
