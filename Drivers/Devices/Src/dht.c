#include "dht.h"


void DHT_ToPost(char *buffer, SampleDHT sample, char *endpoint, char *host)
{
	char json[DHT_JSON_LENGTH] = {"\0"};
    snprintf(json, DHT_JSON_LENGTH,
    		 "{\"station_id\":%d,\"RH\":%.1f,\"T_C\":%.1f,\"T_F\":%.1f,\"HI_C\":%.1f,\"HI_F\":%.1f}",
			 sample.station_id, sample.humidity, sample.temperature_celsius, sample.temperature_fahrenheit,
			 sample.heat_index_celsius, sample.heat_index_fahrenheit);

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


void DHT_ToJson_Partial(char *buffer, SampleDHT sample)
{
	memset(buffer, 0, 100);
	snprintf(buffer, 100,
	         "\"station_id\":%d,\"RH\":%.1f,\"T_C\":%.1f,\"T_F\":%.1f,\"HI_C\":%.1f,\"HI_F\":%.1f",
			 sample.station_id, sample.humidity, sample.temperature_celsius, sample.temperature_fahrenheit,
			 sample.heat_index_celsius, sample.heat_index_fahrenheit);
}


SampleDHT DHT_Sample()
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

	SampleDHT sample = {
		.station_id = 1,
		.humidity = humidity,
		.temperature_celsius = temperature_celsius,
		.temperature_fahrenheit = temperature_fahrenheit,
		.heat_index_celsius = to_celsius(heat_index_fahrenheit),
		.heat_index_fahrenheit = heat_index_fahrenheit
	};
	return sample;
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
