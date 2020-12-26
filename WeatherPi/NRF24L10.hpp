#pragma once
#include <pigpio.h>

#define CONFIG_REG 0x00
#define EN_AA_REG 0x01
#define EN_RXADDR_REG 0x02
#define SETUP_AW_REG 0x03
#define SETUP_RETR_REG 0x04
#define RF_CH_REG 0x05
#define RF_SETUP_REG 0x06
#define STATUS_REG 0x07
#define OBSERVE_TX_REG 0x08
#define CD_REG 0x09				// CD = Carrier Detect
#define RX_ADDR_P0_REG 0x0A
#define RX_ADDR_P1_REG 0x0B
#define RX_ADDR_P2_REG 0x0C
#define RX_ADDR_P3_REG 0x0D
#define RX_ADDR_P4_REG 0x0E
#define RX_ADDR_P5_REG 0x0F
#define TX_ADDR_REG 0x10
#define RX_PW_P0_REG 0x11
#define RX_PW_P1_REG 0x12
#define RX_PW_P2_REG 0x13
#define RX_PW_P3_REG 0x14
#define RX_PW_P4_REG 0x15
#define RX_PW_P5_REG 0x16
#define FIFO_STATUS_REG 0x17
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define NRF24_CE 27
#define TX_ADDR 0x10

class NRF24L10
{
private:
	int handle = -1;

public:

	int Initialize(int channel);
	char ReadRegister(unsigned char reg);
	char ReadRegisterBytes(unsigned char reg, char* buff, int len);
	int WriteRegister(unsigned char reg, unsigned char value);
	int WriteRegisterBytes(unsigned char reg, char* value, int len);
	char ReadPayload(char* rxBuff, int len);
	int LoadPayload(const char* txBuff);
	int TransmitToChannel(const char* txBuff, char channel);
	char ReadStatus(void);
	void PTXmode(void);
	void PRXmode(void);
	void FlushTX(void);
	void FlushRX(void);
	void ResetRXAddr(void);
};

