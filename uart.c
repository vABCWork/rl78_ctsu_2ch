
#include "iodefine.h"
#include "misratypes.h"
#include "ctsu.h"

#include "uart.h"
#include "led_port.h"


// 受信用
volatile uint8_t  rcv_data[64];
volatile uint8_t rcv_over;
volatile uint8_t  rcv_cnt;

volatile uint8_t  rcv_crc_err;
volatile uint16_t rcv_status;

uint16_t rcv_crc_code;

// 送信用
volatile uint8_t sd_data[64];
volatile uint8_t  send_pt;

volatile uint8_t  send_num;



//  UART0 送信バッファ空き割り込み先頭アドレスを、INTST0(0x0012)へ登録
#pragma interrupt INT_uart0_send(vect=INTST0)

// UART0 送信割り込み処理
void INT_uart0_send(void)
{
	SDR00 = sd_data[send_pt];	// データの送信
	
	send_pt++;		// 送信データの位置をインクリメント
	
	TXD_LED_PORT_OUT = 1;   	//  TXD LED 点灯
	
	if ( send_pt == send_num ) { // 最終データの送信完了
	   	STMK0 = 1;		// UART0 送信の転送完了、バッファ空き割り込みの禁止 (割り込みマスクセット)
		TXD_LED_PORT_OUT = 0;   	//  TXD LED 消灯
	}
}



// UART0 受信割り込み処理先頭アドレスを、INTSR0(0x0014)へ登録
#pragma interrupt INT_uart0_rcv(vect=INTSR0)

// UART0 受信割り込み処理
void INT_uart0_rcv(void)
{
	rcv_data[rcv_cnt] = SDR01;	// 受信データの読み出し
	
	rcv_status = SSR01;	// シリアルステータスの読み出し
	
	rcv_cnt++;
	
	RXD_LED_PORT_OUT = 1;	//   RXD LED 点灯

	if ( rcv_data[0] == 0x40 ) {	// ctsu 読み出しコマンド	
 		if ( rcv_cnt == 8 ) {   //  合計 8 byte 受信で、受信完了
		  set_receive_over();	// 受信完了処理
 		}
	}
	
	
}

//
// 受信完了処理
//
void set_receive_over(void)
{
	rcv_over = 1;
	
	RXD_LED_PORT_OUT = 0;	//   RXD LED 消灯
	
}


// コマンド受信の対する、コマンド処理とレスポンス作成処理
//
// 0x40 :CTSU読み出しコマンド
//

void comm_cmd(void)
{
      uint8_t fl_err;
      uint16_t receive_cnt;
      
      
     if ( rcv_data[0] == 0x40 ) {   // CTSU読み出しコマンドの場合
          receive_cnt = 8;
     }
  
     else {
     	   receive_cnt = 0;
     }
	
      rcv_crc_code = buf_crc_cal( &rcv_data[0], receive_cnt);  // 全受信データ(8byte)のCRC計算
      
   
   
      if ( rcv_crc_code != 0 ) {
           rcv_crc_err = 1;		// CRCエラー
      }
      else{
      	    rcv_crc_err = 0;
      }
      
           
     if ( rcv_data[0] == 0x40 ) {   // 静電容量センサ ctsu 読み出しコマンド
       
	    resp_ctsu_read();       // CTSU 読み出しコマンドのレスポンス作成
      }
     
	
	send_pt = 0;			// 送信するデータの位置クリア
	
	SDR00 = sd_data[send_pt];	// 最初のデータの送信
	send_pt++;			// 送信データの位置をインクリメント
	
	STMK0 = 0;		// UART0 送信の転送完了、バッファ空き割り込みの許可 (割り込みマスククリア)
	
}




