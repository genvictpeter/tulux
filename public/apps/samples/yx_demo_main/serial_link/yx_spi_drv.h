#ifndef _YX_SPI_DRV_H_
#define _YX_SPI_DRV_H_
#include "def.h"

int yx_spi_drv_init(void);
INT32S yx_spi_drv_send(INT8U * pBuf,INT32U len);
INT32S yx_spi_drv_read(INT8U * pBuf,INT32U len);
void yx_spi_drv_register_rx_notice(void (*pRxNotice)(void* args));

#endif

