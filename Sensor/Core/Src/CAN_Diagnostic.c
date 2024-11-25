#include "CAN_Diagnostic.h"
#include "stdio.h"


UART_HandleTypeDef huart1;
CAN_TxHeaderTypeDef TxMsg;
CAN_RxHeaderTypeDef RxMsg;
CAN_FilterTypeDef canfilterconfig;


uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;

uint8_t seed[4], key[16];

void CAN_Config(CAN_HandleTypeDef *hcan)
{
	canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig.FilterBank = 18; // which filter bank to use from the assigned ones
	canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	canfilterconfig.FilterIdHigh = CAN_DIAGNOSTIC_RESPONSE_ID << 5;
	canfilterconfig.FilterIdLow = 0;
	canfilterconfig.FilterMaskIdHigh = 0xFFFF;
	canfilterconfig.FilterMaskIdLow = 0x0000;
	canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canfilterconfig.SlaveStartFilterBank = 14; // how many filters to assign to the CAN1 (master can)

	HAL_CAN_ConfigFilter(hcan, &canfilterconfig);
}

void CAN_ReadDataRequest(CAN_HandleTypeDef *hcan)
{
	TxMsg.StdId = CAN_DIAGNOSTIC_REQUEST_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = CAN_DATA_LENGTH;

	TxData[0] = CAN_DIAGNOSTIC_REQUEST_ID & 0xff;
	TxData[1] = READ_DATA_REQUEST_SID;
	TxData[2] = READ_CANID_VALUE >> 8;
	TxData[3] = READ_CANID_VALUE & 0xff;
	TxData[4] = CAN_UNUSED_DATA;
	TxData[5] = CAN_UNUSED_DATA;
	TxData[6] = CAN_UNUSED_DATA;
	TxData[7] = CAN_UNUSED_DATA;

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);
}

void CAN_WriteDataRequest(CAN_HandleTypeDef *hcan)
{
	TxMsg.StdId = CAN_DIAGNOSTIC_REQUEST_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = READ_WRITE_REQUEST_LENGTH;

	TxData[0] = CAN_DIAGNOSTIC_REQUEST_ID & 0xff;
	TxData[1] = WRITE_DATA_REQUEST_SID;
	TxData[2] = 0x01;
	TxData[3] = 0x23;
	TxData[4] = CAN_UNUSED_DATA;
	TxData[5] = CAN_UNUSED_DATA;
	TxData[6] = CAN_UNUSED_DATA;
	TxData[7] = CAN_UNUSED_DATA;

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);
}

void CAN_SecuritySeedRequest(CAN_HandleTypeDef *hcan)
{
	TxMsg.StdId = CAN_DIAGNOSTIC_REQUEST_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = CAN_DATA_LENGTH;

	TxData[0] = CAN_DIAGNOSTIC_REQUEST_ID & 0xff;
	TxData[1] = CAN_SECURITY_ACCESS_REQUEST_SID;
	TxData[2] = CAN_SECURITY_SEED_LEVEL;
	TxData[3] = CAN_UNUSED_DATA;
	TxData[4] = CAN_UNUSED_DATA;
	TxData[5] = CAN_UNUSED_DATA;
	TxData[6] = CAN_UNUSED_DATA;
	TxData[7] = CAN_UNUSED_DATA;

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);
}

void CAN_SecurityKeyRequest(CAN_HandleTypeDef *hcan)
{
	TxMsg.StdId = CAN_DIAGNOSTIC_REQUEST_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = CAN_DATA_LENGTH;

	TxData[0] = CAN_DIAGNOSTIC_REQUEST_ID & 0xff;
	TxData[1] = CAN_SECURITY_ACCESS_REQUEST_SID;
	TxData[2] = CAN_SECURITY_KEY_LEVEL;
	TxData[3] = key[0];
	TxData[4] = key[1];
	TxData[5] = key[2];
	TxData[6] = key[3];

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);
}

