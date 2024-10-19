
extern volatile uint8_t rcv_over;
extern volatile uint8_t  rcv_cnt;


void comm_cmd(void);

void set_receive_over(void);


uint16_t buf_crc_cal(volatile uint8_t *buf, uint16_t size);

void store_prog_data(void);

void  resp_prog_write(uint8_t err_flg);

void  resp_prog_read(void);

void  resp_ctsu_read(void);


void uart_0_ini(void);
void p00_txd0_p01_rxd0(void);

