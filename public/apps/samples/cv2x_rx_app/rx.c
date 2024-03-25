#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "v2x.hpp"

#if 0
int rx_data_cb_handler(V2xRxData *param)
{
    printf("recv %d bytes:", param->length);
    for(int i=0; i<param->length; i++) {
        printf("%02x ", param->data[i]);
    }
    printf("\n");
    return 0;
}
#if 1
void* v2x_pthreadRecv(void* data)
{
    V2xRegisterInfo cb_info = {
        .callback = rx_data_cb_handler,
    };

    yx_V2xDataRecvRegister(&cb_info);
}
#endif
void* v2x_pthreadSend(void* data)
{
    unsigned char senddata[40] = {0};
    V2xTxData tx_info;

    for(int i=2; i<40; i++) {
        senddata[i] = i;
    }

    for(int k=0; k<20; k++) {
        senddata[0] = 0x28;
        senddata[1] = 0x12;
        printf("send data:");
        for(int j=0; j<40; j++) {
            printf("%02x ", senddata[j]);
        }
        printf("\n");
        tx_info.length = sizeof(senddata)/sizeof(senddata[0]);
        tx_info.data = senddata;
        yx_V2xDataSend(&tx_info);
        sleep(1);
    }
}
#endif
int main(int argc, char* agrv[])
{
    //int ret;
    //pthread_t apiPthreadRecv;
    //pthread_t apiPthreadSend;

    //yx_V2xSdkTxInit();
    yx_V2xSdkInit();

    //pthread_create(&apiPthreadRecv, NULL, v2x_pthreadRecv, NULL);

    //pthread_create(&apiPthreadSend, NULL, v2x_pthreadSend, NULL);

    while (1);
    
    return 0;
}
