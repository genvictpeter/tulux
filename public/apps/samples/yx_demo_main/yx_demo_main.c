#include "serial_link/serial_link.h"
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include "serial_link/yx_drv_gpio.h"
#include "can/yx_can.h"
#include "def.h"


#define YX_TRACE_BUF(psFmz, pBuf, Len)       do {  \
		int i;							\
        printf("\n%s:%d\n",psFmz,Len); 		\
        for (i = 0; i < (Len); ++i) {     \
            printf("%02x,", (pBuf)[i]); \
        }                               \
        printf("\n");\
    } while(0)


void yx_thread_time_delay(int ms)
{
    struct timeval tv;

    tv.tv_sec = ms / 1000;
    tv.tv_usec = ms % 1000 * 1000;

    select(0, NULL, NULL, NULL, &tv);
}

void yx_can_send_test(void* arg)
{	
	pthread_detach(pthread_self());
	
	while (1) {
        yx_thread_time_delay(10000);
		
		/*can 发送测试*/
		Yx_Can_Send(CAN_CHANNEL_1,CAN_FORMAT_STD,CAN_FRAME_TYPE_DATA,0x7f8,"12345678",8);
		Yx_Can_Send(CAN_CHANNEL_1,CAN_FORMAT_EXT,CAN_FRAME_TYPE_DATA,0x18ffec00,"12345678",8);

		Yx_Can_Send(CAN_CHANNEL_2,CAN_FORMAT_STD,CAN_FRAME_TYPE_DATA,0x600,"12345678",8);
		Yx_Can_Send(CAN_CHANNEL_2,CAN_FORMAT_EXT,CAN_FRAME_TYPE_DATA,0x18fefc01,"12345678",8);
	}
}

void _yx_can_rx_callback(YX_CAN_RX_T* pData)
{
	printf("\r\n can data rec :canid:0x%x,format:%d,type:%d,len:%d",pData->CanId,pData->Format,pData->Type,pData->Len);
    YX_TRACE_BUF("can data",pData->Data,pData->Len);
}



int main()
{
#if 1 //by yangpengcheng
	int ret;
    pthread_t pid;
	Yx_Serial_Link_Init();							/*spi 链路初始化*/
	Yx_Can_Init();									/*can 模块初始化*/
	Yx_Can_RegisterRxCallBack(_yx_can_rx_callback);	/* 注册接收函数*/
	Yx_Can_SetBaud(CAN_CHANNEL_1, 250000);			/*设置can1 波特率250K*/
	Yx_Can_SetBaud(CAN_CHANNEL_2, 500000);			/*设置can2 波特率500K*/


	ret = pthread_create(&pid, NULL, yx_can_send_test, NULL);	/*发送线程测试  每10s 发送一次*/
	if (ret < 0){
		printf("create thread error %d\n!!",ret);
	}
	
    while (1) {
		yx_thread_time_delay(10000);
    }
#endif
	return 0;

}


