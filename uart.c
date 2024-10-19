
#include "iodefine.h"
#include "misratypes.h"
#include "ctsu.h"

#include "uart.h"
#include "led_port.h"


// ��M�p
volatile uint8_t  rcv_data[64];
volatile uint8_t rcv_over;
volatile uint8_t  rcv_cnt;

volatile uint8_t  rcv_crc_err;
volatile uint16_t rcv_status;

uint16_t rcv_crc_code;

// ���M�p
volatile uint8_t sd_data[64];
volatile uint8_t  send_pt;

volatile uint8_t  send_num;



//  UART0 ���M�o�b�t�@�󂫊��荞�ݐ擪�A�h���X���AINTST0(0x0012)�֓o�^
#pragma interrupt INT_uart0_send(vect=INTST0)

// UART0 ���M���荞�ݏ���
void INT_uart0_send(void)
{
	SDR00 = sd_data[send_pt];	// �f�[�^�̑��M
	
	send_pt++;		// ���M�f�[�^�̈ʒu���C���N�������g
	
	TXD_LED_PORT_OUT = 1;   	//  TXD LED �_��
	
	if ( send_pt == send_num ) { // �ŏI�f�[�^�̑��M����
	   	STMK0 = 1;		// UART0 ���M�̓]�������A�o�b�t�@�󂫊��荞�݂̋֎~ (���荞�݃}�X�N�Z�b�g)
		TXD_LED_PORT_OUT = 0;   	//  TXD LED ����
	}
}



// UART0 ��M���荞�ݏ����擪�A�h���X���AINTSR0(0x0014)�֓o�^
#pragma interrupt INT_uart0_rcv(vect=INTSR0)

// UART0 ��M���荞�ݏ���
void INT_uart0_rcv(void)
{
	rcv_data[rcv_cnt] = SDR01;	// ��M�f�[�^�̓ǂݏo��
	
	rcv_status = SSR01;	// �V���A���X�e�[�^�X�̓ǂݏo��
	
	rcv_cnt++;
	
	RXD_LED_PORT_OUT = 1;	//   RXD LED �_��

	if ( rcv_data[0] == 0x40 ) {	// ctsu �ǂݏo���R�}���h	
 		if ( rcv_cnt == 8 ) {   //  ���v 8 byte ��M�ŁA��M����
		  set_receive_over();	// ��M��������
 		}
	}
	
	
}

//
// ��M��������
//
void set_receive_over(void)
{
	rcv_over = 1;
	
	RXD_LED_PORT_OUT = 0;	//   RXD LED ����
	
}


// �R�}���h��M�̑΂���A�R�}���h�����ƃ��X�|���X�쐬����
//
// 0x40 :CTSU�ǂݏo���R�}���h
//

void comm_cmd(void)
{
      uint8_t fl_err;
      uint16_t receive_cnt;
      
      
     if ( rcv_data[0] == 0x40 ) {   // CTSU�ǂݏo���R�}���h�̏ꍇ
          receive_cnt = 8;
     }
  
     else {
     	   receive_cnt = 0;
     }
	
      rcv_crc_code = buf_crc_cal( &rcv_data[0], receive_cnt);  // �S��M�f�[�^(8byte)��CRC�v�Z
      
   
   
      if ( rcv_crc_code != 0 ) {
           rcv_crc_err = 1;		// CRC�G���[
      }
      else{
      	    rcv_crc_err = 0;
      }
      
           
     if ( rcv_data[0] == 0x40 ) {   // �Ód�e�ʃZ���T ctsu �ǂݏo���R�}���h
       
	    resp_ctsu_read();       // CTSU �ǂݏo���R�}���h�̃��X�|���X�쐬
      }
     
	
	send_pt = 0;			// ���M����f�[�^�̈ʒu�N���A
	
	SDR00 = sd_data[send_pt];	// �ŏ��̃f�[�^�̑��M
	send_pt++;			// ���M�f�[�^�̈ʒu���C���N�������g
	
	STMK0 = 0;		// UART0 ���M�̓]�������A�o�b�t�@�󂫊��荞�݂̋��� (���荞�݃}�X�N�N���A)
	
}




