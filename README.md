# STM32 Device Drivers

A repository dedicated to various device drivers for the STM32 microcontrollers.

They were developed with the help of the stm32f3xx_hal library for the STM32F303RE microcontroller. However, they should work with any STM32 after adjusting the HAL library to use in a driver's header file.

Any suggestions or fixes are gladly appreciated.

# Table of Contents

* [Drivers](#drivers)
  * [ESP8266 WiFi Microchip](#esp8266)
  * [DHT Thermometers](#dht)

* [Extensions](#extensions)
  * [GPIO](#gpio)
  * [Time](#time)

* [Utilities](#utilities)
  *[Weather](#weather)

# Drivers

## ESP8266

The ESP8266 is a low-cost Wi-Fi microchip with full TCP/IP stack and microcontroller. 

The following steps are required to use the library:
1. Declare a ESP8266 handle: `ESP8266_HandleTypeDef hesp;`
1. Declare a NetworkInfo structure: `NetworkInfo net_info;`
1. Initialize the NetworkInfo: `NetworkInfo_Update(&net_info, SSID, PASSWORD, ADDRESS, PORT, TYPE);`
1. Initialize the handle: `ESP8266_Init(&hesp, &huart3, &huart2, &net_info);`
1. Start the ESP8266 driver: `ESP8266_Start(&hesp);`
1. Transmit data to your receiver with `ESP8266_SendData`

### Structures

Two structures are defined: `NetworkInfo` and `ESP8266_HandleTypeDef`.

#### NetworkInfo

This structure is used for storing information on the network for the ESP8266 microchip. 

```c
typedef struct {
	char ssid[SSID_LENGTH];
	char password[PASSWORD_LENGTH];
	char address[ADDRESS_LENGTH];
	uint16_t port;
	char connection_type[3];
} NetworkInfo;
```

The `ssid` and `password` fields represent the WiFi credentials for the `AT+CWJAP` command.

The `address` and `port` fields represent the parameters to start a TCP or UDP connection via the `AT+CIPSTART` command. The connection type is stored as an `enum`. Its options are `TCP` and `UDP`.

#### ESP8266_HandleTypeDef

This structure is used for storing information used by the ESP8266.

```c
typedef struct {
	UART_HandleTypeDef *huart_device;
	UART_HandleTypeDef *huart_external;
	NetworkInfo *net_info;
	char at_cwjap[CWJAP_LENGTH];
	char at_cipstart[CIPSTART_LENGTH];
	char host[HOST_LENGTH];
	char current_rx_byte;
	uint8_t counter_total_bytes_read;
	uint8_t answer_write_point;
	char answer[MAX_ANSWER_LENGTH];
} ESP8266_HandleTypeDef;
```

### Functions

#### void ESP8266_Init(ESP8266_HandleTypeDef *hesp, UART_HandleTypeDef *huart_device, UART_HandleTypeDef *huart_external, NetworkInfo *net_info)

Initializes the ESP8266 microchip.

*Parameters*
  * `hesp`: the ESP8266 handle
  * `huart_device`: the UART handle to send messages from the STM32 to the ESP8266 and back
  * `huart_external`: the UART handle to send messages from the STM32 to the ESP8266 and back
  * `net_info`: the NetworkInfo structure

#### uint8_t ESP8266_Start(ESP8266_HandleTypeDef *hesp)

Starts the ESP8266 microchip by chaining various AT commands.

*Parameters*
  * `hesp`: the ESP8266 handle

*Returns*
  * `ESP_START_SUCCESS`: the device started properly
  * `ESP_START_FAILURE`: the device failed to start. Check the UART messages to know where it fails.

#### uint8_t ESP8266_SendData(ESP8266_HandleTypeDef *hesp, const char *data)

Sends the desired data from the ESP8266 to the client.

*Parameters*
  * `hesp`: the ESP8266 handle
  * `data`: the desired data.

*Returns*
  * `DATA_SENT`: the data was sent properly
  * `DATA_NOT_SENT`: the data was sent

#### uint8_t ESP8266_SendCmd(ESP8266_HandleTypeDef *hesp, const char *cmd, const char *examcode)

Sends the desired AT command to the ESP8266.

*Parameters*
  * `hesp`: the ESP8266 handle
  * `cmd`: the AT command
  * `examcode`: what the AT response should contain

*Returns*
  * `AT_OK`: the command was sent  properly
  * `AT_ERROR`: the AT command returned "ERROR"
  * `AT_TIMEOUT`: the message took longer than specified delay in the `_AT_CheckResponse` function

#### void ESP8266_UpdateWifiInfo(ESP8266_HandleTypeDef *hesp, char *new_ssid, char *new_password)

Update the NetWorkInfo's wifi information.

*Parameters*
  * `hesp`: the ESP8266 handle
  * `new_ssid`: the new SSID
  * `new_password`: the new password

#### void ESP8266_UpdateTcpInfo(ESP8266_HandleTypeDef *hesp, char *new_address, uint16_t new_port)

Update the NetWorkInfo's host information.

*Parameters*
  * `hesp`: the ESP8266 handle
  * `new_address`: the host's address
  * `new_port`: the host's port

#### void ESP8266_ReceiveAnswer(ESP8266_HandleTypeDef *hesp)

The RX data from the ESP8266 stored in a buffer. 

The following is an example of how the function is used:

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)
		ESP8266_ReceiveAnswer(&hesp);
}
```

*Parameters*
  * `hesp`: the ESP8266 handle

#### void NetworkInfo_Update(NetworkInfo *net_info, char *ssid, char *password, char *address, uint16_t port, ConnectionType type)

Update the NetworkInfo instance. Parameters not updated are NULL.

*Parameters*
  * `net_info`: the NetworkInfo instance
  * `ssid`: the wifi SSID
  * `password`: the wifi password
  * `address`: The host's address
  * `port`: The host's port
  * `type`: the connection type. Either TCP or UDP.

## DHT

This library is used to sample DHT11/DHT22 sensors easily. It takes care of detecting whether a DHT11 or DHT22 is used. It does so by checking the resulting data buffers. If the decimal parts of both the humidity and the temperature are 0, then the it's a DHT11.

The following steps are required to use the library:
1. Adjust the `DHT_GPIO_Port` define to the port you plugged the data pin in
1. Adjust the `DHT_Pin` define to the pin number you plugged the data pin in
1. Call `DHT_Sample()` whenever you want to take a sample
1. Wait at least 1s before taking the next sample

### Structures

#### SampleDHT

This structure is what is returned after sampling. 

```c
typedef struct {
	uint8_t station_id;
	float humidity;
	float temperature_celsius;
	float temperature_fahrenheit;
	float heat_index_celsius;
	float heat_index_fahrenheit;
} SampleDHT;
```

The `station_id` field represents the station where the sensor is. 
For example, your room might be station #1, your basement station #2, your backyard 
station #3, etc.

The remaining fields are self explanatory. The heat index is calculated using the [Heat Index Equation](https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml).

### Functions

#### SampleDHT DHT_sample()

Take a sample.

*Returns*
  * The results in a SampleDHT structure

#### void DHT_ToPost(char *buffer, SampleDHT sample, char *endpoint, char *host)

Sends the message to clients connected to the desired URL.

*Parameters*
  * `buffer`: the buffer to store the HTTP POST header
  * `sample`: the sample returned from DHT_sample()
  * `endpoint`: the post endpoint
  * `host`: the host's address, i.e. "192.168.0.170:8090"

# Extensions

Code that extends the HAL library.

## GPIO

### Functions

#### void GPIO_SetOutput(GPIO_TypeDef *port, uint16_t pin)

Set the GPIO pin to output.

*Parameters*
  * `port`: the pin's port
  * `pin`: the pin number

#### void GPIO_SetInput(GPIO_TypeDef *port, uint16_t pin)

Set the GPIO pin to input.

*Parameters*
  * `port`: the pin's port
  * `pin`: the pin number

## Time

The use of this module is to measure time precisely. The code comes from Carmine Noviello's [Mastering STM32](https://leanpub.com/mastering-stm32) book.

### Functions

#### void DWT_Init();

Inialise the Data Watchpoint and Trace unit.

#### void DWT_DelayUs(uint32_t us);

Delay the execution for a given number of microseconds.

*Parameters*
  * `us`: the number of microseconds

# Utilities

## Weather

A collection of functions used to manipulate weather data.

### Functions

#### float calculate_heat_index_fahrenheit(float T, float RH)

Calculate the heat index using [the Heat Index Equation](https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml).

*Parameters*
  * `T`: the temperature in Fahrenheit
  * `RH`: the relative humidity

*Returns*
  * a float: the heat index in Fahrenheit

#### float to_fahrenheit(float T)

Convert a temperature in Celsius to Fahrenheit

*Parameters*
  * `T`: the temperature in Celsius

*Returns*
  * a float: the temperature in Fahrenheit

#### float to_celsius(float T)

Convert a temperature in Fahrenheit to Celsius

*Parameters*
  * `T`: the temperature in Fahrenheit

*Returns*
  * a float: the temperature in Celsius

