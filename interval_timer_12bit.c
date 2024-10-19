
#include "iodefine.h"
#include "misratypes.h"

#include "ctsu.h"
#include "key.h"
#include "interval_timer_12bit.h"



// intera_timerをベクタアドレス(INIT=0x0038)に登録
#pragma interrupt INT_interval_timer(vect=INTIT)

// インターバルタイマ割り込み処理 (10msec毎に発生)
// 用途:
//      1) CTSUの計測開始トリガ信号(10msec毎)
//	2) タッチされたキーの判定とキー入力確定
//      3) ステッピングモータへのパルス出力
//
void INT_interval_timer(void)
{
	
        touch_key_status_check();	//  各キーのタッチ有り判定値と比較して、タッチの有無を判定
	
	key_input_check( 1 );		// 反時計回り 入力判定
	
	key_input_check( 0 );	        // 時計回り 入力判定
	
	motor_pulse_out();		// ステッピングモータへのパルス出力
}


// インターバルタイマ 12bitの初期設定 (10msec) 　CTSU計測開始のトリガ信号
//　
void interval_timer_12bit_ini(void)
{
	OSMC = 0x10;		// 周辺機能へのサブシステム・クロック供給許可, 低速オンチップ・オシレータ・クロック（fIL）(15 [KHz])をインターバルタイマへ供給　

	TMKAEN = 1;		// 12 ビット・インターバル・タイマへ入力クロック供給 (周辺イネーブル・レジスタ0（PER0）のビット 7 )
	
	ITMC = 149;            //  150 - 1 count, 10msec , ( 15[KHz], 1clock=1/15 [msec] , 150 count/ 15 = 10 [msec])
	
	ITMC = ITMC | 0x8000;      // RINTE:カウンタ動作開始
	
	ITMK = 0;		// 12 ビット・インターバル・タイマ割り込みのマスクをクリア
	
} 