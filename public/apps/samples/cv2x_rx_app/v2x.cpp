#include <stdio.h>

#include <assert.h>
#include <ifaddrs.h>

#include <cstring>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include <telux/cv2x/Cv2xRadio.hpp>

#include "v2x.hpp"

using std::array;
using std::cerr;
using std::cout;
using std::endl;
using std::promise;
using std::shared_ptr;
using telux::common::ErrorCode;
using telux::common::Status;
using telux::cv2x::Cv2xFactory;
using telux::cv2x::Cv2xStatus;
using telux::cv2x::Cv2xStatusType;
using telux::cv2x::ICv2xRxSubscription;
using telux::cv2x::TrafficCategory;
using telux::cv2x::TrafficIpType;
using telux::cv2x::ICv2xTxFlow;
using telux::cv2x::Periodicity;
using telux::cv2x::Priority;
using telux::cv2x::SpsFlowInfo;


static constexpr uint16_t RX_PORT_NUM = 9000u;
static constexpr uint32_t GRx_BUF_LEN = 3000u;
static constexpr uint32_t NUM_TEST_ITERATIONS = 100u;

static Cv2xStatus gCv2xRxStatus;
static Cv2xStatus gCv2xTxStatus;
static promise<ErrorCode> gTxCallbackPromise;
static promise<ErrorCode> gRxCallbackPromise;

static Cv2xStatus gCv2xStatus;
static promise<ErrorCode> gCallbackPromise;
static shared_ptr<ICv2xRxSubscription> gRxSub;
static uint32_t gPacketsReceived = 0u;
static array<char, GRx_BUF_LEN> gRxBuf;

static constexpr uint32_t SPS_SERVICE_ID = 1u;
static constexpr uint16_t SPS_SRC_PORT_NUM = 2500u;
static constexpr uint32_t G_BUF_LEN = 128;
static constexpr int      PRIORITY = 3;

static constexpr char TEST_VERNO_MAGIC = 'Q';
static constexpr char UEID = 1;
static shared_ptr<ICv2xTxFlow> gSpsFlow;
static array<char, G_BUF_LEN> gBuf;

//add yaxon
V2xRegisterInfo *g_cb_info;

// Resets the global callback promise
static inline void resetCallbackPromise(void) {
    gCallbackPromise = promise<ErrorCode>();
}

// Resets the global callback promise
static inline void resetRxCallbackPromise(void) {
    gRxCallbackPromise = promise<ErrorCode>();
}

// Resets the global callback promise
static inline void resetTxCallbackPromise(void) {
    gTxCallbackPromise = promise<ErrorCode>();
}

// Callback function for Cv2xRadioManager->requestCv2xStatus()
static void cv2xStatusCallback(Cv2xStatus status, ErrorCode error) {
    if (ErrorCode::SUCCESS == error) {
        gCv2xStatus = status;
    }
    gCallbackPromise.set_value(error);
}

// Callback function for Cv2xRadioManager->requestCv2xStatus()
static void cv2xRxStatusCallback(Cv2xStatus status, ErrorCode error) {
    if (ErrorCode::SUCCESS == error) {
        gCv2xRxStatus = status;
    }
    gRxCallbackPromise.set_value(error);
}

// Callback function for Cv2xRadioManager->requestCv2xStatus()
static void cv2xTxStatusCallback(Cv2xStatus status, ErrorCode error) {
    if (ErrorCode::SUCCESS == error) {
        gCv2xTxStatus = status;
    }
    gTxCallbackPromise.set_value(error);
}

// Callback function for ICv2xRadio->createTxSpsFlow()
static void createSpsFlowCallback(shared_ptr<ICv2xTxFlow> txSpsFlow,
                                  shared_ptr<ICv2xTxFlow> unusedFlow,
                                  ErrorCode spsError,
                                  ErrorCode unusedError) {
    if (ErrorCode::SUCCESS == spsError) {
        gSpsFlow = txSpsFlow;
    }
    gTxCallbackPromise.set_value(spsError);
}

// Callback function for Cv2xRadio->createRxSubscription() and Cv2xRadio->closeRxSubscription()
static void rxSubCallback(shared_ptr<ICv2xRxSubscription> rxSub, ErrorCode error) {
    if (ErrorCode::SUCCESS == error) {
        gRxSub = rxSub;
    }
    gCallbackPromise.set_value(error);
}