//
// CTSU �ǂݏo���R�}���h�̃��X�|���X�쐬
// ��M�f�[�^: 
//  rcv_data[ ]:
//           0:�@0x40 (�ǂݏo���R�}���h)
//           1: dummy 0
//           2: dummy 0 
//           3: dummy 0
//           4: dummy 0 
//           5: dummy 0
//           6: CRC ��ʃo�C�g
//           7: CRC ���ʃo�C�g
//
//���M�f�[�^ :
// sd_data[ ]:
//         0:�R�}���h�ɑ΂��郌�X�|���X(0xc0)
//         1:CTSU �X�e�[�^�X���W�X�^�iCTSUST�j
//         2:CTSU �G���[�X�e�[�^�X���W�X�^�iCTSUERRS�j(���ʃo�C�g��)
//         3:	�@�@�@�@�@�@�@�@�@�@:�@�@�@�@�@�@�@�@ (��ʃo�C�g��)
//         4:TS06 �Z���T�J�E���^�iCTSUSC�j(���ʃo�C�g��)
//         5:	                   �F�@�@(��ʃo�C�g��)
//         6:TS06 ���t�@�����X�J�E���^�iCTSURC)  (���ʃo�C�g��)      
//         7:	                   �F�@�@�@�@�@�@(��ʃo�C�g��)
//         8:TS06 ����m�C�Y�ጸ�X�y�N�g�����g�U���䃌�W�X�^�iCTSUSSC�j(���ʃo�C�g��)      
//         9:	                   �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        10:TS06 �Z���T�I�t�Z�b�g���W�X�^0�iCTSUSO0)  (���ʃo�C�g��) 
//        11: 	                   �F�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        12:TS06 �Z���T�I�t�Z�b�g���W�X�^1�iCTSUSO1)  (���ʃo�C�g��) 
//        13: 	                   �F�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        14:TS08 �Z���T�J�E���^�iCTSUSC�j(���ʃo�C�g��)
//        15: 	                   �F�@�@ (��ʃo�C�g��)
//        16:TS08 ���t�@�����X�J�E���^�iCTSURC)  (���ʃo�C�g��)      
//        17: 	                   �F�@�@�@�@�@�@(��ʃo�C�g��)
//        18:TS08 ����m�C�Y�ጸ�X�y�N�g�����g�U���䃌�W�X�^�iCTSUSSC�j(���ʃo�C�g��)      
//        19: 	                   �F�@�@�@�@�@�@(��ʃo�C�g��)
//        20:TS08 �Z���T�I�t�Z�b�g���W�X�^0�iCTSUSO0)  (���ʃo�C�g��) 
//        21: 	                 �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        22:TS08 �Z���T�I�t�Z�b�g���W�X�^1�iCTSUSO1)  (���ʃo�C�g��) 
//        23: 	                 �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        24:TS10 �Z���T�J�E���^�iCTSUSC�j(���ʃo�C�g��)
//        25: 	    �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        26:TS10 ���t�@�����X�J�E���^�iCTSURC)  (���ʃo�C�g��)      
//        27: 	           �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        28:TS10 ����m�C�Y�ጸ�X�y�N�g�����g�U���䃌�W�X�^�iCTSUSSC�j(���ʃo�C�g��)      
//        29: 	                   �F�@�@�@�@�@�@(��ʃo�C�g��)
//        30:TS10 �Z���T�I�t�Z�b�g���W�X�^0�iCTSUSO0)  (���ʃo�C�g��) 
//        31: 	                 �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        32:TS10 �Z���T�I�t�Z�b�g���W�X�^1�iCTSUSO1)  (���ʃo�C�g��) 
//        33: 	                 �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        34:TS13 �Z���T�J�E���^�iCTSUSC�j(���ʃo�C�g��)
//        35: 	    �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        36:TS13 ���t�@�����X�J�E���^�iCTSURC)  (���ʃo�C�g��)      
//        37: 	           �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        38:TS13 ����m�C�Y�ጸ�X�y�N�g�����g�U���䃌�W�X�^�iCTSUSSC�j(���ʃo�C�g��)      
//        39: 	           �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        40:TS13 �Z���T�I�t�Z�b�g���W�X�^0�iCTSUSO0)  (���ʃo�C�g��) 
//        41: 	                 �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        42:TS13 �Z���T�I�t�Z�b�g���W�X�^1�iCTSUSO1)  (���ʃo�C�g��) 
//        43: 	                 �F�@�@�@�@�@�@�@�@�@�@(��ʃo�C�g��)
//        44: 	CRC ��ʃo�C�g
//        45: 	CRC ���ʃo�C�g
//
// 
//    P06: TS06 : SW4
//    P23: TS08 : SW3
//    P21: TS10 : SW2
//    P41: TS13 : SW1
//
//���M�f�[�^��: send_num = 46