//void securityRemainKeySend(CAN_HandleTypeDef *hcan)
//{
//	TxMsg.StdId = CAN_DIAGNOSTIC_REQUEST_ID;
//	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
//	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
//	TxMsg.DLC = CAN_DATA_LENGTH;
//
//	TxData[0] = CAN_DIAGNOSTIC_REQUEST_ID & 0xff;
//	TxData[1] = CAN_SECURITY_ACCESS_REQUEST_SID;
//	TxData[2] = CAN_SECURITY_KEY_LEVEL;
//	TxData[3] = key[4];
//	TxData[4] = key[5];
//	TxData[5] = key[6];
//	TxData[6] = key[7];
//
//	CAN_Transmit(hcan, &CANTxHeaderRequest, CANTxRequest, &CANTxMailboxesRequest);
//
//	HAL_Delay(100);
//
//	CANTxRequest[0] 	= 0x22;
//	CANTxRequest[1] 	= key[11];
//	CANTxRequest[2] 	= key[12];
//	CANTxRequest[3]		= key[13];
//	CANTxRequest[4]		= key[14];
//	CANTxRequest[5] 	= key[15];
//	CANTxRequest[6]		= UNUSED_DATA;
//	CANTxRequest[7]		= UNUSED_DATA;
//
//	CAN_Transmit(hcan, &CANTxHeaderRequest, CANTxRequest, &CANTxMailboxesRequest);
//
//}

void CAN_ReadDataResponeCheck(uint8_t RxData[])
{
	if (RxData[1] == POSITIVE_RESPONSE_SID(READ_DATA_REQUEST_SID)) {
		if ((RxData[2] == (READ_CANID_VALUE >> 8)) && (RxData[3] == (READ_CANID_VALUE & 0xff))
				&& RxData[4] == (CAN_DIAGNOSTIC_RESPONSE_ID & 0xff))
			HAL_GPIO_TogglePin(LEDR_GPIO_Port, LEDR_Pin);
	}
}

void CAN_WriteDataResponeCheck(uint8_t RxData[])
{
	if (RxData[1] == POSITIVE_RESPONSE_SID(WRITE_DATA_REQUEST_SID))
		HAL_GPIO_TogglePin(LEDG_GPIO_Port, LEDG_Pin);
}

void CAN_SecuritySeedResponeCheck(uint8_t RxData[])
{
	seed[0] = RxData[2];
	seed[1] = RxData[3];
	seed[2] = RxData[4];
	seed[3] = RxData[5];

	key[0] = seed[0] ^ seed[1];
	key[1] = seed[1] + seed[2];
	key[2] = seed[2] ^ seed[3];
	key[3] = seed[3] + seed[0];

	key[4] = seed[0] | seed[1];
	key[5] = seed[1] + seed[2];
	key[6] = seed[2] | seed[3];
	key[7] = seed[3] + seed[1];

	key[8] = seed[0] & seed[1];
	key[9] = seed[1] ^ seed[2];
	key[10] = seed[2] & seed[3];
	key[11] = seed[3] ^ seed[0];

	key[12] = seed[0] - seed[1];
	key[13] = seed[1] ^ seed[2];
	key[14] = seed[2] - seed[3];
	key[15] = seed[3] ^ seed[0];
}

void CAN_SecurityKeyResponeCheck(uint8_t RxData[])
{
	if (RxData[1] == POSITIVE_RESPONSE_SID(CAN_SECURITY_ACCESS_REQUEST_SID))
		if (RxData[2] == CAN_SECURITY_KEY_LEVEL)
		{
			HAL_GPIO_TogglePin(LEDR_GPIO_Port, LEDR_Pin);
			HAL_GPIO_TogglePin(LEDG_GPIO_Port, LEDG_Pin);
		}
}

void CAN_ErrorHandler(uint8_t RxData[])
{
	printf("SID: 0x%02X%02X\n", RxData[1], RxData[2]);
}




#if defined(__GNUC__)
int _write(int fd, char * ptr, int len) {
  HAL_UART_Transmit( & huart1, (uint8_t * ) ptr, len, HAL_MAX_DELAY);
  return len;
}
#elif defined(__ICCARM__)#include "LowLevelIOInterface.h"

size_t __write(int handle,
  const unsigned char * buffer, size_t size) {
  HAL_UART_Transmit( & huart1, (uint8_t * ) buffer, size, HAL_MAX_DELAY);
  return size;
}
#elif defined(__CC_ARM)
int fputc(int ch, FILE * f) {
  HAL_UART_Transmit( & huart1, (uint8_t * ) & ch, 1, HAL_MAX_DELAY);
  return ch;
}
#endif
