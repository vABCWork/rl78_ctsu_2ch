
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
// ���C������
// 
void main(void)
{
	
	uart_0_ini();		// UART0 �ʐM�̏�����
	
	interval_timer_12bit_ini();	// 12bit �C���^�[�o���^�C�}�̋N��
	
	ctsu_self_set();	//  �Ód�e�ʎ��^�b�`�Z���T(CTSU)�̐ݒ�

	timer_array_ini();	//  ���̓N���b�N�����ƕ�����ݒ�
	timer_ch5_ini();        // �X�e�b�s���O���[�^ �p���X�ݒ�
	
	p121_out_mode();		// �X�e�b�s���O���[�^ ��]�����ݒ�

	uart_0_led_port();      // TXD0,RXD0�p LED�|�[�g�ݒ�
	alarm_led_port();	// �A���[���p LED�|�[�g�ݒ�
	
	touch_key_on_val();	//   �e�L�[�̃^�b�`�L�蔻��l�̃Z�b�g
	
	__EI();			// ���荞�݋���
	
	CTSUSTRT = 1; 		 // CTSU�v������J�n
	
	while(1)
	{
	  WDTE = 0xac;		// �E�I�b�`�h�b�N�@�J�E���^�̃N���A
		
		  
	  if ( rcv_over == 1 ) {	// �R�}���h��M����
		comm_cmd();		// ���X�|���X�쐬�A���M
	   	rcv_over = 0;		// �R�}���h��M�t���O�̃N���A
		rcv_cnt  = 0;		//  ��M�o�C�g���̏���
		
	   }
	
	}
	
}


//
// ���g�p�Ȋ��荞�݂ɁA�o�^�����֐�
//  �u�����N�E�I�v�V�������o�̓R�[�h���x�N�^�E�e�[�u���󂫗̈�̃A�h���X�v�� _Dummy �Ƃ���B
//
//�@( �R���p�C����ARETI(61 FC))�@
//
#pragma interrupt Dummy
void Dummy(void)
{
}




