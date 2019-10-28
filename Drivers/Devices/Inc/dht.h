#ifndef __DHT11_H
#define __DHT11_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "gpio_ext.h"
#include "time_ext.h"
#include "weather_utils.h"

#define DHT_GPIO_Port GPIOC
#define DHT_Pin       GPIO_PIN_5

#define DHT_N_BITS     40
#define DHT_N_BYTES    5

#define IDX_BYTE_IRH 0
#define IDX_BYTE_DRH 1
#define IDX_BYTE_ITP 2
#define IDX_BYTE_DTP 3
#define IDX_BYTE_CHK 4

#define DHT_JSON_LENGTH 100
#define DHT_POST_LENGTH 500

#define SetBit(A,k)   ( A[(k/8)] |= (1 << (7-(k%8))) )
#define ClearBit(A,k) ( A[(k/8)] &= ~(1 << (7-(k%8))) )

// Structures
typedef struct {
	uint8_t station_id;
	float humidity;
	float temperature_celsius;
	float temperature_fahrenheit;
	float heat_index_celsius;
	float heat_index_fahrenheit;
} DHT_t;

// Public Functions
void DHT_Init(uint8_t station_id);
void DHT_Sample();
void DHT_ToPost(char *buffer, char *endpoint, char *host);
void DHT_ToJson_Partial(char *buffer);
float DHT_GetTemperature(bool in_celsius);

// Private Functions
void _DHT_StartSignal(void);
bool _DHT_IsResponseValid(void);
void _DHT_Read(uint8_t buffer[DHT_N_BYTES]);
bool _DHT_IsReadValid(uint8_t buffer[DHT_N_BYTES]);
bool _DHT_IsDht11(uint8_t buffer[DHT_N_BYTES]);

#endif /* __DHT11_H */