// Callback for ICv2xRadio->closeTxFlow()
static void closeFlowCallback(shared_ptr<ICv2xTxFlow> flow, ErrorCode error) {
    gCallbackPromise.set_value(error);
}
#if 0
// Function for reading from Rx socket
static void sampleRx(void) {
    V2xRxData param;
    unsigned char *dataSave;
    int sock = gRxSub->getSock();

    // Attempt to read from socket
    int RecvByte = recv(sock, gRxBuf.data(), gRxBuf.max_size(), 0);

    dataSave = (unsigned char *)malloc(gRxBuf.max_size());
    memcpy(dataSave, gRxBuf.data(), gRxBuf.max_size());
    
    param.length      = RecvByte;
    param.data        = dataSave;

    g_cb_info->callback(&param);

    free(dataSave);
    dataSave = NULL;
}
#endif

// Function for reading from Rx socket
static void sampleRx(void) {

    int sock = gRxSub->getSock();

    cout << "sampleRx(" << sock << ")" << endl;

    // Attempt to read from socket
    int n = recv(sock, gBuf.data(), gBuf.max_size(), 0);
    cout << "Rx array values: " << endl;
    for (auto it = gBuf.begin(); it != gBuf.end(); ++it) {
          printf("0x%02x ", *it);
    }
    if (n < 0) {
        cerr << "Error occurred reading from socket[" << sock << "]" << endl;
    }
    else {
        cout << __FUNCTION__ << ": Received " << n << " bytes" << endl;
        ++gPacketsReceived;
    }
}

int yx_V2xDataSend(V2xTxData *txInfo)
{
    static uint32_t txCount = 0u;

    int sock = gSpsFlow->getSock();
    
    cout << "Tx(" << sock << ")" << endl;
    
    struct msghdr message = {0};
    struct iovec iov[1] = {0};
    struct cmsghdr * cmsghp = NULL;
    char control[CMSG_SPACE(sizeof(int))];

    // Send data using sendmsg to provide IPV6_TCLASS per packet
    iov[0].iov_base = txInfo->data;
    iov[0].iov_len = txInfo->length;
    message.msg_iov = iov;
    message.msg_iovlen = 1;
    message.msg_control = control;
    message.msg_controllen = sizeof(control);

    // Fill ancillary data
    int priority = PRIORITY;
    cmsghp = CMSG_FIRSTHDR(&message);
    cmsghp->cmsg_level = IPPROTO_IPV6;
    cmsghp->cmsg_type = IPV6_TCLASS;
    cmsghp->cmsg_len = CMSG_LEN(sizeof(int));
    memcpy(CMSG_DATA(cmsghp), &priority, sizeof(int));
                                                 
    // Send data
    auto bytes_sent = sendmsg(sock, &message, 0);
    cout << "bytes_sent=" << bytes_sent << endl;
                                                                                 
    // Check bytes sent
    if (bytes_sent < 0) {
        cerr << "Error sending message: " << bytes_sent << endl;
        bytes_sent = -1;
    } else {
        if (bytes_sent == txInfo->length) {
            ++txCount;
        } else {
            cerr << "Error : " << bytes_sent << " bytes sent." << endl;
        }
    }
                                                                                                                                                    
    cout << "TX count: " << txCount << endl;

    return 0;
}

int yx_V2xDataRecvRegister(V2xRegisterInfo *cbInfo)
{
    if (cbInfo == NULL)
        return -1;
    g_cb_info = cbInfo;

    return 0;
}

void* v2x_pthread(void* data)
{
    while (1){
        sampleRx();
    }
}