void  resp_ctsu_read(void)
{
	uint8_t i;
	uint8_t pt;
	
	uint16_t crc;
	
	sd_data[0] = 0xc0;	 	// �R�}���h�ɑ΂��郌�X�|���X	
	sd_data[1] = CTSUST;		// CTSU �X�e�[�^�X���W�X�^�iCTSUST�j
	sd_data[2] = CTSUERRS;		// CTSU �G���[�X�e�[�^�X���W�X�^�iCTSUERRS�j(���ʃo�C�g��)		
	sd_data[3] = CTSUERRS >> 8;     //           :                              (��ʃo�C�g��)
	
	
	for ( i = 0;  i < CTSU_CH_NUM; i++ ) {
	   pt = i * 10 + 4;
		
	   sd_data[pt]     = ctsu_sc[i];        // �Z���T�J�E���^�iCTSUSC�j(���ʃo�C�g��)
	   sd_data[pt + 1] = ctsu_sc[i] >> 8;   //            :            (��ʃo�C�g��)
	
	   sd_data[pt + 2] = ctsu_rc[i];        // ���t�@�����X�J�E���^�iCTSURC) (���ʃo�C�g��) 
	   sd_data[pt + 3] = ctsu_rc[i] >> 8;   //            :                  (��ʃo�C�g��)
	
	   sd_data[pt + 4] = ctsu_ssc[i];        // ����m�C�Y�ጸ�X�y�N�g�����g�U���䃌�W�X�^�iCTSUSSC) (���ʃo�C�g��) 
	   sd_data[pt + 5] = ctsu_ssc[i] >> 8;   //            :                  (��ʃo�C�g��)
	
	   sd_data[pt + 6] = ctsu_so0[i];        // �Z���T�I�t�Z�b�g���W�X�^0�iCTSUSO0)  (���ʃo�C�g��) 
	   sd_data[pt + 7] = ctsu_so0[i] >> 8;   //            :		 (��ʃo�C�g��)	
	
	   sd_data[pt + 8] = ctsu_so1[i];        // �Z���T�I�t�Z�b�g���W�X�^1�iCTSUSO1)  (���ʃo�C�g��) 
	   sd_data[pt + 9] = ctsu_so1[i] >> 8;   //            :		 (��ʃo�C�g��)	
	}
	
	crc = buf_crc_cal( &sd_data[0], 44);  // ���M�f�[�^(44 byte)��CRC�v�Z
	
	sd_data[44] = crc >> 8;		// CRC ��ʃo�C�g
	sd_data[45] = crc;		// CRC ���ʃo�C�g	
	
	send_num = 46;			// ���M�f�[�^��
	
}



