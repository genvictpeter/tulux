#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/select.h>
#include "def.h"
#include "yx_drv_gpio.h"
#include "yx_spi_drv.h"
#include "yx_roundbuf.h"
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>



#define YX_LOG_BUF(psFmz, pBuf, Len)       do {  \
		int i;							\
        printf("\n%s:%d\n",psFmz,Len); 		\
        for (i = 0; i < (Len); ++i) {     \
            printf("%02x,", (pBuf)[i]); \
        }                               \
        printf("\n");\
    } while(0)


#define YX_SPI_DRV_TX_BUF_SIZE   2048
#define YX_SPI_DRV_RX_BUF_SIZE   2048

#define YX_SPI_FIX_BUF_SIZE      64
#define YX_SPI_FILL_BYTE         0x00

typedef struct {
	int           spifd;
	int           spi_pipe_read;
	int           spi_pipe_write;
    yx_roundbuf_t       TxRingBuffer;
    yx_roundbuf_t       RxRingBuffer;
    INT8U	TxBuf[YX_SPI_DRV_TX_BUF_SIZE];
    INT8U	RxBuf[YX_SPI_DRV_RX_BUF_SIZE];
} yx_spi_drv_para_t;

static yx_spi_drv_para_t sgSpiDevObj;
static void (*pRxCallBack)(void* args) = NULL;
static int sgSpiSlaveFd = -1;
static int sgSpiLevelIcFd = -1;


//#define YX_SPI_DEV         "/dev/spidev0.0"
#define YX_SPI_DEV         "/dev/spidev2.0"
#define YX_SPI_SPEED_HZ    4000000

	
#define YX_SPI_SLAVE_GPIO_ID     128
#define YX_SPI_LEVEL_IC_ID       58		// 电平装换芯片id

#define YX_SPI_SLAVE_VALID_LEVEL   1        // spi valid level


static int _spi_drv_config(void)
{
    int ret;
    unsigned int mode = SPI_MODE_0;
    unsigned int lsb = 0;
    unsigned int bit = 8;
    unsigned int speed = YX_SPI_SPEED_HZ;

    sgSpiDevObj.spifd = open(YX_SPI_DEV, O_RDWR);
    if (sgSpiDevObj.spifd == -1) {
        printf("fail to open %s\n", YX_SPI_DEV);
        return sgSpiDevObj.spifd;
    }

    ret = ioctl(sgSpiDevObj.spifd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
        printf("ioctl SPI_IOC_WR_MODE error\n");
        goto end;
    }

    ret = ioctl(sgSpiDevObj.spifd, SPI_IOC_WR_LSB_FIRST, &lsb);
    if (ret == -1) {
        printf("ioctl SPI_IOC_WR_LSB_FIRST error\n");
        goto end;
    }

    ret = ioctl(sgSpiDevObj.spifd, SPI_IOC_WR_BITS_PER_WORD, &bit);
    if (ret == -1) {
        printf("ioctl SPI_IOC_WR_BITS_PER_WORD error\n");
        goto end;
    }

    ret = ioctl(sgSpiDevObj.spifd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        printf("ioctl SPI_IOC_WR_MAX_SPEED_HZ error\n");
        goto end;
    }

    return sgSpiDevObj.spifd;
end:
    close(sgSpiDevObj.spifd);
    return ret;
}


static int _spi_drv_transfer(char *txbuf,char *rxbuf, unsigned int len)
{
    int ret;
    struct spi_ioc_transfer trans;

    if (sgSpiDevObj.spifd == -1)
        return -1;

    trans.len = len;
    trans.speed_hz = YX_SPI_SPEED_HZ;
    trans.bits_per_word = 8;
    trans.delay_usecs = 20;
    trans.cs_change = 1;
    trans.rx_buf = (__u64)rxbuf;
    trans.tx_buf = (__u64)txbuf;
    trans.rx_nbits = 8;
    trans.tx_nbits = 8;
    ret = ioctl(sgSpiDevObj.spifd, SPI_IOC_MESSAGE(1), &trans);
    if (ret == -1) {
        printf("ioctl transfer message error\n");
    }

    return ret;
}