//
// CTSU 読み出しコマンドのレスポンス作成
// 受信データ: 
//  rcv_data[ ]:
//           0:　0x40 (読み出しコマンド)
//           1: dummy 0
//           2: dummy 0 
//           3: dummy 0
//           4: dummy 0 
//           5: dummy 0
//           6: CRC 上位バイト
//           7: CRC 下位バイト
//
//送信データ :
// sd_data[ ]:
//         0:コマンドに対するレスポンス(0xc0)
//         1:CTSU ステータスレジスタ（CTSUST）
//         2:CTSU エラーステータスレジスタ（CTSUERRS）(下位バイト側)
//         3:	　　　　　　　　　　:　　　　　　　　 (上位バイト側)
//         4:TS06 センサカウンタ（CTSUSC）(下位バイト側)
//         5:	                   ：　　(上位バイト側)
//         6:TS06 リファレンスカウンタ（CTSURC)  (下位バイト側)      
//         7:	                   ：　　　　　　(上位バイト側)
//         8:TS06 高域ノイズ低減スペクトラム拡散制御レジスタ（CTSUSSC）(下位バイト側)      
//         9:	                   ：　　　　　　　　　　(上位バイト側)
//        10:TS06 センサオフセットレジスタ0（CTSUSO0)  (下位バイト側) 
//        11: 	                   ：　　　　　　　　　(上位バイト側)
//        12:TS06 センサオフセットレジスタ1（CTSUSO1)  (下位バイト側) 
//        13: 	                   ：　　　　　　　　　(上位バイト側)
//        14:TS08 センサカウンタ（CTSUSC）(下位バイト側)
//        15: 	                   ：　　 (上位バイト側)
//        16:TS08 リファレンスカウンタ（CTSURC)  (下位バイト側)      
//        17: 	                   ：　　　　　　(上位バイト側)
//        18:TS08 高域ノイズ低減スペクトラム拡散制御レジスタ（CTSUSSC）(下位バイト側)      
//        19: 	                   ：　　　　　　(上位バイト側)
//        20:TS08 センサオフセットレジスタ0（CTSUSO0)  (下位バイト側) 
//        21: 	                 ：　　　　　　　　　　(上位バイト側)
//        22:TS08 センサオフセットレジスタ1（CTSUSO1)  (下位バイト側) 
//        23: 	                 ：　　　　　　　　　　(上位バイト側)
//        24:TS10 センサカウンタ（CTSUSC）(下位バイト側)
//        25: 	    ：　　　　　　　　　　(上位バイト側)
//        26:TS10 リファレンスカウンタ（CTSURC)  (下位バイト側)      
//        27: 	           ：　　　　　　　　　　(上位バイト側)
//        28:TS10 高域ノイズ低減スペクトラム拡散制御レジスタ（CTSUSSC）(下位バイト側)      
//        29: 	                   ：　　　　　　(上位バイト側)
//        30:TS10 センサオフセットレジスタ0（CTSUSO0)  (下位バイト側) 
//        31: 	                 ：　　　　　　　　　　(上位バイト側)
//        32:TS10 センサオフセットレジスタ1（CTSUSO1)  (下位バイト側) 
//        33: 	                 ：　　　　　　　　　　(上位バイト側)
//        34:TS13 センサカウンタ（CTSUSC）(下位バイト側)
//        35: 	    ：　　　　　　　　　　(上位バイト側)
//        36:TS13 リファレンスカウンタ（CTSURC)  (下位バイト側)      
//        37: 	           ：　　　　　　　　　　(上位バイト側)
//        38:TS13 高域ノイズ低減スペクトラム拡散制御レジスタ（CTSUSSC）(下位バイト側)      
//        39: 	           ：　　　　　　　　　　(上位バイト側)
//        40:TS13 センサオフセットレジスタ0（CTSUSO0)  (下位バイト側) 
//        41: 	                 ：　　　　　　　　　　(上位バイト側)
//        42:TS13 センサオフセットレジスタ1（CTSUSO1)  (下位バイト側) 
//        43: 	                 ：　　　　　　　　　　(上位バイト側)
//        44: 	CRC 上位バイト
//        45: 	CRC 下位バイト
//
// 
//    P06: TS06 : SW4
//    P23: TS08 : SW3
//    P21: TS10 : SW2
//    P41: TS13 : SW1
//
//送信データ数: send_num = 46

