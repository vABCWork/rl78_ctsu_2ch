
//   P03 : RXD_LED
//   P04 : TXD_LED
//   P05 : ALM_LED

#define RXD_LED_PORT_PMC  PMC0_bit.no3 //  �|�[�g�E���[�h�E�R���g���[���E���W�X�^
				       // 0:�f�W�^�����o��, 1:�A�i���O���� (���Z�b�g�M���̔����ɂ��AFFH)
#define RXD_LED_PORT_PM  PM0_bit.no3   //  �|�[�g�E���[�h�E���W�X�^
 				       // 0:�o�̓��[�h(�o�̓o�b�t�@ON), 1:���̓��[�h(�o�̓o�b�t�@OFF) (���Z�b�g�M���̔����ɂ��AFFH)
#define RXD_LED_PORT_OUT P0_bit.no3    //  �o�̓f�[�^



#define TXD_LED_PORT_PMC  PMC0_bit.no4  //  �|�[�g�E���[�h�E�R���g���[���E���W�X�^
					// 0:�f�W�^�����o��, 1:�A�i���O���� (���Z�b�g�M���̔����ɂ��AFFH)
#define TXD_LED_PORT_PM  PM0_bit.no4   //  �|�[�g�E���[�h�E���W�X�^
				       // 0:�o�̓��[�h(�o�̓o�b�t�@ON), 1:���̓��[�h(�o�̓o�b�t�@OFF) (���Z�b�g�M���̔����ɂ��AFFH)
#define TXD_LED_PORT_OUT P0_bit.no4    //  �o�̓f�[�^




#define ALM_LED_PORT_POM  POM0_bit.no5  // �|�[�g�o�̓��[�h�E���W�X�^
					// 0:�ʏ�o�̓��[�h, 1:N-ch �I�[�v���E�h���C���o��
#define ALM_LED_PORT_PMC  PMC0_bit.no5  //  �|�[�g�E���[�h�E�R���g���[���E���W�X�^
					// 0:�f�W�^�����o��, 1:�A�i���O���� (���Z�b�g�M���̔����ɂ��AFFH)					
					
#define ALM_LED_PORT_PM  PM0_bit.no5   //  �|�[�g�E���[�h�E���W�X�^
				       // 0:�o�̓��[�h(�o�̓o�b�t�@ON), 1:���̓��[�h(�o�̓o�b�t�@OFF) (���Z�b�g�M���̔����ɂ��AFFH)
#define ALM_LED_PORT_OUT P0_bit.no5    //  �o�̓f�[�^






void uart_0_led_port(void);

void alarm_led_port(void);