static void* yx_spi_drv_thread(void)
{
	fd_set			wait_fds; 
	fd_set 			exceptfd;
	int             max_fd,s,len,ret ;
	INT8U     txbuf[YX_SPI_FIX_BUF_SIZE];
	INT8U     rxbuf[YX_SPI_FIX_BUF_SIZE];
	INT16U    real_tx_len = 0;
	INT16U    real_rx_len = 0; 
	INT8U     invalid_level_cnt = 0; 
    struct timeval tv;
	pthread_detach(pthread_self());
	yx_drv_gpio_get_value_byfd(sgSpiSlaveFd);
	while (1) {
		tv.tv_sec =  0;
        tv.tv_usec = 500000;	// 500 ms 超时
		max_fd = -1;
		FD_ZERO(&wait_fds);
		FD_ZERO(&exceptfd);
		FD_SET(sgSpiDevObj.spi_pipe_read,&wait_fds);
		FD_SET(sgSpiSlaveFd,&exceptfd);
		max_fd = YX_MAX(sgSpiDevObj.spi_pipe_read,sgSpiSlaveFd);
		
		s = select(max_fd + 1, &wait_fds, NULL, &exceptfd, &tv);				 /* 调用select接口, 测试是否可以发送 */

		if (s != -1) {

			if (s == 0) {
				//printf("\nspi thread timer out !!");
			}
	        if (sgSpiDevObj.spi_pipe_read != -1&& FD_ISSET(sgSpiDevObj.spi_pipe_read, &wait_fds)) {  
			 	//printf("\nclear pipe read");
	            char    buf[16];                                                  
	            do {                                                                /* 清空管道数据 */
	                len = read(sgSpiDevObj.spi_pipe_read, buf, 16);
	            } while (len >= 16 || (len < 0 && errno == EINTR));
	        } else if (sgSpiSlaveFd != -1  && FD_ISSET(sgSpiSlaveFd, &exceptfd)) { /* 如果在管道上收到数据, 则说明 */
				//printf(" \ngpio int %d",yx_drv_gpio_get_value_byfd(sgSpiSlaveFd));
			}
			 /*
				TODO 
			 */
			if (yx_drv_gpio_get_value_byfd(sgSpiSlaveFd) == YX_SPI_SLAVE_VALID_LEVEL) {
				invalid_level_cnt = 0;
			} else if (invalid_level_cnt == 0) {
				invalid_level_cnt++;
			}
			
			while (yx_roundbuf_data_len(&sgSpiDevObj.TxRingBuffer) > 0  || (yx_drv_gpio_get_value_byfd(sgSpiSlaveFd) == YX_SPI_SLAVE_VALID_LEVEL) || (invalid_level_cnt < 1)){
				 if (yx_drv_gpio_get_value_byfd(sgSpiSlaveFd) == YX_SPI_SLAVE_VALID_LEVEL) {
					invalid_level_cnt = 0;
				 } else if (invalid_level_cnt == 0) {
					invalid_level_cnt++;
				 }
				 memset(txbuf,YX_SPI_FILL_BYTE,sizeof(txbuf));
				 memset(rxbuf,YX_SPI_FILL_BYTE,sizeof(rxbuf));
				 real_tx_len = yx_roundbuf_get(&sgSpiDevObj.TxRingBuffer,txbuf,YX_SPI_FIX_BUF_SIZE);
				 _spi_drv_transfer(txbuf, rxbuf, YX_SPI_FIX_BUF_SIZE);
				 //YX_LOG_BUF("\n---Snd Data:\n",txbuf,YX_SPI_FIX_BUF_SIZE);
				 if (yx_roundbuf_put(&sgSpiDevObj.RxRingBuffer, rxbuf, YX_SPI_FIX_BUF_SIZE)  != YX_SPI_FIX_BUF_SIZE) {
					printf("\nrx roundbuf error!!\n");
				 }
				 if (pRxCallBack != NULL) {
					pRxCallBack (NULL);
				 }
			 }
		}	
	}
}




