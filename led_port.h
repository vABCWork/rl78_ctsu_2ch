
//   P03 : RXD_LED
//   P04 : TXD_LED
//   P05 : ALM_LED

#define RXD_LED_PORT_PMC  PMC0_bit.no3 //  ポート・モード・コントロール・レジスタ
				       // 0:デジタル入出力, 1:アナログ入力 (リセット信号の発生により、FFH)
#define RXD_LED_PORT_PM  PM0_bit.no3   //  ポート・モード・レジスタ
 				       // 0:出力モード(出力バッファON), 1:入力モード(出力バッファOFF) (リセット信号の発生により、FFH)
#define RXD_LED_PORT_OUT P0_bit.no3    //  出力データ



#define TXD_LED_PORT_PMC  PMC0_bit.no4  //  ポート・モード・コントロール・レジスタ
					// 0:デジタル入出力, 1:アナログ入力 (リセット信号の発生により、FFH)
#define TXD_LED_PORT_PM  PM0_bit.no4   //  ポート・モード・レジスタ
				       // 0:出力モード(出力バッファON), 1:入力モード(出力バッファOFF) (リセット信号の発生により、FFH)
#define TXD_LED_PORT_OUT P0_bit.no4    //  出力データ




#define ALM_LED_PORT_POM  POM0_bit.no5  // ポート出力モード・レジスタ
					// 0:通常出力モード, 1:N-ch オープン・ドレイン出力
#define ALM_LED_PORT_PMC  PMC0_bit.no5  //  ポート・モード・コントロール・レジスタ
					// 0:デジタル入出力, 1:アナログ入力 (リセット信号の発生により、FFH)					
					
#define ALM_LED_PORT_PM  PM0_bit.no5   //  ポート・モード・レジスタ
				       // 0:出力モード(出力バッファON), 1:入力モード(出力バッファOFF) (リセット信号の発生により、FFH)
#define ALM_LED_PORT_OUT P0_bit.no5    //  出力データ






void uart_0_led_port(void);

void alarm_led_port(void);
