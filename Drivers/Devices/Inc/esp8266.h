#ifndef __ESP8266_H
#define __ESP8266_H

#include <string.h>
#include "stm32f3xx_hal.h"

#define AT                    "AT\r\n"
#define AT_RST                "AT+RST\r\n"
#define ATE0                  "ATE0\r\n"
#define AT_CIFSR              "AT+CIFSR\r\n"
#define AT_CIPMUX0            "AT+CIPMUX=0\r\n"
#define AT_CIPMODE0           "AT+CIPMODE=0\r\n"
#define AT_CIPCLOSE           "AT+CIPCLOSE\r\n"
#define AT_CIPSTATUS          "AT+CIPSTATUS\r\n"
#define AT_IS_CONNECTED       "AT+CWJAP?\r\n"
#define WIFI_UP               "Wifi connection established\r\n"
#define TCP_CONNECTION_FAILED "ERROR: TCP connection failed\r\n"

#define OK    "OK"
#define ERROR "ERROR"

#define AT_OK             0
#define AT_ERROR          1
#define AT_TIMEOUT        2
#define DATA_SENT         0
#define DATA_NOT_SENT     1
#define ESP_START_SUCCESS 0
#define ESP_START_FAILURE 1

#define ESP_MSG_START_SUCCESS "SUCCESS: ESP8266 wake up\r\n"
#define ERROR_MSG_AT          "ERROR: AT command failed on device wakeup\r\n"
#define ERROR_MSG_ATE0        "ERROR: ATE0 command failed on device wakeup\r\n"
#define ERROR_MSG_CIPMUX0     "ERROR: AT+CIPMUX=0 command failed on device wakeup\r\n"
#define ERROR_MSG_CIPMODE0    "ERROR: AT+CIPMODE=0 command failed on device wakeup\r\n"
#define ERROR_MSG_WIFI        "ERROR: Cannot connect to SSID\r\n"

#define SSID_LENGTH        20
#define PASSWORD_LENGTH    50
#define CWJAP_LENGTH       SSID_LENGTH + PASSWORD_LENGTH + 20
#define ADDRESS_LENGTH     15
#define CIPSTART_LENGTH    ADDRESS_LENGTH + 35
#define HOST_LENGTH        25
#define MAX_ANSWER_LENGTH  250

// Structures
typedef enum {
	TCP = 1,
	UDP = 2
} ConnectionType;

typedef struct {
	char ssid[SSID_LENGTH];
	char password[PASSWORD_LENGTH];
	char address[ADDRESS_LENGTH];
	uint16_t port;
	char connection_type[4];
} NetworkInfo_t;

typedef struct {
	UART_HandleTypeDef *huart_device;
	UART_HandleTypeDef *huart_external; // An external UART like FT232RL
	char at_cwjap[CWJAP_LENGTH];
	char at_cipstart[CIPSTART_LENGTH];
	char host[HOST_LENGTH];
	char current_rx_byte;
	uint8_t counter_total_bytes_read;
	uint8_t answer_write_point;
	char answer[MAX_ANSWER_LENGTH];
} ESP8266_t;

// Public Functions
void ESP8266_Init(UART_HandleTypeDef *huart_device, UART_HandleTypeDef *huart_external);
uint8_t ESP8266_Start();
uint8_t ESP8266_SendData(const char *data);
uint8_t ESP8266_SendCmd(const char *cmd, const char *examcode);
void ESP8266_UpdateWifiInfo(char *new_ssid, char *new_password);
void ESP8266_UpdateTcpInfo(char *new_address, uint16_t new_port);
void ESP8266_ReceiveAnswer();
void NetworkInfo_Update(char *ssid, char *password, char *address, uint16_t port, ConnectionType type);
char *ESP8266_GetHost();

// Private Functions
uint8_t _ESP8266_OpenTcpPort();
uint8_t _ESP8266_CheckWifiConnection();
uint8_t _AT_CommandError(UART_HandleTypeDef *huart, uint8_t error_val, char *message);
uint8_t _AT_CheckResponse(const char *expected_text, uint16_t delay_s);
void _ESP8266_AnswerClear();
char *_connection_type_to_string(ConnectionType type);

#endif /* __ESP8266_H */
