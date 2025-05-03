#include "w5500_spi.h"

SPI_HandleTypeDef *spi_handle = &hspi1;


void mcu_cris_enter(void) {
	__set_PRIMASK(1);
}

void mcu_cris_exit(void) {
	__set_PRIMASK(0);
}

void wizchip_select(void) {
	  HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_RESET);
}

void wizchip_deselect(void) {
	  HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET);
}

uint8_t wizchip_read() {
	uint8_t rbuf;
	HAL_SPI_Receive(spi_handle, &rbuf, 1, 100);
	return rbuf;
}

void wizchip_write(uint8_t wb) {
	HAL_SPI_Transmit(spi_handle, &wb, 1, 100);
}

void wizchip_readburst(uint8_t* pBuf, uint16_t len) {
	for (uint16_t i=0; i<len; i++) {
		*pBuf = wizchip_read();
		pBuf++;
	}
}

void wizchip_writeburst(uint8_t* pBuf, uint16_t len) {
	for (uint16_t i=0; i<len; i++){
		wizchip_write(*pBuf);
		pBuf++;
	}
}


void W5500Init() {
	uint8_t tmp;
	// initialize the buffers, 2kb for each socket
	uint8_t memsize[2][8] = {{4,2,2,2, 2,2,1,1}, {4,2,2,2, 2,2,1,1}};

	// CS high by default, not selected by default
	HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET);

	// give the reset pin a brief pulse, to reset everything
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
	tmp = 0xFF;
	while(tmp--);
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);

	// point to the mcu specific functions
	reg_wizchip_cris_cbfunc(mcu_cris_enter, mcu_cris_exit);
	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);

	// wizchip initialize
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize) == -1) {
		while(1);
		// @todo: handle this failed init case.
	}


	wizchip_setnetinfo(&netInfo);
	wizchip_getnetinfo(&netInfo);

}
