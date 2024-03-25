#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "v2x.hpp"


int rx_data_cb_handler(V2xDsmpRxData *param)
{
    printf("recv %d bytes:", param->length);
    for(int i=0; i<param->length; i++) {
        printf("%02x ", param->data[i]);
    }
    printf("\n");
    return 0;
}

int main(int argc, char* agrv[])
{
    yx_V2xSdkInit();

    printf("yx_V2xSdkInit done\n");
#if 0
    int ret;
    unsigned char senddata[40] = {0};
#if 0
    yx_V2xSdkRxInit();
#else
    yx_V2xSdkTxInit();
#endif
    printf("main 1111\n");

    V2xDsmpTxData tx_info = {
         .aid         = 17,
         .priority    = 1,
    };
#if 0
    V2xDsmpRegisterInfo cb_info = {
        .aid = 17,
        .callback = rx_data_cb_handler,
    };

    ret = yx_V2xDsmpDataRecvRegister(&cb_info);
    if (ret < 0) {
        printf("v2x recv register failed\n");
        return -1;
    }
#endif    
#if 0
    while (1);
#else
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
        yx_V2xDsmpDataSend(&tx_info);
        sleep(1);
    }
#endif
#endif
    return 0;
}
