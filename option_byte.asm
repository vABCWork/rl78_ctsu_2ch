; 
;  オプション・バイト (0x000C0〜0x000C2)の設定
;
;  リンク・オプション(デバイス)のユーザオプションバイトの設定がされている場合、
;  リンク時の設定データが優先される。 
;
;
.SECTION .option_byte, OPT_BYTE

				;000C0H 
;	.DB	0xEA		; ウオッチドックタイマ カウント停止
				; HALT/STOPモード時、カウント停止

	.DB	0xF9		; ウオッチドックタイマ カウント許可, 
				; オーバフロー時間= 118 [ms]
				; HALT/STOPモード時、カウンタ動作許可	


				;000C1H
        .DB	0xFF		; セレクタブルパワーオンリセット(SPOR)検出電圧
                                ;  立ち上がり(リセット解除)電圧 = 2.16[V]
				;  立下り(リセット発生)電圧     = 2.11[V]
				; P125 = RESET端子として使用

				;000C2H
	.DB	0xF9		; 高速オンチップオシレータ動作周波数 = 16[MHz]	

				;000C3H
        .DB     0x85		; オンチップデバックの動作許可
	
	