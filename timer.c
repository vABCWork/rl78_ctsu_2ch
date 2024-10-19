
#include "iodefine.h"
#include "misratypes.h"

#include "key.h"
#include "timer.h"


// �^�C�}�A���C���j�b�g�̏�����
//  ���̓N���b�N�����ƕ�����ݒ�
//    CK00 = 500[KHz]  
//           16MHz/ 2^5 = 16000 KHz/32 = 500 [KHz]
//
//    CK01 = 1.95[KHz]
//            16MHz/2^13 = 16000 KHz/8192 = 1.953 [KHz]
//

void timer_array_ini(void)
{

	TAU0EN = 1;		// �^�C�}�A���C���j�b�g�ւ̃N���b�N���� , ���ӃC�l�[�u���E���W�X�^0�iPER0�j��bit0
	
	TPS0 = 0x00d5;		// CK01 = 1.95[KHz], CK00 =  500[KHz]
}
				




//
// �^�C�}�A���C���j�b�g �`�����l��5�̏�����
//  �J�E���g�p�N���b�N CK00 = 500[KHz]
// 
//  TO05 ����̏o�͕��`�g�̎���= �J�E���g�E�N���b�N�̎��� x ( TDR04 �̐ݒ�l + 1) x 2
//    4[msec] = 0.002[msec] x ( TDR04 �̐ݒ�l + 1) x 2
//    ( TDR04 �̐ݒ�l + 1 ) =  4 / 0.004

void timer_ch5_ini(void)
{
				
	TMR05 = 0x0000;		// ����N���b�N=CK00,�X�^�[�g�g���K:�\�t�g�E�F�A
				// ���샂�[�h:�C���^�[�o���^�C�}���[�h, ���`�g�o�� (�J�E���g�J�n���Ƀ^�C�}���荞�݂𔭐����Ȃ�)
	
				// 2msec�̃C���^�[�o���^�C�}�p�̃^�C�}�f�[�^���W�X�^�l�i�o�͕��`�g�̎����́A4[msec])
	TDR05 = 1000 - 1;	// �^�C�}�p�N���b�N=500[KHz], 1 count = 0.002[msec],   0.002 * �^�C�}�f�[�^���W�X�^�l�@= 2msec

	
	TOM0L = TOM0L & 0xdf;	// �`�����l��5 : �}�X�^�E�`���l���o�̓��[�h�i�^�C�}���荞�ݗv���M���iINTTMmn�j�ɂ��g�O���o�͂��s���j
	
	TO0L = TO0L & 0xdf;	// �`�����l��5 :�^�C�}�o�͂̃o�b�t�@�E���W�X�^ (�����o�̓��x��) = Low
	
	TOE0L_bit.no5 = 0;     // �^�C�}�o�͋����W�X�^  ch5(TO05) �o�͋֎~�@
	
	p122_to05();		// P122 = TO05(ch5�^�C�}�o��) �Ƃ��鏈��
	
	
	TS0L_bit.no5 = 1;	// �`�����l��5�J�E���g�J�n
				
}





// 
//   P122 = TO05(ch5�^�C�}�o��) �Ƃ��鏈��
//
//   PIOR (����I/O ���_�C���N�V�����E���W�X�^):  PIOR1 bit5 = 0
//   POM (�|�[�g�o�̓��[�h�E���W�X�^):  don't care
//   PMC (�|�[�g�E���[�h�E�R���g���[���E���W�X�^):�@don't care
//   PM (�|�[�g�E���[�h�E���W�X�^) : PM12 bit2= 0 ( �o�̓��[�h�i�o�̓o�b�t�@�E�I���j)
//   P (�|�[�g�E���W�X�^) : P12 bit2 = 0 (0���o��)
//   TSSEL(�^�b�`�[�q�@�\�I�����W�X�^) : don't care
//
//   ����: �uRL78/G16 ���[�U�[�Y�}�j���A�� �n�[�h�E�F�A�ҁv( R01UH0980JJ0110 Rev.1.10 )
//        �u4.5.3 �g�p����|�[�g�@�\����ь��p�@�\�̃��W�X�^�ݒ��v���
//

void p122_to05(void)
{

	PIOR1 = PIOR1 & 0xdf;   // PIOR1 bit5 = 0,  1101 1111 (0xdf)
	
	PM12_bit.no2 = 0;	// P122�́A�o�̓��[�h�i�o�̓o�b�t�@�E�I���j
	P12_bit.no2 = 0;	// 0���o��
	
}



//
//  P121���o�͂Ƃ��鏈��
//�@P121:�@�X�e�b�s���O���[�^�̉�]��������p

void p121_out_mode(void)
{
	
	PM12_bit.no1 = 0;	// P121�́A�o�̓��[�h
	
	P12_bit.no1 = 0;	// 0���o��
}


