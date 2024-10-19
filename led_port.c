
#include "iodefine.h"
#include "misratypes.h"
#include "led_port.h"



//
//  TXD,RXD LED 出力ポート設定
//   P03 : RXD_LED
//   P04 : TXD_LED
//
void uart_0_led_port(void)
{
		
	TXD_LED_PORT_PMC = 0;	// デジタル入出力(アナログ入力ではない)
	TXD_LED_PORT_PM = 0;	// 出力モード
	TXD_LED_PORT_OUT = 0;       // 0 出力
	
	
	RXD_LED_PORT_PMC = 0;  // デジタル入出力(アナログ入力ではない)
	RXD_LED_PORT_PM = 0;	// 出力モード
	RXD_LED_PORT_OUT = 0;	// 0 出力
	
	
	
}


//
//  Alarm LED 出力ポート設定
//   P05 : ALM_LED
//
void alarm_led_port(void)
{
	ALM_LED_PORT_POM = 0;   // 通常出力モード(Nchオープンドレイン出力でない)
	ALM_LED_PORT_PMC = 0;	// デジタル入出力(アナログ入力ではない)
	ALM_LED_PORT_PM = 0;	// 出力モード
	ALM_LED_PORT_OUT = 0;       // 0 出力
	
}
