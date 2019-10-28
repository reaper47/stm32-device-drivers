#include "dht.h"

DHT_t dht;

void DHT_Init(uint8_t station_id)
{
	dht.heat_index_celsius = 0.0;
	dht.heat_index_fahrenheit = 0.0;
	dht.humidity = 0.0;
	dht.station_id = station_id;
	dht.temperature_celsius = 0.0;
	dht.temperature_fahrenheit = 0.0;
}


void DHT_ToPost(char *buffer, char *endpoint, char *host)
{
	char json[DHT_JSON_LENGTH] = {"\0"};
    snprintf(json, DHT_JSON_LENGTH,
    		 "{\"DHT\":{\"station_id\":%d,\"RH\":%.1f,\"T_C\":%.1f,\"T_F\":%.1f,\"HI_C\":%.1f,\"HI_F\":%.1f}}",
			 dht.station_id, dht.humidity, dht.temperature_celsius, dht.temperature_fahrenheit,
			 dht.heat_index_celsius, dht.heat_index_fahrenheit);

    memset(buffer, 0, DHT_POST_LENGTH);
    snprintf(buffer, DHT_POST_LENGTH,
    		 "POST %s HTTP/1.1\r\n"
    		 "Host: %s\r\n"
    		 "Content-Type: application/json\r\n"
    		 "Content-Length: %d\r\n"
    		 "\r\n"
    		 "%s\r\n"
    		 "\r\n", endpoint, host, strlen(json), json);
}


void DHT_ToJson_Partial(char *buffer)
{
	memset(buffer, 0, DHT_JSON_LENGTH);
	snprintf(buffer, DHT_JSON_LENGTH,
	         "\"DHT\":{"
	         "\"station_id\":%d,"
	         "\"RH\":%.1f,"
	         "\"T_C\":%.1f,"
	         "\"T_F\":%.1f,"
	         "\"HI_C\":%.1f,"
	         "\"HI_F\":%.1f}",
			 dht.station_id, dht.humidity, dht.temperature_celsius,
			 dht.temperature_fahrenheit, dht.heat_index_celsius, dht.heat_index_fahrenheit);
}


void DHT_Sample()
{
	uint8_t buffer[DHT_N_BYTES] = {0};

	_DHT_StartSignal();
	if (_DHT_IsResponseValid())
		_DHT_Read(buffer);

	float temperature_celsius = 0.0;
	float temperature_fahrenheit = 0.0;
	float humidity = 0.0;
	float heat_index_fahrenheit = 0.0;

	if (_DHT_IsReadValid(buffer)) {
		if (_DHT_IsDht11(buffer)) {
			humidity = buffer[IDX_BYTE_IRH] + buffer[IDX_BYTE_DRH]/10.0;
			temperature_celsius = buffer[IDX_BYTE_ITP] + buffer[IDX_BYTE_DTP]/10.0;
		} else {
			humidity = ((buffer[IDX_BYTE_IRH] << 8) | buffer[IDX_BYTE_DRH])*0.1;
			temperature_celsius = (((buffer[IDX_BYTE_ITP] & 0x7F) << 8) | buffer[IDX_BYTE_DTP])*0.1;
			if (buffer[IDX_BYTE_ITP] & 0x80)
				temperature_celsius *= -1;
		}

		temperature_fahrenheit = to_fahrenheit(temperature_celsius);
		heat_index_fahrenheit = calculate_heat_index_fahrenheit(temperature_fahrenheit, humidity);
	}

	dht.humidity = humidity;
	dht.temperature_celsius = temperature_celsius;
	dht.temperature_fahrenheit = temperature_fahrenheit;
	dht.heat_index_celsius = to_celsius(heat_index_fahrenheit);
	dht.heat_index_fahrenheit = heat_index_fahrenheit;
}


void _DHT_StartSignal(void)
{
	GPIO_SetOutput(DHT_GPIO_Port, DHT_Pin);
	HAL_GPIO_WritePin(DHT_GPIO_Port, DHT_Pin, 0);
	DWT_DelayUs(18000);
	HAL_GPIO_WritePin(DHT_GPIO_Port, DHT_Pin, 1);
	DWT_DelayUs(40);
	GPIO_SetInput(DHT_GPIO_Port, DHT_Pin);
}


bool _DHT_IsResponseValid(void)
{
	bool is_checked = false;
	if (!(HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin))) {
		DWT_DelayUs(80);
		is_checked = HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin);
		DWT_DelayUs(80);
	}
	return is_checked;
}


void _DHT_Read(uint8_t buffer[DHT_N_BYTES])
{
	for (uint8_t i = 0; i < DHT_N_BITS; i++) {
		while (!(HAL_GPIO_ReadPin (DHT_GPIO_Port, DHT_Pin)));
		DWT_DelayUs(40);
		if (HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin) == 0)
			ClearBit(buffer, i);
		else
			SetBit(buffer, i);
		while (HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin));
	}
}


bool _DHT_IsReadValid(uint8_t buffer[DHT_N_BYTES])
{
	uint8_t sum = buffer[IDX_BYTE_IRH] + buffer[IDX_BYTE_DRH] + buffer[IDX_BYTE_ITP] + buffer[IDX_BYTE_DTP];
	return sum == buffer[IDX_BYTE_CHK];
}


bool _DHT_IsDht11(uint8_t buffer[DHT_N_BYTES])
{
	return (buffer[IDX_BYTE_DRH] == 0 && buffer[IDX_BYTE_DTP] == 0);
}


float DHT_GetTemperature(bool in_celsius)
{
	if (in_celsius)
		return dht.temperature_celsius;
	return dht.temperature_fahrenheit;
}