void  resp_ctsu_read(void)
{
	uint8_t i;
	uint8_t pt;
	
	uint16_t crc;
	
	sd_data[0] = 0xc0;	 	// コマンドに対するレスポンス	
	sd_data[1] = CTSUST;		// CTSU ステータスレジスタ（CTSUST）
	sd_data[2] = CTSUERRS;		// CTSU エラーステータスレジスタ（CTSUERRS）(下位バイト側)		
	sd_data[3] = CTSUERRS >> 8;     //           :                              (上位バイト側)
	
	
	for ( i = 0;  i < CTSU_CH_NUM; i++ ) {
	   pt = i * 10 + 4;
		
	   sd_data[pt]     = ctsu_sc[i];        // センサカウンタ（CTSUSC）(下位バイト側)
	   sd_data[pt + 1] = ctsu_sc[i] >> 8;   //            :            (上位バイト側)
	
	   sd_data[pt + 2] = ctsu_rc[i];        // リファレンスカウンタ（CTSURC) (下位バイト側) 
	   sd_data[pt + 3] = ctsu_rc[i] >> 8;   //            :                  (上位バイト側)
	
	   sd_data[pt + 4] = ctsu_ssc[i];        // 高域ノイズ低減スペクトラム拡散制御レジスタ（CTSUSSC) (下位バイト側) 
	   sd_data[pt + 5] = ctsu_ssc[i] >> 8;   //            :                  (上位バイト側)
	
	   sd_data[pt + 6] = ctsu_so0[i];        // センサオフセットレジスタ0（CTSUSO0)  (下位バイト側) 
	   sd_data[pt + 7] = ctsu_so0[i] >> 8;   //            :		 (上位バイト側)	
	
	   sd_data[pt + 8] = ctsu_so1[i];        // センサオフセットレジスタ1（CTSUSO1)  (下位バイト側) 
	   sd_data[pt + 9] = ctsu_so1[i] >> 8;   //            :		 (上位バイト側)	
	}
	
	crc = buf_crc_cal( &sd_data[0], 44);  // 送信データ(44 byte)のCRC計算
	
	sd_data[44] = crc >> 8;		// CRC 上位バイト
	sd_data[45] = crc;		// CRC 下位バイト	
	
	send_num = 46;			// 送信データ数
	
}



// CRCの計算 
// buf[size]のCRCコードを作成
//
// 入力:
//    *buf: buf[size]先頭アドレス
//    size:データ数
// 
//  CRC-16 CCITT:
//  多項式: X^16 + X^12 + X^5 + 1　
//  初期値: 0xffff
//  MSBファースト
//  非反転出力
// 
// 参考:「CC-RX コンパイラ　ユーザズマニュアル」( R20UT3248JJ0113 Rev.1.13 ) 
//        p251 のコードが元
//        

uint16_t buf_crc_cal( volatile uint8_t *buf, uint16_t size)
{
    uint16_t crc;

    uint16_t i;

    crc = 0xffff;

    for (i = 0; i < size; i++)
    {
        crc = (uint16_t)((crc >> 8) | ((uint16_t)((uint32_t)crc << 8)));

        crc = (uint16_t)(crc ^ buf[i]);
        crc = (uint16_t)(crc ^ (uint16_t)((crc & 0xff) >> 4));
        crc = (uint16_t)(crc ^ (uint16_t)((crc << 8) << 4));
        crc = (uint16_t)(crc ^ (((crc & 0xff) << 4) << 1));
    }

    return crc;
}



