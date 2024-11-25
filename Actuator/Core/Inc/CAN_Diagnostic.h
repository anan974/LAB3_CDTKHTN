/*
 * CAN_Diagnostic.h
 *
 *  Created on: Nov 25, 2024
 *      Author: THANH AN
 */

#ifndef INC_CAN_DIAGNOSTIC_H_

#include "main.h"

#define CAN_DIAGNOSTIC_REQUEST_ID		0x712
#define CAN_DIAGNOSTIC_RESPONSE_ID		0x7A2

#define CAN_PCI_SINGLE_FRAME 			0x00
#define CAN_PCI_FIRST_FRAME				0x01
#define CAN_PCI_CONSECUTIVE_FRAME		0x02
#define CAN_PCI_FLOW_CONTROL			0x03

#define READ_WRITE_REQUEST_LENGTH		0x04
#define CAN_UNUSED_DATA					0x55

#define NEGATIVE_RESPONSE				0x7f
#define NEGATIVE_RESPONSE_LENGTH 		0x03
#define POSITIVE_RESPONSE_SID(X)		(X + 0x40)
#define READ_DATA_REQUEST_SID 			0x22
#define READ_DATA_RESPONSE_SID 			0x62
#define READ_CANID_VALUE				0x0123
#define READ_POSITIVE_RESPONSE_LENGTH	0x05
#define WRITE_DATA_REQUEST_SID			0x2e
#define WRITE_DATA_RESPONSE_SID			0x6e
#define WRITE_DATA_BY_ID_RECORD			0x0123
#define WRITE_POSITIVE_RESPONSE_LENGTH	0x01
#define CAN_SECURITY_ACCESS_REQUEST_SID	0x27
#define CAN_SECURITY_ACCESS_RESPONSE_SID 0x67
#define CAN_SEED_REQUEST_LENGTH	 		0x02
#define CAN_UNLOCK_REQUEST_LENGTH	 	0x12
#define CAN_SECURITY_SEED_LEVEL			0x01
#define CAN_SECURITY_KEY_LEVEL			0x02
#define CAN_DATA_LENGTH					0x08

#define CAN_SECURITY_KEY_RV1ST			0x28
#define CAN_SECURITY_KEY_RV2ND			0x29
#define CAN_SECURITY_KEY_RV3RD			0x30

#define CAN_INVALID_LENGTH				0x13
#define WRITE_DID_NOT_SUPPORT			0x31
#define SECURITY_INVALID_KEY			0x35

void CAN_Config(CAN_HandleTypeDef *hcan);

void CAN_ReadDataRespone(CAN_HandleTypeDef *hcan);

void CAN_WriteDataRespone(CAN_HandleTypeDef *hcan);

void CAN_SecuritySeedRespone(CAN_HandleTypeDef *hcan);

void CAN_SecurityKeyRespone(CAN_HandleTypeDef *hcan);

void CAN_NegativeRespone(CAN_HandleTypeDef *hcan,uint8_t SID, uint8_t message);

uint8_t CAN_SecurityKeyCheck(void);

#define INC_CAN_DIAGNOSTIC_H_

#endif /* INC_CAN_DIAGNOSTIC_H_ */