// CRC�̌v�Z 
// buf[size]��CRC�R�[�h���쐬
//
// ����:
//    *buf: buf[size]�擪�A�h���X
//    size:�f�[�^��
// 
//  CRC-16 CCITT:
//  ������: X^16 + X^12 + X^5 + 1�@
//  �����l: 0xffff
//  MSB�t�@�[�X�g
//  �񔽓]�o��
// 
// �Q�l:�uCC-RX �R���p�C���@���[�U�Y�}�j���A���v( R20UT3248JJ0113 Rev.1.13 ) 
//        p251 �̃R�[�h����
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
//  UART0 �̏����� ( 8bit,1Stop,�p���e�B���� )
//
//  �V���A���E�A���C�E���j�b�g0�̃`�����l��2(UART1���M TXD1),�`�����l��3(UART1��M RXD1)�̐ݒ�
//
//  �V���A���E�A���C�E���j�b�g0 :�`�����l��0 : UART0 ���M(TXD0)(P00) (�p�\�R�����j�^�\�t�g�p)�@
//�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@:�`�����l��1 : UART0 ��M(RXD0)(P01) (�p�\�R�����j�^�\�t�g�p)�@
//�@				:�`�����l��2 : UART1 ���M(TXD1)(P04) (���g�p)�@
//				:�`�����l��3 : UART1 ��M(RXD1)(P03) (���g�p)
//
// �V���A���E���[�h�E���W�X�^(SMRmn):  (m = Unit, n = ch)
//         b15   b14  b13 b12 b11 b10  b9  b8    b7  b6   b5  b4 b3  b2    b1    b0
// SMRmn: CKSmn CCSmn  0   0    0   0   0  STSmn  0  SISmn 1  0  0  MDmn2 MDmn1  MDmn0 
//
// �V���A���ʐM����ݒ背�W�X�^�iSCRmn�j
//         b15   b14  b13   b12    b11  b10     b9       b8       b7   b6   b5     b4      b3   b2    b1     b0
// SCRmn: TXEmn RXEmn DAPmm CKPmn   0   EOCmn  PTCmn1  PTCmn0   DIRmn  0   SLCmn1  SLCmn0  0    1   DLSmn1 DLSmn0 
//
// �V���A���E�f�[�^�E���W�X�^mn�iSDRmn�j
//         b15   b14   b13    b12   b11   b10     b9     b8    b7   b6   b5     b4      b3   b2    b1     b0
// SDRmn: <-�{�[���[�g�p(fMCK �̕����ݒ背�W�X�^)-->     0    <---        ����M�o�b�t�@�E���W�X�^     --->
//
//�E�{�[���[�g = fMCK / ( 2 * (SDR[15:9] + 1) ) 
//
// ��1: �{�[���[�g 1[Mbps],  fMCK(UART����N���b�N) = 16[MHz]
// �{�[���[�g
//    = 16[MHz] / ( 2 * ( 7 + 1 ) )
//    = 16000000 / 16
//    = 1 [Mbps]
//   SDR[15:9] = 7= 0x07 = 0000 0111 
// 
// SDR�ւ̐ݒ�́A0000 1110 0000 0000 (=0x0e00)
//
// ��2: �{�[���[�g 2[Mbps],  fMCK(UART����N���b�N) = 16[MHz]
// �{�[���[�g
//    = 16[MHz] / ( 2 * ( 3 + 1 ) )
//    = 16000000 / 8
//    = 2 [Mbps]
//   SDR[15:9] = 3= 0x03 = 0000 0011 
// 
// SDR�ւ̐ݒ�́A0000 0110 0000 0000 (=0x0600)
//
//
// ��3: �{�[���[�g 4[Mbps],  fMCK(UART����N���b�N) = 16[MHz]
// �{�[���[�g
//    = 16[MHz] / ( 2 * ( 1 + 1 ) )
//    = 16000000 / 4
//    = 4 [Mbps]
//   SDR[15:9] = 1= 0x01 = 0000 0001 
//
// ***�V���A���E�f�[�^�E���W�X�^mn�iSDRmn�jSDRmn[15:9]���i0000000B, 0000001B�j�͐ݒ�֎~�ł��B***
// (�u13.6.3 �{�[�E���[�g�̎Z�o�v��� )
//
//
// ��5: �{�[���[�g 38.4[Kbps],  fMCK(UART����N���b�N) = 8[MHz]
// �{�[���[�g
//    = 8[MHz] / ( 2 * ( 103 + 1 ) )
//    = 8000000 / 208
//    = 38462 [bps]
//   SDR[15:9] = 103 = 0x67 = 0110 0111 
// 
// SDR�ւ̐ݒ�́A1100 1110 0000 0000 (=0xce00)
//
void uart_0_ini(void)
{
	SAU0EN = 1;		// �V���A���E�A���C�E���j�b�g0 �ւ̃N���b�N�����@(���ӃC�l�[�u���E���W�X�^0�iPER0�j��bit2=1)

	SPS0 = 0x0000;		// �V���A���E�A���C�E���j�b�g0 ����N���b�N(fMCK) �ݒ� ( CK00=16[MHz],CK01=16[MHz]  at fclk=16[MHz] )
			
				
	SMR00 = 0x0023;         // UART0 ���M��(TXD0) = (�V���A���E�A���C�E���j�b�g0: �`�����l��0 )�̐ݒ�
				// CKS00 = 0 : fMCK=CK00
				// CCS00 = 0 : �]���N���b�N�́AfMCK�̕����N���b�N�B������SDR00[15:9]�Ŏw��
				// STS00 = 0 : �X�^�[�g�g���K�v�� = �\�t�g�E�F�A�g���K(UART���M)
				// SIS00 = 0 : UART��M�f�[�^�̃��x�����] = ����
				// MD002 = 0, MD001 = 1 :UART���[�h
				// MD000 = 1 : �`�����l��2�̊��荞�ݗv�� = �o�b�t�@�󂫊��荞��
				
	SCR00 = 0x8097;         // UART0 ���M(TXD0) �ʐM�t�H�[�}�b�g�̐ݒ�
				//  8bit,1-Stop,�p���e�B����
				//  TXE00=1, RXE00=0 :	���M�̂ݍs��
				//  DAP00=0, CKP00=0 : UART���[�h���̐ݒ�
				//  EOC00=0 :UART ���M���̐ݒ�
				//  PTC001=0,PTC000=0 :�p���e�B�Ȃ�
				//  DIR00=1 :UART���[�h�ł̃f�[�^�]������=LSB�t�@�[�X�g
				//  SLC001=0,SLC000=1 : 1 Stop �r�b�g
				//  DLS001=1, DLS000=1 : 8 bit�f�[�^
	
	SDR00 =	0x0e00;		// �{�[���[�g�ݒ� 1[Mbps]

	
	SOL0 = 0x0000;		// ���M�f�[�^ ���x�����]�Ȃ�
	SO0 =  0x0f0f;		// �����o�̓��x�� High
	SOE0 = 0x0001;		// ch0�̏o�͋���
	
			
	SMR01 = 0x0122;         // UART0 ��M��(RXD0) = (�V���A���E�A���C�E���j�b�g0: �`�����l��1 )�̐ݒ�
				// CKS01 = 0 : fMCK=CK00
				// CCS01 = 0 : �]���N���b�N�́AfMCK�̕����N���b�N�B������SDR01[15:9]�Ŏw��
				// STS01 = 1 : �X�^�[�g�g���K�v�� = RxD1�[�q�̗L���G�b�W(UART��M)
				// SIS01 = 0 : ����������G�b�W���X�^�[�g�E�r�b�g�Ƃ��Č��o(UART��M�f�[�^�̃��x�����] = ����)
				// MD012 = 0, MD011 = 1 :UART���[�h
				// MD010 = 0 : �`�����l��3�̊��荞�ݗv�� = �]���������荞��
				
	SCR01 = 0x4097;         // UART1 ��M�M(RXD1) �ʐM�t�H�[�}�b�g�̐ݒ�
				//  8bit,1-Stop,�p���e�B����
				//  TXE01=1, RXE01=1 :	��M�̂ݍs��
				//  DAP01=0, CKP01=0 : UART���[�h���̐ݒ�
				//  EOC01=0 :�G���[���荞��INTSREx �̔������֎~����iINTSRx ����������j
				//  PTC011=0,PTC010=0 :�p���e�B�Ȃ�
				//  DIR01=1 :UART���[�h�ł̃f�[�^�]������=LSB�t�@�[�X�g
				//  SLC011=0,SLC010=1 : 1 Stop �r�b�g
				//  DLS011=1, DLS010=1 : 8 bit�f�[�^
	
	SDR01 =	0x0e00;		// �{�[���[�g�ݒ� 1[Mbps]
	
	NFEN0 = 0x01;		// RXD0�[�q�̃m�C�Y�t�B���^ ON
	
	
	p00_txd0_p01_rxd0();    //  P00 = TxD0, P01 = RxD0�Ƃ��鏈��

	
	SRPR00 = 1;		// RXD0��M���荞�݃��x�� = 1 (���x��0�`3 :0���ō�)
	SRPR10 = 0;
	SRMK0 = 0;		// RXD0��M���荞�݋��� (���荞�݃}�X�N�N���A)
	
		
		
	STPR00 = 1;		// TXD0���M���荞�݃��x�� = 1 (���x��0�`3 :0���ō�)
	STPR10 = 0;
	
	
	SS0 = SS0 | 0x0003;	// ���M TXD0(���j�b�g0: �`�����l��0)�A��M RXD0(���j�b�g0: �`�����l��1)���� 
	
}