static int v2x_rx_init(void)
{
    cout << "v2x rx init Start" << endl;

    // Get handle to Cv2xRadioManager
    auto & cv2xFactory = Cv2xFactory::getInstance();
    auto cv2xRadioManager = cv2xFactory.getCv2xRadioManager();

    // Wait for radio manager to complete initialization
    if (not cv2xRadioManager->isReady()) {
        if (cv2xRadioManager->onReady().get()) {
            cout << "C-V2X Radio Manager is ready" << endl;
        }
        else {
            cerr << "C-V2X Radio Manager initialization failed, exiting" << endl;
            return EXIT_FAILURE;
        }
    }

    // Get C-V2X status and make sure Rx is enabled
    assert(Status::SUCCESS == cv2xRadioManager->requestCv2xStatus(cv2xStatusCallback));
    assert(ErrorCode::SUCCESS == gCallbackPromise.get_future().get());

    if (Cv2xStatusType::ACTIVE == gCv2xStatus.rxStatus) {
        cout << "C-V2X RX status is active" << endl;
    }
    else {
        cerr << "C-V2X RX is inactive" << endl;
        return EXIT_FAILURE;
    }

    // Get handle to Cv2xRadio
    auto cv2xRadio = cv2xRadioManager->getCv2xRadio(TrafficCategory::SAFETY_TYPE);

    // Wait for radio to complete initialization
    if (not cv2xRadio->isReady()) {
        if (Status::SUCCESS == cv2xRadio->onReady().get()) {
            cout << "C-V2X Radio is ready" << endl;
        }
        else {
            cerr << "C-V2X Radio initialization failed." << endl;
            return EXIT_FAILURE;
        }
    }

    // Create new Rx subscription
    resetCallbackPromise();
    assert(Status::SUCCESS == cv2xRadio->createRxSubscription(TrafficIpType::TRAFFIC_NON_IP,
                                                              RX_PORT_NUM,
                                                              rxSubCallback));
    assert(ErrorCode::SUCCESS == gCallbackPromise.get_future().get());

    cout << "v2x rx init Done" << endl;

    return 0;
}

static int v2x_tx_init(void)
{
    cout << "v2x tx init Start" << endl;

    // Get handle to Cv2xRadioManager
    auto & cv2xFactory = Cv2xFactory::getInstance();
    auto cv2xRadioManager = cv2xFactory.getCv2xRadioManager();
    
    // Wait for radio manager to complete initialization
    if (not cv2xRadioManager->isReady()) {
        if (cv2xRadioManager->onReady().get()) {
            cout << "C-V2X Radio Manager is ready" << endl;
        }
        else {
            cerr << "C-V2X Radio Manager initialization failed, exiting" << endl;
            return EXIT_FAILURE;
        }
    }

    // Get C-V2X status and make sure Tx is enabled
    assert(Status::SUCCESS == cv2xRadioManager->requestCv2xStatus(cv2xTxStatusCallback));
    assert(ErrorCode::SUCCESS == gTxCallbackPromise.get_future().get());

    if (Cv2xStatusType::ACTIVE == gCv2xTxStatus.txStatus) {
        cout << "C-V2X TX status is active" << endl;
    }
    else {
        cerr << "C-V2X TX is inactive" << endl;
        return EXIT_FAILURE;
    }

    // Get handle to Cv2xRadio
    auto cv2xRadio = cv2xRadioManager->getCv2xRadio(TrafficCategory::SAFETY_TYPE);

    // Wait for radio to complete initialization
    if (not cv2xRadio->isReady()) {
        if (Status::SUCCESS == cv2xRadio->onReady().get()) {
            cout << "C-V2X Radio is ready" << endl;
        }
        else {
            cerr << "C-V2X Radio initialization failed." << endl;
            return EXIT_FAILURE;
        }
    }

    // Create new Tx SPS flow
    SpsFlowInfo spsInfo;
    spsInfo.priority = Priority::PRIORITY_2;
    spsInfo.periodicity = Periodicity::PERIODICITY_100MS;
    spsInfo.nbytesReserved = G_BUF_LEN;
    spsInfo.autoRetransEnabledValid = true;
    spsInfo.autoRetransEnabled = true;

    resetTxCallbackPromise();
    assert(Status::SUCCESS == cv2xRadio->createTxSpsFlow(TrafficIpType::TRAFFIC_NON_IP,
                                                         SPS_SERVICE_ID,
                                                         spsInfo,
                                                         SPS_SRC_PORT_NUM,
                                                         false,
                                                         0,
                                                         createSpsFlowCallback));
    assert(ErrorCode::SUCCESS == gTxCallbackPromise.get_future().get());
    
    cout << "v2x tx init Done" << endl;

    return 0;
}

int yx_V2xSdkRxInit(void)
{
    pthread_t apiPthread;

    v2x_rx_init();

    pthread_create(&apiPthread, NULL, v2x_pthread, NULL);
    
    return 0;
}

int yx_V2xSdkTxInit(void)
{
    v2x_tx_init();

    return 0;
}