int yx_spi_drv_init(void)
{
    int ret;
	int 	fds[2];
    pthread_t pid;
	char value;
#if 0    
	sgSpiSlaveFd = yx_drv_gpio_init(YX_SPI_SLAVE_GPIO_ID, GPIO_DIR_IN);
	if (sgSpiSlaveFd == -1 ) {
		printf("\nspi gpio spi slave errror !!");
	}
#endif //ypc    
    sgSpiLevelIcFd = yx_drv_gpio_init(YX_SPI_LEVEL_IC_ID, GPIO_DIR_OUT);
	if (sgSpiLevelIcFd == -1 ) {
		printf(" \nspi gpio level ic errror !!");
	}
	yx_drv_gpio_set_value_byfd(sgSpiLevelIcFd,1);
#if 0//ypc
#if YX_SPI_SLAVE_VALID_LEVEL == 1
    yx_drv_gpio_edge_set(YX_SPI_SLAVE_GPIO_ID, GPIO_EDGE_RISING);
#else
    yx_drv_gpio_edge_set(YX_SPI_SLAVE_GPIO_ID, GPIO_EDGE_FALLING);
#endif
#endif
	printf("\nspi drv config!!");
	_spi_drv_config();
	yx_roundbuf_init(&sgSpiDevObj.RxRingBuffer, sgSpiDevObj.RxBuf, YX_SPI_DRV_RX_BUF_SIZE);
	yx_roundbuf_init(&sgSpiDevObj.TxRingBuffer, sgSpiDevObj.TxBuf, YX_SPI_DRV_TX_BUF_SIZE);
	
    ret = pthread_create(&pid, NULL, yx_spi_drv_thread, NULL);
    if (ret == -1) {
        printf("\npthread_create error");
        return -1;
    }
	ret = pipe(fds);
	if (ret < 0) {
		printf("\nError in pipe(), errno: %d", errno);
	}
	sgSpiDevObj.spi_pipe_read  = fds[0];
	sgSpiDevObj.spi_pipe_write = fds[1];

	if (sgSpiDevObj.spi_pipe_read == -1 || sgSpiDevObj.spi_pipe_write == -1) {
		printf("\npipe error !!");
	}
//    fcntl(sgSpiDevObj.spi_pipe_read,  F_SETFL,O_NONBLOCK );							/* 将管道设置成阻塞模式 */
//    fcntl(sgSpiDevObj.spi_pipe_write, F_SETFL, O_NONBLOCK);
    if (yx_drv_gpio_get_value_byfd(sgSpiSlaveFd) == YX_SPI_SLAVE_VALID_LEVEL) {
		ret = write (sgSpiDevObj.spi_pipe_write, " ", 1);
	}
    return 0;
}

INT32S yx_spi_drv_send(INT8U * pBuf,INT32U len)
{
	int ret;
	if (len > yx_roundbuf_space_len(&sgSpiDevObj.TxRingBuffer)) {
		printf("spi tx space error!!\n");
		return 0;
	}
	ret = yx_roundbuf_put(&sgSpiDevObj.TxRingBuffer, pBuf, len);
	if (ret != len) {
		printf("spi roundbuf put error!!  ret = %d, len = %d\n",ret,len);
	}
    do {                                                                        /* 往管道中写入数据*/
        ret = write (sgSpiDevObj.spi_pipe_write, " ", 1);
    } while (ret < 0 && errno == EINTR);

	return len;
}

INT32S yx_spi_drv_read(INT8U * pBuf,INT32U len)
{
	INT32S rxlen = 0;
	if (yx_roundbuf_data_len(&sgSpiDevObj.RxRingBuffer) > 0) {
		rxlen = yx_roundbuf_get(&sgSpiDevObj.RxRingBuffer, (INT8U*)pBuf, (INT16U)len);
	}
	return rxlen;
}

void yx_spi_drv_register_rx_notice(void (*pRxNotice)(void* args))
{
	pRxCallBack  = pRxNotice;
}
