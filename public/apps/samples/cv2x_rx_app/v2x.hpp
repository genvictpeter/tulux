#ifndef YX_V2X_H
#define YX_V2X_H

typedef struct {
    unsigned short length;
    const unsigned char *data;
} V2xRxData;

typedef struct {
    unsigned short length;
    unsigned char *data;
} V2xTxData;

typedef int (*V2xRecvCallback)(V2xRxData *param);

typedef struct {
    V2xRecvCallback callback;
} V2xRegisterInfo;


int yx_V2xSdkInit(void);
int yx_V2xSdkRxInit(void);
int yx_V2xSdkTxInit(void);
int yx_V2xDataRecvRegister(V2xRegisterInfo *cbInfo);
int yx_V2xDataSend(V2xTxData *txInfo);

#endif