// Returns current timestamp
static uint64_t getCurrentTimestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000ull + tv.tv_usec;
}

// Fills buffer with dummy data
static void fillBuffer(void) {

    static uint16_t seq_num = 0u;
    auto timestamp = getCurrentTimestamp();

    // Very first payload is test Magic number, this is  where V2X Family ID would normally be.
    gBuf[0] = TEST_VERNO_MAGIC;

    // Next byte is the UE equipment ID
    gBuf[1] = UEID;

    // Sequence number
    auto dataPtr = gBuf.data() + 2;
    uint16_t tmp = htons(seq_num++);
    memcpy(dataPtr, reinterpret_cast<char *>(&tmp), sizeof(uint16_t));
    dataPtr += sizeof(uint16_t);

    // Timestamp
    dataPtr += snprintf(dataPtr, G_BUF_LEN - (2 + sizeof(uint16_t)),
                        "<%llu> ", static_cast<long long unsigned>(timestamp));

    printf("gBuf[0]=0x%02x, gBuf[1]=0x%02x", gBuf.at(0), gBuf.at(1));
    // Dummy payload
    constexpr int NUM_LETTERS = 26;
    auto i = 2 + sizeof(uint16_t) - sizeof(long long unsigned);
    for (; i < G_BUF_LEN; ++i) {
        gBuf[i] = 'a' + ((seq_num + i) % NUM_LETTERS);
    }
}

// Function for transmitting data
static void sampleSpsTx(void) {

    static uint32_t txCount = 0u;
    int sock = gSpsFlow->getSock();

    cout << "sampleSpsTx(" << sock << ")" << endl;

    struct msghdr message = {0};
    struct iovec iov[1] = {0};
    struct cmsghdr * cmsghp = NULL;
    char control[CMSG_SPACE(sizeof(int))];

    // Send data using sendmsg to provide IPV6_TCLASS per packet
    iov[0].iov_base = gBuf.data();
    iov[0].iov_len = G_BUF_LEN;
    message.msg_iov = iov;
    message.msg_iovlen = 1;
    message.msg_control = control;
    message.msg_controllen = sizeof(control);

    // Fill ancillary data
    int priority = PRIORITY;
    cmsghp = CMSG_FIRSTHDR(&message);
    cmsghp->cmsg_level = IPPROTO_IPV6;
    cmsghp->cmsg_type = IPV6_TCLASS;
    cmsghp->cmsg_len = CMSG_LEN(sizeof(int));
    memcpy(CMSG_DATA(cmsghp), &priority, sizeof(int));

    // Send data
    auto bytes_sent = sendmsg(sock, &message, 0);
    cout << "bytes_sent=" << bytes_sent << endl;

    // Check bytes sent
    if (bytes_sent < 0) {
        cerr << "Error sending message: " << bytes_sent << endl;
        bytes_sent = -1;
    } else {
        if (bytes_sent == G_BUF_LEN) {
           ++txCount;
        } else {
            cerr << "Error : " << bytes_sent << " bytes sent." << endl;
        }
    }

    cout << "TX count: " << txCount << endl;
}