//
//  UART0 の初期化 ( 8bit,1Stop,パリティ無し )
//
//  シリアル・アレイ・ユニット0のチャンネル2(UART1送信 TXD1),チャンネル3(UART1受信 RXD1)の設定
//
//  シリアル・アレイ・ユニット0 :チャンネル0 : UART0 送信(TXD0)(P00) (パソコンモニタソフト用)　
//　　　　　　　　　　　　　　　:チャンネル1 : UART0 受信(RXD0)(P01) (パソコンモニタソフト用)　
//　				:チャンネル2 : UART1 送信(TXD1)(P04) (未使用)　
//				:チャンネル3 : UART1 受信(RXD1)(P03) (未使用)
//
// シリアル・モード・レジスタ(SMRmn):  (m = Unit, n = ch)
//         b15   b14  b13 b12 b11 b10  b9  b8    b7  b6   b5  b4 b3  b2    b1    b0
// SMRmn: CKSmn CCSmn  0   0    0   0   0  STSmn  0  SISmn 1  0  0  MDmn2 MDmn1  MDmn0 
//
// シリアル通信動作設定レジスタ（SCRmn）
//         b15   b14  b13   b12    b11  b10     b9       b8       b7   b6   b5     b4      b3   b2    b1     b0
// SCRmn: TXEmn RXEmn DAPmm CKPmn   0   EOCmn  PTCmn1  PTCmn0   DIRmn  0   SLCmn1  SLCmn0  0    1   DLSmn1 DLSmn0 
//
// シリアル・データ・レジスタmn（SDRmn）
//         b15   b14   b13    b12   b11   b10     b9     b8    b7   b6   b5     b4      b3   b2    b1     b0
// SDRmn: <-ボーレート用(fMCK の分周設定レジスタ)-->     0    <---        送受信バッファ・レジスタ     --->
//
//・ボーレート = fMCK / ( 2 * (SDR[15:9] + 1) ) 
//
// 例1: ボーレート 1[Mbps],  fMCK(UART動作クロック) = 16[MHz]
// ボーレート
//    = 16[MHz] / ( 2 * ( 7 + 1 ) )
//    = 16000000 / 16
//    = 1 [Mbps]
//   SDR[15:9] = 7= 0x07 = 0000 0111 
// 
// SDRへの設定は、0000 1110 0000 0000 (=0x0e00)
//
// 例2: ボーレート 2[Mbps],  fMCK(UART動作クロック) = 16[MHz]
// ボーレート
//    = 16[MHz] / ( 2 * ( 3 + 1 ) )
//    = 16000000 / 8
//    = 2 [Mbps]
//   SDR[15:9] = 3= 0x03 = 0000 0011 
// 
// SDRへの設定は、0000 0110 0000 0000 (=0x0600)
//
//
// 例3: ボーレート 4[Mbps],  fMCK(UART動作クロック) = 16[MHz]
// ボーレート
//    = 16[MHz] / ( 2 * ( 1 + 1 ) )
//    = 16000000 / 4
//    = 4 [Mbps]
//   SDR[15:9] = 1= 0x01 = 0000 0001 
//
// ***シリアル・データ・レジスタmn（SDRmn）SDRmn[15:9]＝（0000000B, 0000001B）は設定禁止です。***
// (「13.6.3 ボー・レートの算出」より )
//
//
// 例5: ボーレート 38.4[Kbps],  fMCK(UART動作クロック) = 8[MHz]
// ボーレート
//    = 8[MHz] / ( 2 * ( 103 + 1 ) )
//    = 8000000 / 208
//    = 38462 [bps]
//   SDR[15:9] = 103 = 0x67 = 0110 0111 
// 
// SDRへの設定は、1100 1110 0000 0000 (=0xce00)
//
void uart_0_ini(void)
{
	SAU0EN = 1;		// シリアル・アレイ・ユニット0 へのクロック供給　(周辺イネーブル・レジスタ0（PER0）のbit2=1)

	SPS0 = 0x0000;		// シリアル・アレイ・ユニット0 動作クロック(fMCK) 設定 ( CK00=16[MHz],CK01=16[MHz]  at fclk=16[MHz] )
			
				
	SMR00 = 0x0023;         // UART0 送信側(TXD0) = (シリアル・アレイ・ユニット0: チャンネル0 )の設定
				// CKS00 = 0 : fMCK=CK00
				// CCS00 = 0 : 転送クロックは、fMCKの分周クロック。分周はSDR00[15:9]で指定
				// STS00 = 0 : スタートトリガ要因 = ソフトウェアトリガ(UART送信)
				// SIS00 = 0 : UART受信データのレベル反転 = 無し
				// MD002 = 0, MD001 = 1 :UARTモード
				// MD000 = 1 : チャンネル2の割り込み要因 = バッファ空き割り込み
				
	SCR00 = 0x8097;         // UART0 送信(TXD0) 通信フォーマットの設定
				//  8bit,1-Stop,パリティ無し
				//  TXE00=1, RXE00=0 :	送信のみ行う
				//  DAP00=0, CKP00=0 : UARTモード時の設定
				//  EOC00=0 :UART 送信時の設定
				//  PTC001=0,PTC000=0 :パリティなし
				//  DIR00=1 :UARTモードでのデータ転送順番=LSBファースト
				//  SLC001=0,SLC000=1 : 1 Stop ビット
				//  DLS001=1, DLS000=1 : 8 bitデータ
	
	SDR00 =	0x0e00;		// ボーレート設定 1[Mbps]

	
	SOL0 = 0x0000;		// 送信データ レベル反転なし
	SO0 =  0x0f0f;		// 初期出力レベル High
	SOE0 = 0x0001;		// ch0の出力許可
	
			
	SMR01 = 0x0122;         // UART0 受信側(RXD0) = (シリアル・アレイ・ユニット0: チャンネル1 )の設定
				// CKS01 = 0 : fMCK=CK00
				// CCS01 = 0 : 転送クロックは、fMCKの分周クロック。分周はSDR01[15:9]で指定
				// STS01 = 1 : スタートトリガ要因 = RxD1端子の有効エッジ(UART受信)
				// SIS01 = 0 : 立ち下がりエッジをスタート・ビットとして検出(UART受信データのレベル反転 = 無し)
				// MD012 = 0, MD011 = 1 :UARTモード
				// MD010 = 0 : チャンネル3の割り込み要因 = 転送完了割り込み
				
	SCR01 = 0x4097;         // UART1 受信信(RXD1) 通信フォーマットの設定
				//  8bit,1-Stop,パリティ無し
				//  TXE01=1, RXE01=1 :	受信のみ行う
				//  DAP01=0, CKP01=0 : UARTモード時の設定
				//  EOC01=0 :エラー割り込みINTSREx の発生を禁止する（INTSRx が発生する）
				//  PTC011=0,PTC010=0 :パリティなし
				//  DIR01=1 :UARTモードでのデータ転送順番=LSBファースト
				//  SLC011=0,SLC010=1 : 1 Stop ビット
				//  DLS011=1, DLS010=1 : 8 bitデータ
	
	SDR01 =	0x0e00;		// ボーレート設定 1[Mbps]
	
	NFEN0 = 0x01;		// RXD0端子のノイズフィルタ ON
	
	
	p00_txd0_p01_rxd0();    //  P00 = TxD0, P01 = RxD0とする処理

	
	SRPR00 = 1;		// RXD0受信割り込みレベル = 1 (レベル0～3 :0が最高)
	SRPR10 = 0;
	SRMK0 = 0;		// RXD0受信割り込み許可 (割り込みマスククリア)
	
		
		
	STPR00 = 1;		// TXD0送信割り込みレベル = 1 (レベル0～3 :0が最高)
	STPR10 = 0;
	
	
	SS0 = SS0 | 0x0003;	// 送信 TXD0(ユニット0: チャンネル0)、受信 RXD0(ユニット0: チャンネル1)許可 
	
}




