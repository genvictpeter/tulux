/*
 * SPI testing utility (using spidev driver)
 * for example: insmod /lib/modules/4.14.151/kernel/drivers/spi/spidev.ko busnum=1 chipselect=0
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define device   "/dev/spidev2.0"

typedef enum
{
	SPIMODE0  =   SPI_MODE_0,
	SPIMODE1  =   SPI_MODE_1,
	SPIMODE2  =   SPI_MODE_2,
	SPIMODE3  =   SPI_MODE_3,
}SPI_MODE;

/**************************************************************************

SPI default support frequency

**************************************************************************/
typedef enum
{
	S_960K  	=   960000,
	S_4_8M 		=   4800000,
	S_9_6M  	=   9600000,
	S_16M  		=   16000000,
	S_19_2M 	=   19200000,
	S_25M  		=   25000000,
	S_50M  		=   50000000,
}SPI_SPEED;


static uint8_t spi_bits = 8;
static uint32_t  spi_speed = 10000000;
static uint16_t spi_delay;

/**************************************************************************
dev_name:need to insmod spidev.ko 
mode:the default is mode0
    CPOL:when spi work in idles,the level of sclk, 1:HIGH,0:LOW
    CHPA:sample on the first few edges of sclk     0:the first edge 1:the second edge
bits:bits of word  
speed:the default support spi frequency  
**************************************************************************/

int ql_spi_init(char *dev_name,SPI_MODE mode,uint8_t bits,SPI_SPEED speed)
{
    int ret = -1;
    int fd_spi = open(dev_name, O_RDWR);
    
    printf("< open(%s, O_RDWR)=%d >\n", dev_name, fd_spi);
    if (fd_spi < 0)  
    {
        printf("< Fail to open spi >\n");
        return -1;  
    }  
    /*
    * spi mode
    */
    ret = ioctl(fd_spi, SPI_IOC_WR_MODE32, &mode);
    if (ret == -1)
    {
        perror("can't set spi mode");
        close(fd_spi);
        return ret;
    }
    /*
     * bits per word
     */
    spi_bits = bits;
    ret = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &spi_bits);
    if (ret == -1)
    {
        perror("can't set bits per word");
        close(fd_spi);
        return ret;
    }

    /*
    * speed
    */
    spi_speed = (uint32_t)speed;
    ret = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if (ret == -1)
    {
        perror("can't set max speed hz");
        close(fd_spi);
        return ret;
    }
    printf("spi mode:0x%x\n", mode);
    printf("bits per word: %d\n", spi_bits);
    printf("max speed    : %d Hz (%d KHz)\n", spi_speed, spi_speed/1000);

    return fd_spi;
}

int ql_spi_write_read(int fd,uint8_t* write_buf,uint8_t* read_buf,uint32_t len)
{
    int ret;
    struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)write_buf,
    .rx_buf = (unsigned long)read_buf,
    .len = len,
    .delay_usecs = spi_delay,
    .speed_hz = spi_speed,
    .bits_per_word = spi_bits,
    };
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 0)
    {
        perror("can't send spi message");
        return ret;
    }

    return ret;
}

int ql_spi_deinit(int fd)
{
    close(fd);
    return 0;
}


int main(int argc, char *argv[])
{
	int fd;
	int i;
	uint8_t  writebuf[1024];
	uint8_t  readbuf[1024];

	fd = ql_spi_init(device,SPIMODE0,8,S_19_2M);

	for(i = 0 ;i < 1024;i++)
		writebuf[i] = i%256;

	ql_spi_write_read(fd,writebuf,readbuf,1024);

	for (i = 0; i<1024; i++) {
		if (!(i % 32))
		puts("");
		printf("%.2X ", readbuf[i]);
	}
	puts("");
	
	ql_spi_deinit(fd);
	return  0 ;
}
