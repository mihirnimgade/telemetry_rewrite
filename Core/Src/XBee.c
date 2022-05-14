#include "XBee.h"

/**
 * Initializes the STM32 UART on GPIO B10 and B11 to a baudrate of 9600
 */
void XBeeInit(void)
{
    // Enable GPIOA clock, AFIO
    RCC->APB2ENR |= 0x9UL;			//Enables Port B Clock and AFIO Clock
    GPIOB->CRH &= ~(0xFF00UL);	    //B11: Floating Input
    GPIOB->CRH |= 0x4B00UL;			//B10: Push-Pull, Output 50MHZ

    RCC->APB1ENR |= (0x1UL << 18);	//Enable USART 3 CLock

    USART3->CR2 = 0;
    USART3->CR3 = 0;
    USART3->CR1 |= 0xC;				//Transmitter and Receiver Enable

    // By Default, set baudrate to 9600
    // Mantissa = 0xEA => 234
    // Fraction = 0x6 => 6/16 = 0.375
    // USARTDIV = Mantissa + Fraction = 234.375

    // Set baud rate to 230400
    // Mantissa = 0x09 => 9
    // Fraction = 0x0C => 12/16 = 0.75
    // USARTDIV = Mantissa + Fraction = 9.75
    USART3->BRR = 0x009CUL;

    // Enable USART3
    USART3->CR1 |= 0x1 << 13;
}

/**
 * Transmits a CAN message via the previously initialized STM32 UART
 * Passes: The CAN header from HAL based code and the data that was extracted in readCANTask
 */
void XBeeTransmitCan(CAN_RxHeaderTypeDef* msg_tx, uint8_t* data)
{
	//Timestamp: 4 bytes
	uint8_t time[2];
	uint8_t count;

	time[0] = 0xFFUL & (RTC->CNTH);
	time[1] = 0xFFUL & (RTC->CNTL);

	for (count = 0; count < 2; count++)
	{
		XBeeSendChar(time[count]);
	}

	//CAN ID: 2 bytes
	uint8_t ID_H = 0xFFUL & ( msg_tx->StdId >> 8);
	uint8_t ID_L = 0xFFUL & ( msg_tx->StdId);

	XBeeSendChar(ID_H);
	XBeeSendChar(ID_L);

	XBeeSendChar(time[0]);
	XBeeSendChar(time[1]);

	//DATA: 8 bytes
	uint8_t c = 0;
	uint8_t byte_t;

	for (c = 0; c < 8; c++)
	{
		byte_t = 0xFFUL & (data[c]);
		XBeeSendChar(byte_t);
	}

	//Length: 1 byte
	XBeeSendByte("0123456789ABCDEF"[ msg_tx->DLC & 0xFUL]);

	//Newline: 2 bytes to send Form Feed and Carriage Return, according to ASCII
	XBeeSendByte(0x0A);
	XBeeSendByte(0x0D);

}

/**
 * Outputs a character in 2 bytes according to its ASCII value
 * For example, 'A' is referenced in ASCII as 0x41, so 4 and 1 are sent
 * as individual bytes.
 * 'z' is referenced in ASCII as 0x7A, so 7 and A are sent in individual
 * bytes
 */
void XBeeSendChar(char c)
{

	//Convert to ASCII
	uint8_t c_H = "0123456789ABCDEF"[(c >> 4) & 0xFUL];
	uint8_t c_L = "0123456789ABCDEF"[ c & 0xFUL];

	XBeeSendByte(c_H);
	XBeeSendByte(c_L);

}

/**
 * Outputs the raw character written here
 * 	If the parameter is 'A', output to console 'A'
 *
 * Passes: a character (1 byte) that will appear on a serial monitor
 */
void XBeeSendByte(char c)
{
	// Write half-byte to data register
	USART3->DR = 0xFF & (c);

	// Poll until character is transmitted
	while (!((USART3->SR >> 7) & 0x1));

}
