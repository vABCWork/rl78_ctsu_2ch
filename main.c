
#include   "iodefine.h"
#include   "misratypes.h"

#include "timer.h"
#include  "ctsu.h"
#include "key.h"
#include  "uart.h"
#include  "interval_timer_12bit.h"
#include  "delay.h"
#include  "led_port.h"

#include   "debug_port.h"

void mode_disp(void);
// メイン処理
// 
void main(void)
{
	
	uart_0_ini();		// UART0 通信の初期化
	
	interval_timer_12bit_ini();	// 12bit インターバルタイマの起動
	
	ctsu_self_set();	//  静電容量式タッチセンサ(CTSU)の設定

	timer_array_ini();	//  入力クロック供給と分周比設定
	timer_ch5_ini();        // ステッピングモータ パルス設定
	
	p121_out_mode();		// ステッピングモータ 回転方向設定

	uart_0_led_port();      // TXD0,RXD0用 LEDポート設定
	alarm_led_port();	// アラーム用 LEDポート設定
	
	touch_key_on_val();	//   各キーのタッチ有り判定値のセット
	
	__EI();			// 割り込み許可
	
	CTSUSTRT = 1; 		 // CTSU計測動作開始
	
	while(1)
	{
	  WDTE = 0xac;		// ウオッチドック　カウンタのクリア
		
		  
	  if ( rcv_over == 1 ) {	// コマンド受信完了
		comm_cmd();		// レスポンス作成、送信
	   	rcv_over = 0;		// コマンド受信フラグのクリア
		rcv_cnt  = 0;		//  受信バイト数の初期
		
	   }
	
	}
	
}


//
// 未使用な割り込みに、登録される関数
//  「リンク・オプション→出力コード→ベクタ・テーブル空き領域のアドレス」で _Dummy とする。
//
//　( コンパイル後、RETI(61 FC))　
//
#pragma interrupt Dummy
void Dummy(void)
{
}




