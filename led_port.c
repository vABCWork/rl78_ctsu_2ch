
#include "iodefine.h"
#include "misratypes.h"
#include "led_port.h"



//
//  TXD,RXD LED �o�̓|�[�g�ݒ�
//   P03 : RXD_LED
//   P04 : TXD_LED
//
void uart_0_led_port(void)
{
		
	TXD_LED_PORT_PMC = 0;	// �f�W�^�����o��(�A�i���O���͂ł͂Ȃ�)
	TXD_LED_PORT_PM = 0;	// �o�̓��[�h
	TXD_LED_PORT_OUT = 0;       // 0 �o��
	
	
	RXD_LED_PORT_PMC = 0;  // �f�W�^�����o��(�A�i���O���͂ł͂Ȃ�)
	RXD_LED_PORT_PM = 0;	// �o�̓��[�h
	RXD_LED_PORT_OUT = 0;	// 0 �o��
	
	
	
}


//
//  Alarm LED �o�̓|�[�g�ݒ�
//   P05 : ALM_LED
//
void alarm_led_port(void)
{
	ALM_LED_PORT_POM = 0;   // �ʏ�o�̓��[�h(Nch�I�[�v���h���C���o�͂łȂ�)
	ALM_LED_PORT_PMC = 0;	// �f�W�^�����o��(�A�i���O���͂ł͂Ȃ�)
	ALM_LED_PORT_PM = 0;	// �o�̓��[�h
	ALM_LED_PORT_OUT = 0;       // 0 �o��
	
}
