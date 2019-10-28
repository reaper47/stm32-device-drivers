#include <esp8266.h>

NetworkInfo_t net_info;
ESP8266_t esp;

void ESP8266_Init(UART_HandleTypeDef *huart_device, UART_HandleTypeDef *huart_external)
{
	char host[HOST_LENGTH];
	snprintf(host, sizeof host, "%s:%d", net_info.address, net_info.port);

	esp.huart_device = huart_device;
	esp.huart_external = huart_external;
	ESP8266_UpdateWifiInfo(net_info.ssid, net_info.password);
	ESP8266_UpdateTcpInfo(net_info.address, net_info.port);
	strcpy(esp.host, host);
	esp.current_rx_byte = 0;
	esp.counter_total_bytes_read = 0;
	esp.answer_write_point = 0;
	memset(esp.answer, 0, sizeof esp.answer);
}


void ESP8266_UpdateWifiInfo(char *new_ssid, char *new_password)
{
	strcpy(net_info.ssid, new_ssid);
	strcpy(net_info.password, new_password);

	char at_cwjap[CWJAP_LENGTH];
	snprintf(at_cwjap, sizeof at_cwjap, "AT+CWJAP=\"%s\",\"%s\"\r\n", new_ssid, new_password);
	strcpy(esp.at_cwjap, at_cwjap);
}


void ESP8266_UpdateTcpInfo(char *new_address, uint16_t new_port)
{
	strcpy(net_info.address, new_address);
	net_info.port = new_port;

	char at_cipstart[CIPSTART_LENGTH];
	snprintf(at_cipstart, sizeof at_cipstart, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", new_address, new_port);
	strcpy(esp.at_cipstart, at_cipstart);
}


uint8_t ESP8266_Start()
{
	HAL_UART_Receive_IT(esp.huart_device, (uint8_t*)&esp.current_rx_byte, 1); // Start Receiving
	_ESP8266_AnswerClear();

	if (ESP8266_SendCmd(AT, OK) != AT_OK)
		return _AT_CommandError(esp.huart_external, ESP_START_FAILURE, ERROR_MSG_AT);

	ESP8266_SendCmd(AT_RST, "ready");

	if (ESP8266_SendCmd(ATE0, OK) != AT_OK)
		return _AT_CommandError(esp.huart_external, ESP_START_FAILURE, ERROR_MSG_ATE0);

	if (ESP8266_SendCmd(AT_CIPMUX0, OK) != AT_OK)
		return _AT_CommandError(esp.huart_external, ESP_START_FAILURE, ERROR_MSG_CIPMUX0);

	if (ESP8266_SendCmd(AT_CIPMODE0, OK) != AT_OK)
		return _AT_CommandError(esp.huart_external, ESP_START_FAILURE, ERROR_MSG_CIPMODE0);

	if (_ESP8266_CheckWifiConnection() != AT_OK)
		return _AT_CommandError(esp.huart_external, ESP_START_FAILURE, ERROR_MSG_WIFI);

	HAL_UART_Transmit(esp.huart_external, (uint8_t*)ESP_MSG_START_SUCCESS, (uint16_t) strlen(ESP_MSG_START_SUCCESS), HAL_MAX_DELAY);
	return ESP_START_SUCCESS;
}


uint8_t ESP8266_SendCmd(const char *cmd, const char *examcode)
{
	HAL_UART_Transmit(esp.huart_device, (uint8_t*)cmd, (uint16_t) strlen(cmd), HAL_MAX_DELAY);
	uint8_t at_state = _AT_CheckResponse(examcode, 5);
	_ESP8266_AnswerClear();
	return at_state;
}


uint8_t _AT_CheckResponse(const char *expected_text, uint16_t delay_s)
{
	uint8_t counter = 0;
	while (counter++ < delay_s) {
		if (strstr((char*)esp.answer, expected_text) != NULL)
			return AT_OK;
		HAL_Delay(1000);
	}

	if (strstr((char*)esp.answer, ERROR) != NULL)
		return AT_ERROR;
	return AT_TIMEOUT;
}


void _ESP8266_AnswerClear()
{
	memset(esp.answer, 0, sizeof esp.answer);
	esp.answer_write_point = 0;
}


uint8_t ESP8266_SendData(const char *data)
{
	if (_ESP8266_OpenTcpPort() != AT_OK)
		return _AT_CommandError(esp.huart_external, ESP_START_FAILURE, TCP_CONNECTION_FAILED);

	uint8_t is_data_sent = DATA_NOT_SENT;
	uint16_t len = strlen(data);
	char msg[20] = {"\0"};
	snprintf(msg, sizeof msg, "AT+CIPSEND=%d\r\n", len);

	if (ESP8266_SendCmd(msg, ">") == AT_OK) {
		is_data_sent = DATA_SENT;
		HAL_UART_Transmit(esp.huart_device, (uint8_t*)data, len, HAL_MAX_DELAY);
		HAL_UART_Transmit(esp.huart_external, (uint8_t*)"Data sent\r\n", (uint16_t) strlen("Data sent\r\n"), HAL_MAX_DELAY);
	}

	return is_data_sent;
}


uint8_t _ESP8266_OpenTcpPort()
{
	if (_ESP8266_CheckWifiConnection() != AT_OK)
		return _AT_CommandError(esp.huart_external, ESP_START_FAILURE, ERROR_MSG_WIFI);

	if (ESP8266_SendCmd(AT_CIPSTATUS, "4") == AT_OK ||
		ESP8266_SendCmd(AT_CIPSTATUS, "2") == AT_OK) {
		if (ESP8266_SendCmd(esp.at_cipstart, "OK") != AT_OK)
			return AT_ERROR;
	}

	return AT_OK;
}


uint8_t _ESP8266_CheckWifiConnection()
{
	if (ESP8266_SendCmd(AT_IS_CONNECTED, net_info.ssid) != AT_OK) {
		if (ESP8266_SendCmd(esp.at_cwjap, "WIFI GOT IP") != AT_OK) {
			if (ESP8266_Start() != ESP_START_SUCCESS)
				return AT_ERROR;
		}
	}
	return AT_OK;
}


uint8_t _AT_CommandError(UART_HandleTypeDef *huart, uint8_t error_val, char *message)
{
	HAL_UART_Transmit(huart, (uint8_t*)message, (uint16_t) strlen(message), HAL_MAX_DELAY);
	return error_val;
}


void ESP8266_ReceiveAnswer()
{
	esp.answer[esp.answer_write_point] = esp.current_rx_byte;
	if (esp.answer_write_point < MAX_ANSWER_LENGTH - 1)
		esp.answer_write_point++;
	else
		esp.answer_write_point = 0;

	esp.counter_total_bytes_read++;
	HAL_UART_Receive_IT(esp.huart_device, (uint8_t*)&esp.current_rx_byte, 1);
}

void NetworkInfo_Update(char *ssid, char *password, char *address, uint16_t port, ConnectionType type)
{
	if (ssid) {
		strcpy(net_info.ssid, ssid);
	}

	if (password) {
		strcpy(net_info.password, password);
	}

	if (address) {
		strcpy(net_info.address, address);
	}

	if (port) {
		net_info.port = port;
	}

	if (type) {
		strcpy(net_info.connection_type, _connection_type_to_string(type));
	}
}


char *_connection_type_to_string(ConnectionType type)
{
	switch (type) {
	case TCP:
		return "TCP";
	case UDP:
		return "UDP";
	default:
		return "N/A";
	}
}


char *ESP8266_GetHost()
{
	return esp.host;
}