//  UART0 �[�q�ݒ�
//   P00 = TXD0, P01 = RxD0�Ƃ��鏈��
//
// 1) P00 = TxD0
//   PIOR (����I/O ���_�C���N�V�����E���W�X�^):   PIOR2 bit0 = 0,  PIOR2 bit1 = 0
//   POM (�|�[�g�o�̓��[�h�E���W�X�^):  POM0 bit0 = 0  (�ʏ�o�̓��[�h CMOS�o��)
//   PMC (�|�[�g�E���[�h�E�R���g���[���E���W�X�^): x = don't care
//   PM (�|�[�g�E���[�h�E���W�X�^) : PM0 bit0= 0 (�o�̓��[�h�i�o�̓o�b�t�@�E�I���j )
//   P (�|�[�g�E���W�X�^) : P0 bit0 = 1 (�n�C�E���x�����o��)
//   TSSEL(�^�b�`�[�q�@�\�I�����W�X�^) : x = don't care
//
// 2) P01 = RxD0
//   PIOR (����I/O ���_�C���N�V�����E���W�X�^):  PIOR2 bit0 = 0,  PIOR2 bit1 = 0
//   POM (�|�[�g�o�̓��[�h�E���W�X�^):  POM0 bit1 :x = don't care
//   PMC (�|�[�g�E���[�h�E�R���g���[���E���W�X�^): PMC0 bit1 = 0 (�f�W�^�����o�́i�A�i���O���͈ȊO�̌��p�@�\�j)
//   PM (�|�[�g�E���[�h�E���W�X�^) : PM0 bit1 = 1  (���̓��[�h�i�o�̓o�b�t�@�E�I�t�j)
//   P (�|�[�g�E���W�X�^) : P0 bit�P = 1 x = don't care
//   TSSEL(�^�b�`�[�q�@�\�I�����W�X�^) : TSSEL0 bit0 = 0 (�^�b�`�[�q�@�\�ȊO�i���p�@�\�j�Ƃ��Ďg�p)
//
//   ����: �uRL78/G16 ���[�U�[�Y�}�j���A�� �n�[�h�E�F�A�ҁv( R01UH0980JJ0110 Rev.1.10 )
//        �u4.5.3 �g�p����|�[�g�@�\����ь��p�@�\�̃��W�X�^�ݒ��v���
//

void p00_txd0_p01_rxd0(void)
{

	PIOR2 = PIOR2 & 0x0c;   // P00=TxD0�AP01=RxD0
	
	POM0_bit.no0 = 0;       // P00�́A�ʏ�o�̓��[�h (CMOS�o��)�\�j
	PM0_bit.no0 = 0;	// P00�́A�o�̓��[�h�i�o�̓o�b�t�@�E�I��)
	P0_bit.no0 = 1;		// 1 (�n�C�E���x�����o��)
	
	
	PMC0_bit.no1 = 0;	// P01�́A�f�W�^�����o�́i�A�i���O���͈ȊO�̌��p�@�\�j
	PM0_bit.no1 = 1;	// P01�́A���̓��[�h�i�o�̓o�b�t�@�E�I�t�j
	TSSEL0_bit.no0 = 0;	// P01�́A �^�b�`�[�q�@�\�ȊO�i���p�@�\�j�Ƃ��Ďg�p

	
}