//  UART0 端子設定
//   P00 = TXD0, P01 = RxD0とする処理
//
// 1) P00 = TxD0
//   PIOR (周辺I/O リダイレクション・レジスタ):   PIOR2 bit0 = 0,  PIOR2 bit1 = 0
//   POM (ポート出力モード・レジスタ):  POM0 bit0 = 0  (通常出力モード CMOS出力)
//   PMC (ポート・モード・コントロール・レジスタ): x = don't care
//   PM (ポート・モード・レジスタ) : PM0 bit0= 0 (出力モード（出力バッファ・オン） )
//   P (ポート・レジスタ) : P0 bit0 = 1 (ハイ・レベルを出力)
//   TSSEL(タッチ端子機能選択レジスタ) : x = don't care
//
// 2) P01 = RxD0
//   PIOR (周辺I/O リダイレクション・レジスタ):  PIOR2 bit0 = 0,  PIOR2 bit1 = 0
//   POM (ポート出力モード・レジスタ):  POM0 bit1 :x = don't care
//   PMC (ポート・モード・コントロール・レジスタ): PMC0 bit1 = 0 (デジタル入出力（アナログ入力以外の兼用機能）)
//   PM (ポート・モード・レジスタ) : PM0 bit1 = 1  (入力モード（出力バッファ・オフ）)
//   P (ポート・レジスタ) : P0 bit１ = 1 x = don't care
//   TSSEL(タッチ端子機能選択レジスタ) : TSSEL0 bit0 = 0 (タッチ端子機能以外（兼用機能）として使用)
//
//   資料: 「RL78/G16 ユーザーズマニュアル ハードウェア編」( R01UH0980JJ0110 Rev.1.10 )
//        「4.5.3 使用するポート機能および兼用機能のレジスタ設定例」より
//

void p00_txd0_p01_rxd0(void)
{

	PIOR2 = PIOR2 & 0x0c;   // P00=TxD0、P01=RxD0
	
	POM0_bit.no0 = 0;       // P00は、通常出力モード (CMOS出力)能）
	PM0_bit.no0 = 0;	// P00は、出力モード（出力バッファ・オン)
	P0_bit.no0 = 1;		// 1 (ハイ・レベルを出力)
	
	
	PMC0_bit.no1 = 0;	// P01は、デジタル入出力（アナログ入力以外の兼用機能）
	PM0_bit.no1 = 1;	// P01は、入力モード（出力バッファ・オフ）
	TSSEL0_bit.no0 = 0;	// P01は、 タッチ端子機能以外（兼用機能）として使用

	
}
