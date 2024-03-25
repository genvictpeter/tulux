#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "v2x.hpp"

int main(int argc, char* agrv[])
{
    int ret;
    unsigned char senddata[40] = {0};
    
    yx_V2xSdkTxInit();

    V2xDsmpTxData tx_info = {
         .aid         = 17,
         .priority    = 1,
    };
    
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

    return 0;
}
