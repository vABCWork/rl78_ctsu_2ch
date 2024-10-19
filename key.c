
#include   "iodefine.h"
#include   "misratypes.h"

#include "ctsu.h"
#include "key.h"

// スイッチ入力関係の構造体
//  
struct SW_info
{
	uint8_t status;		// 今回の タッチ有り(Low=0),非タッチ(High=1)状態 (10msec毎)

	uint8_t pre_status;	// 前回(10msec前)の タッチ有り(Low=0),非タッチ(High=1)状態 
	
	uint8_t low_cnt;	// タッチ有りの回数

	uint8_t one_push;	// 0:キー入力処理要求なし 1:キー入力処理要求(1度押し)　（キー入力処理後に0クリア）
	
	uint8_t edge_flg;	// 1: 立下り検出済み
	
	uint16_t threshold;	// ONと判定する CTSUのカウント値 

};


volatile struct  SW_info  Key_sw[CTSU_CH_NUM];	// スイッチ個数分の情報格納領域


//
//   ステッピングモータへのパルス出力
//
// ctsu_sc[0] : TS09:  Key_sw[0]: 時計回り   
// ctsu_sc[1] : TS10:  Key_sw[1]: 反時計回り 
//

void motor_pulse_out(void)
{
	if ( Key_sw[0].one_push == 1 ) {	// 時計回り ON

	     TOE0L_bit.no5 = 1;     		// タイマ出力許可レジスタ  ch5(TO05) 出力許可　(ステッピングモータ パルス出力)
	 
	     P12_bit.no1 = 0;      		// P121 = 0を出力　(ステッピングモータ　方向制御)
	
	}

	else if ( Key_sw[1].one_push == 1 ) {	// 反時計回り ON
	  
             TOE0L_bit.no5 = 1;     		// タイマ出力許可レジスタ  ch5(TO05) 出力許可　(ステッピングモータ パルス出力)
		
	      P12_bit.no1 = 1;       		// P121 = 1を出力　(ステッピングモータ　方向制御)
		
	}
	
	else {
	
            TOE0L_bit.no5 = 0;     		// タイマ出力許可レジスタ  ch5(TO05) 出力禁止　(ステッピングモータ パルス出力なし) 
          
	    TO0L = TO0L & 0xdf;			// チャンネル4 タイマ出力 = 0
	 
	    P12_bit.no1 = 0;       		// P121 = 0を出力　(ステッピングモータ　方向制御)
	
	}

}


//
//  各キーのタッチ有り判定値のセット
// 
void touch_key_on_val(void)
{
	uint8_t i;
	
	for( i = 0; i < CTSU_CH_NUM ; i++ ) {
	
		Key_sw[i].threshold = TOUCH_ON_VALUE;
	}
	
}

//
//   各キーのタッチ有り判定値と比較して、タッチの有無を判定する。
//
// ctsu_sc[0] : TS09:  Key_sw[0]: 時計回り  
// ctsu_sc[1] : TS10:  Key_sw[1]: 反時計回り 
// ctsu_sc[2] : TS11:  key_se[2]: 未使用                                                 
// ctsu_sc[3] : TS13:  key_sw[3]: 未使用
//
void touch_key_status_check(void)
{
	uint8_t i;
	
	for( i = 0; i < CTSU_CH_NUM ; i++ ) {
	
	  if ( ctsu_sc[i] > Key_sw[i].threshold ) {  //  タッチ有り(ON)判定値より大きい場合、
	  
	  	Key_sw[i].status = 0;		// タッチ有り (Low=0)
	  }
	  else {
	  	Key_sw[i].status = 1;		// タッチ無し (High=1)
	  }
	  
	}
}


//
//  時計回り(TS09)、反時計回り(TS10 ) 用 キー入力の確定処理  (10msec毎、タイマ割り込み内で実行)
//    タッチ有り判定ならば、SWオン確定フラグをセットする。
//
//  入力: id = 0 ～ 1
//
// ctsu_sc[0] : TS09:  Key_sw[0]: 時計回り   
// ctsu_sc[1] : TS10:  Key_sw[1]: 反時計回り
//

void key_input_check( uint8_t id ) 
{
	if ( Key_sw[id].status == 0 ) {     // 今回 タッチ状態
	    
	      Key_sw[id].one_push = 1;	    // SWオン確定フラグをセット
	}
	else {				    // 今回 タッチ無し
	      
	       Key_sw[id].one_push = 0;	    // SWオン確定フラグをクリア
	}
}