static int yx_V2xRxTxInit(void)
{
    cout << "Running Sample C-V2X RX app" << endl;

    // Get handle to Cv2xRadioManager
    auto & cv2xFactory = Cv2xFactory::getInstance();
    auto cv2xRadioManager = cv2xFactory.getCv2xRadioManager();

    // Wait for radio manager to complete initialization
    if (not cv2xRadioManager->isReady()) {
        if (cv2xRadioManager->onReady().get()) {
            cout << "C-V2X Radio Manager is ready" << endl;
        }
        else {
            cerr << "C-V2X Radio Manager initialization failed, exiting" << endl;
            return EXIT_FAILURE;
        }
    }

    // Get C-V2X status and make sure Rx is enabled
    assert(Status::SUCCESS == cv2xRadioManager->requestCv2xStatus(cv2xStatusCallback));
    assert(ErrorCode::SUCCESS == gCallbackPromise.get_future().get());

    if (Cv2xStatusType::ACTIVE == gCv2xStatus.rxStatus) {
        cout << "C-V2X RX status is active" << endl;
    }
    else {
        cerr << "C-V2X RX is inactive" << endl;
        return EXIT_FAILURE;
    }
#if 1
    if (Cv2xStatusType::ACTIVE == gCv2xStatus.txStatus) {
        cout << "C-V2X TX status is active" << endl;
    }
    else {
        cerr << "C-V2X TX is inactive" << endl;
        return EXIT_FAILURE;
    }
#endif
    // Get handle to Cv2xRadio
    auto cv2xRadio = cv2xRadioManager->getCv2xRadio(TrafficCategory::SAFETY_TYPE);

    // Wait for radio to complete initialization
    if (not cv2xRadio->isReady()) {
        if (Status::SUCCESS == cv2xRadio->onReady().get()) {
            cout << "C-V2X Radio is ready" << endl;
        }
        else {
            cerr << "C-V2X Radio initialization failed." << endl;
            return EXIT_FAILURE;
        }
    }


    // Create new Rx subscription
    resetCallbackPromise();
    assert(Status::SUCCESS == cv2xRadio->createRxSubscription(TrafficIpType::TRAFFIC_NON_IP,
                                                              RX_PORT_NUM,
                                                              rxSubCallback));
    assert(ErrorCode::SUCCESS == gCallbackPromise.get_future().get());
#if 1
    // Read from the RX socket in a loop
    for (uint32_t i = 0; i < 10; ++i) {
        sampleRx();
    }
#endif

    //tx:
    cout << "Running Sample C-V2X TX app" << endl;
#if 0
    // Get handle to Cv2xRadioManager
    auto & cv2xFactory1 = Cv2xFactory::getInstance();
    auto cv2xRadioManager1 = cv2xFactory.getCv2xRadioManager();

    // Wait for radio manager to complete initialization
    if (not cv2xRadioManager1->isReady()) {
        if (cv2xRadioManager1->onReady().get()) {
            cout << "C-V2X Radio Manager is ready" << endl;
        }
        else {
            cerr << "C-V2X Radio Manager initialization failed, exiting" << endl;
            return EXIT_FAILURE;
        }
    }
#endif

#if 0
    // Get C-V2X status and make sure Tx is enabled
    assert(Status::SUCCESS == cv2xRadioManager1->requestCv2xStatus(cv2xStatusCallback));
    assert(ErrorCode::SUCCESS == gCallbackPromise.get_future().get());

    if (Cv2xStatusType::ACTIVE == gCv2xStatus.txStatus) {
        cout << "C-V2X TX status is active" << endl;
    }
    else {
        cerr << "C-V2X TX is inactive" << endl;
        return EXIT_FAILURE;
    }

    // Get handle to Cv2xRadio
    auto cv2xRadio1 = cv2xRadioManager1->getCv2xRadio(TrafficCategory::SAFETY_TYPE);

    // Wait for radio to complete initialization
    if (not cv2xRadio1->isReady()) {
        if (Status::SUCCESS == cv2xRadio1->onReady().get()) {
            cout << "C-V2X Radio is ready" << endl;
        }
        else {
            cerr << "C-V2X Radio initialization failed." << endl;
            return EXIT_FAILURE;
        }
    }
#endif
    cout << "11111" << endl;
    // Create new Tx SPS flow
    SpsFlowInfo spsInfo;
    spsInfo.priority = Priority::PRIORITY_2;
    spsInfo.periodicity = Periodicity::PERIODICITY_100MS;
    spsInfo.nbytesReserved = G_BUF_LEN;
    spsInfo.autoRetransEnabledValid = true;
    spsInfo.autoRetransEnabled = true;

    cout << "22222" << endl;
    resetCallbackPromise();
    cout << "33333" << endl;
    assert(Status::SUCCESS == cv2xRadio->createTxSpsFlow(TrafficIpType::TRAFFIC_NON_IP,
                                                         SPS_SERVICE_ID,
                                                         spsInfo,
                                                         SPS_SRC_PORT_NUM,
                                                         false,
                                                         0,
                                                         createSpsFlowCallback));
    cout << "44444" << endl;
    assert(ErrorCode::SUCCESS == gCallbackPromise.get_future().get());
    cout << "55555" << endl;
#if 1
    // Send message in a loop
    for (uint16_t i = 0; i < 10; ++i) {
        fillBuffer();
        sampleSpsTx();
        usleep(100000u);
    }
#endif
    return 0;
}

int yx_V2xSdkInit(void)
{
    cout << "yx_V2xSdkInit start" << endl;
    yx_V2xRxTxInit();

    return 0;
}
