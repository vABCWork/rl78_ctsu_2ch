
#include   "iodefine.h"
#include   "misratypes.h"

#include "ctsu.h"
#include "key.h"

// �X�C�b�`���͊֌W�̍\����
//  
struct SW_info
{
	uint8_t status;		// ����� �^�b�`�L��(Low=0),��^�b�`(High=1)��� (10msec��)

	uint8_t pre_status;	// �O��(10msec�O)�� �^�b�`�L��(Low=0),��^�b�`(High=1)��� 
	
	uint8_t low_cnt;	// �^�b�`�L��̉�

	uint8_t one_push;	// 0:�L�[���͏����v���Ȃ� 1:�L�[���͏����v��(1�x����)�@�i�L�[���͏������0�N���A�j
	
	uint8_t edge_flg;	// 1: �����茟�o�ς�
	
	uint16_t threshold;	// ON�Ɣ��肷�� CTSU�̃J�E���g�l 

};


volatile struct  SW_info  Key_sw[CTSU_CH_NUM];	// �X�C�b�`�����̏��i�[�̈�


//
//   �X�e�b�s���O���[�^�ւ̃p���X�o��
//
// ctsu_sc[0] : TS09:  Key_sw[0]: ���v���   
// ctsu_sc[1] : TS10:  Key_sw[1]: �����v��� 
//

void motor_pulse_out(void)
{
	if ( Key_sw[0].one_push == 1 ) {	// ���v��� ON

	     TOE0L_bit.no5 = 1;     		// �^�C�}�o�͋����W�X�^  ch5(TO05) �o�͋��@(�X�e�b�s���O���[�^ �p���X�o��)
	 
	     P12_bit.no1 = 0;      		// P121 = 0���o�́@(�X�e�b�s���O���[�^�@��������)
	
	}

	else if ( Key_sw[1].one_push == 1 ) {	// �����v��� ON
	  
             TOE0L_bit.no5 = 1;     		// �^�C�}�o�͋����W�X�^  ch5(TO05) �o�͋��@(�X�e�b�s���O���[�^ �p���X�o��)
		
	      P12_bit.no1 = 1;       		// P121 = 1���o�́@(�X�e�b�s���O���[�^�@��������)
		
	}
	
	else {
	
            TOE0L_bit.no5 = 0;     		// �^�C�}�o�͋����W�X�^  ch5(TO05) �o�͋֎~�@(�X�e�b�s���O���[�^ �p���X�o�͂Ȃ�) 
          
	    TO0L = TO0L & 0xdf;			// �`�����l��4 �^�C�}�o�� = 0
	 
	    P12_bit.no1 = 0;       		// P121 = 0���o�́@(�X�e�b�s���O���[�^�@��������)
	
	}

}


//
//  �e�L�[�̃^�b�`�L�蔻��l�̃Z�b�g
// 
void touch_key_on_val(void)
{
	uint8_t i;
	
	for( i = 0; i < CTSU_CH_NUM ; i++ ) {
	
		Key_sw[i].threshold = TOUCH_ON_VALUE;
	}
	
}

//
//   �e�L�[�̃^�b�`�L�蔻��l�Ɣ�r���āA�^�b�`�̗L���𔻒肷��B
//
// ctsu_sc[0] : TS09:  Key_sw[0]: ���v���  
// ctsu_sc[1] : TS10:  Key_sw[1]: �����v��� 
// ctsu_sc[2] : TS11:  key_se[2]: ���g�p                                                 
// ctsu_sc[3] : TS13:  key_sw[3]: ���g�p
//
void touch_key_status_check(void)
{
	uint8_t i;
	
	for( i = 0; i < CTSU_CH_NUM ; i++ ) {
	
	  if ( ctsu_sc[i] > Key_sw[i].threshold ) {  //  �^�b�`�L��(ON)����l���傫���ꍇ�A
	  
	  	Key_sw[i].status = 0;		// �^�b�`�L�� (Low=0)
	  }
	  else {
	  	Key_sw[i].status = 1;		// �^�b�`���� (High=1)
	  }
	  
	}
}


//
//  ���v���(TS09)�A�����v���(TS10 ) �p �L�[���͂̊m�菈��  (10msec���A�^�C�}���荞�ݓ��Ŏ��s)
//    �^�b�`�L�蔻��Ȃ�΁ASW�I���m��t���O���Z�b�g����B
//
//  ����: id = 0 �` 1
//
// ctsu_sc[0] : TS09:  Key_sw[0]: ���v���   
// ctsu_sc[1] : TS10:  Key_sw[1]: �����v���
//

void key_input_check( uint8_t id ) 
{
	if ( Key_sw[id].status == 0 ) {     // ���� �^�b�`���
	    
	      Key_sw[id].one_push = 1;	    // SW�I���m��t���O���Z�b�g
	}
	else {				    // ���� �^�b�`����
	      
	       Key_sw[id].one_push = 0;	    // SW�I���m��t���O���N���A
	}
}

