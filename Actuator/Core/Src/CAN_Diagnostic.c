#include "CAN_Diagnostic.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

CAN_TxHeaderTypeDef TxMsg;
CAN_RxHeaderTypeDef RxMsg;
CAN_FilterTypeDef canfilterconfig;


uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;

uint8_t seed[4], key[16], rcv_key[16];

uint8_t rv_keyTable = 0;

void CAN_Config(CAN_HandleTypeDef *hcan)
{
	canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig.FilterBank = 18; // which filter bank to use from the assigned ones
	canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	canfilterconfig.FilterIdHigh = CAN_DIAGNOSTIC_REQUEST_ID << 5;
	canfilterconfig.FilterIdLow = 0;
	canfilterconfig.FilterMaskIdHigh = 0xFFFF;
	canfilterconfig.FilterMaskIdLow = 0x0000;
	canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canfilterconfig.SlaveStartFilterBank = 14; // how many filters to assign to the CAN1 (master can)

	HAL_CAN_ConfigFilter(hcan, &canfilterconfig);
}

void CAN_ReadDataRespone(CAN_HandleTypeDef *hcan)
{
	uint8_t CAN_DL = RxData[0] & 0x0f;
	if (CAN_DL != 0x03)
		{
			CAN_NegativeRespone(hcan, RxData[1], CAN_INVALID_LENGTH);
			return;
		}

	TxMsg.StdId = CAN_DIAGNOSTIC_RESPONSE_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = CAN_DATA_LENGTH;

	TxData[0] = CAN_UNUSED_DATA;
	TxData[1] = POSITIVE_RESPONSE_SID(READ_DATA_REQUEST_SID);
	TxData[2] = READ_CANID_VALUE >> 8;
	TxData[3] = READ_CANID_VALUE & 0xff;
	TxData[4] = CAN_DIAGNOSTIC_RESPONSE_ID & 0xff;
	TxData[5] = CAN_UNUSED_DATA;
	TxData[6] = CAN_UNUSED_DATA;
	TxData[7] = CAN_UNUSED_DATA;

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);

	HAL_GPIO_TogglePin(LEDR_GPIO_Port, LEDR_Pin);
}

void CAN_WriteDataRespone(CAN_HandleTypeDef *hcan)
{
	uint8_t CAN_DL = RxData[0] & 0x0f;
	uint16_t CAN_DID = (RxData[2] << 8) | (RxData[3] << 0);

	if (CAN_DL < 0x03)
		{
			CAN_NegativeRespone(hcan, RxData[1], CAN_INVALID_LENGTH);
			return;
		}
		if (CAN_DID != 0x0123)
		{
			CAN_NegativeRespone(hcan, RxData[1], WRITE_DID_NOT_SUPPORT);
			return;
		}

	TxMsg.StdId = CAN_DIAGNOSTIC_RESPONSE_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = CAN_DATA_LENGTH;

	TxData[0] = CAN_UNUSED_DATA;
	TxData[1] = POSITIVE_RESPONSE_SID(WRITE_DATA_REQUEST_SID);
	TxData[2] = 0x01;
	TxData[3] = 0x23;
	TxData[4] = CAN_UNUSED_DATA;
	TxData[5] = CAN_UNUSED_DATA;
	TxData[6] = CAN_UNUSED_DATA;
	TxData[7] = CAN_UNUSED_DATA;

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);

	HAL_GPIO_TogglePin(LEDG_GPIO_Port, LEDG_Pin);
}

void CAN_SecuritySeedRespone(CAN_HandleTypeDef *hcan)
{

	TxMsg.StdId = CAN_DIAGNOSTIC_RESPONSE_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = CAN_DATA_LENGTH;

	TxData[0] = CAN_UNUSED_DATA;
	TxData[1] = POSITIVE_RESPONSE_SID(CAN_SECURITY_ACCESS_REQUEST_SID);
	TxData[2] = CAN_SECURITY_SEED_LEVEL;

	srand(HAL_GetTick());
	/* Generate SEED */
	TxData[3] = rand() % 256;
	TxData[4] = rand() % 256;
	TxData[5] = rand() % 256;
	TxData[6] = rand() % 256;
	TxData[7] = CAN_UNUSED_DATA;

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);

	/* Save SEED and calculate KEY*/
	seed[0] = TxData[3];
	seed[1] = TxData[4];
	seed[2] = TxData[5];
	seed[3] = TxData[6];

	key[0] 	= seed[0] ^ seed[1];
	key[1] 	= seed[1] + seed[2];
	key[2] 	= seed[2] ^ seed[3];
	key[3] 	= seed[3] + seed[0];

	key[4] 	= seed[0] | seed[1];
	key[5] 	= seed[1] + seed[2];
	key[6] 	= seed[2] | seed[3];
	key[7] 	= seed[3] + seed[1];

	key[8] 	= seed[0] & seed[1];
	key[9] 	= seed[1] ^ seed[2];
	key[10] = seed[2] & seed[3];
	key[11] = seed[3] ^ seed[0];

	key[12] = seed[0] - seed[1];
	key[13] = seed[1] ^ seed[2];
	key[14] = seed[2] - seed[3];
	key[15] = seed[3] ^ seed[0];

}

void CAN_SecurityKeyRespone(CAN_HandleTypeDef *hcan)
{
	if (!CAN_SecurityKeyCheck()) CAN_NegativeRespone(hcan, CAN_SECURITY_ACCESS_REQUEST_SID, SECURITY_INVALID_KEY);

	TxMsg.StdId = CAN_DIAGNOSTIC_RESPONSE_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = CAN_DATA_LENGTH;

	TxData[0] = CAN_UNUSED_DATA;
	TxData[1] = POSITIVE_RESPONSE_SID(CAN_SECURITY_ACCESS_REQUEST_SID);
	TxData[2] = CAN_SECURITY_KEY_LEVEL;
	TxData[3] = CAN_UNUSED_DATA;
	TxData[4] = CAN_UNUSED_DATA;
	TxData[5] = CAN_UNUSED_DATA;
	TxData[6] = CAN_UNUSED_DATA;
	TxData[7] = CAN_UNUSED_DATA;

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);
}

void CAN_NegativeRespone(CAN_HandleTypeDef *hcan,uint8_t SID, uint8_t message)
{
	TxMsg.StdId = CAN_DIAGNOSTIC_RESPONSE_ID;
	TxMsg.RTR = CAN_RTR_DATA; //Remote transmission request = Data frame
	TxMsg.IDE = CAN_ID_STD; 	 //Standard Id (11 bits for the identifier)
	TxMsg.DLC = NEGATIVE_RESPONSE_LENGTH;

	TxData[0] = CAN_UNUSED_DATA;
	TxData[1] = NEGATIVE_RESPONSE;
	TxData[2] = SID;
	TxData[3] = message;
	TxData[4] = CAN_UNUSED_DATA;
	TxData[5] = CAN_UNUSED_DATA;
	TxData[6] = CAN_UNUSED_DATA;
	TxData[7] = CAN_UNUSED_DATA;

	HAL_CAN_AddTxMessage(hcan, &TxMsg, TxData, &TxMailbox);
}

uint8_t CAN_SecurityKeyCheck(void)
{
	for (int _key = 0; _key < 16; ++ _key)
		{
			if (rcv_key[_key] != key[_key]) return 0;
		}
	return 1;
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

