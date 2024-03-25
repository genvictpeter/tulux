/*
 *  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       AudioQmiClient.hpp
 * @brief      QMI Client interacts with Telematics SDK Audio service to send/receive
 *             QMI requests/ indications and dispatch to respective listeners
 */

#ifndef AUDIOQMICLIENT_HPP
#define AUDIOQMICLIENT_HPP

#include "QmiClient.hpp"

extern "C" {
#include "telsdk_audio_service_v01.h"
}

namespace telux {
namespace qmi {

class IQmiLoadSubsystemCallback;
class IQmiGetSupportedDevicesCallback;
class IQmiGetSupportedStreamsCallback;
class IQmiCreateStreamCallback;
class IQmiDeleteStreamCallback;
class IQmiStreamSetDeviceCallback;
class IQmiStreamGetDeviceCallback;
class IQmiStreamSetVolumeCallback;
class IQmiStreamGetVolumeCallback;
class IQmiStreamSetMuteCallback;
class IQmiStreamGetMuteCallback;
class IQmiStreamStartCallback;
class IQmiStreamStopCallback;
class IQmiStreamWriteCallback;
class IQmiStreamReadCallback;
class IQmiDtmfToneCallback;
class IQmiRegisterEventCallback;
class IQmiLoopbackCallback;
class IQmiToneGenCallback;
class IQmiAudioVoiceListener;
class IQmiPlayStreamFlushCallback;
class IQmiPlayStreamDrainCallback;
class IQmiCreateTranscoderCallback;
class IQmiTranscoderTeardownCallback;
class IQmiAudioPlayListener;
class IQmiAudioServiceListener;
class IQmiGetCalInitStatusCallback;

// Data Representation as per audio interface, to synchronize with Qmi layer representation.
struct QmiFormatParams {
};

struct QmiAmrwbpParams : QmiFormatParams {
   uint32_t bitWidth;
   uint32_t frameFormat;
};

struct QmiTransCodeStreamConfig {
    uint32_t inSampleRate;
    uint32_t inChannelMask;
    uint32_t inFormat;
    QmiFormatParams *inCodecParams;
    uint32_t inCodecParamLen;
    uint32_t outSampleRate;
    uint32_t outChannelMask;
    uint32_t outFormat;
    QmiFormatParams *outCodecParams;
    uint32_t outCodecParamLen;
};

struct QmiStreamConfig {
    uint32_t halReference;
    uint32_t streamType;
    uint32_t subId;
    uint32_t sampleRate;
    uint32_t channelMask;
    uint32_t format;
    uint32_t numDevices;
    uint32_t *deviceType;
    QmiFormatParams *codecParams;
    uint32_t codecParamLen;
    uint32_t numVoicePaths;
    uint32_t *voicePaths;
    uint32_t ecnrMode;
};

/**
 * Represents state of Audio calibration file initialization in system.
 */
enum class QmiCalInitStatus {
    UNKNOWN = -1,       /**< calibration file initialization status is unknown*/
    INIT_SUCCESS = 0,  /**< calibration file initialized successfully*/
    INIT_FAILED = 1,   /**< calibration file initialization failed*/
};

class AudioQmiClient : public QmiClient {
public:
    /**
     * This function is called by the QmiClient::qmiIndicationCallback infrastructure when it
     * receives an indication for this client.
     *
     * NOTE: Callback happens in the QCCI thread context and raises signal.
     *
     * @param [in] userHandle           Opaque handle used by the infrastructure to
     *                                  identify different services.
     * @param [in] msgId                Message ID of the indication
     * @param [in] indBuf               Buffer holding the encoded indication
     * @param [in] indBufLen            Length of the encoded indication
     * @param [in] indCbData            Cookie user data value supplied by the client during
     *                                  registration
     */
    void indicationHandler(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
                           unsigned int indBufLen, void *infCbData) override;

    /**
     * This function is called by the QmiClient::qmiAsyncResponseHandler infrastructure when it
     * receives an asynchronous response for this client.
     *
     * NOTE: Callback happens in the QCCI thread context and raises signal.
     *
     * @param [in] msgId                Message ID of the indication
     * @param [in] respCStruct          Buffer holding the decoded response
     * @param [in] respCStructLen       Length of the decoded response
     * @param [in] userData             Cookie user data value supplied by the client
     * @param [in] transpErr            QMI error
     * @param [in] callback             Command callback pointer
     */
    void asyncResponseHandler(unsigned int msgId, void *respCStruct, unsigned int RespCStructLen,
                              void *userData, qmi_client_error_type transpErr,
                              std::shared_ptr<telux::common::ICommandCallback> callback) override;

    /**
     * This function is called by qmiClientErrorCallbackSync when the QMI service
     * terminates or deregisters.
     *
     * NOTE: Callback happens on new thread created by std::async.
     *
     * @param [in] clientError           Error value
     */
    void errorHandler(qmi_client_error_type clientError);

    /**
     * This function is called by qmiClientNotifierCallbackSync when a QMI service
     * event occurs indicating that the service count has changed.
     *
     * NOTE: Callback happens on new thread created by std::async.
     *
     * @param [in]   serviceEvent       Event type
     */
    void notifierHandler(qmi_client_notify_event_type serviceEvent);

    telux::common::Status sendLoadSubsystem(std::shared_ptr<IQmiLoadSubsystemCallback> callback,
                                            void *userData = nullptr);

    telux::common::Status sendGetSupportedDevices(uint32_t halReference,
                                    std::shared_ptr<IQmiGetSupportedDevicesCallback> callback,
                                    void *userData = nullptr);

    telux::common::Status sendGetSupportedStreams(uint32_t halReference,
                                    std::shared_ptr<IQmiGetSupportedStreamsCallback> callback,
                                    void *userData = nullptr);

    telux::common::Status sendCreateStream(QmiStreamConfig config,
            std::shared_ptr<IQmiCreateStreamCallback> callback, void *userData = nullptr);

    telux::common::Status sendDeleteStream(uint32_t halReference, uint32_t streamReference,
                                           std::shared_ptr<IQmiDeleteStreamCallback> callback,
                                           void *userData = nullptr);

    telux::common::Status sendStreamSetDevice(uint32_t streamReference,
                                              uint32_t numDevices, uint32_t *deviceType,
                                              std::shared_ptr<IQmiStreamSetDeviceCallback> callback,
                                              void *userData = nullptr);

    telux::common::Status sendStreamGetDevice(uint32_t streamReference,
                                              std::shared_ptr<IQmiStreamGetDeviceCallback> callback,
                                              void *userData = nullptr);

    telux::common::Status sendStreamSetVolume(uint32_t streamReference, uint32_t dir,
                                          uint32_t numChannels, uint32_t *channel, float *volume,
                                          std::shared_ptr<IQmiStreamSetVolumeCallback> callback,
                                          void *userData = nullptr);

    telux::common::Status sendStreamGetVolume(uint32_t streamReference, uint32_t dir,
                                              std::shared_ptr<IQmiStreamGetVolumeCallback> callback,
                                              void *userData = nullptr);

    telux::common::Status sendStreamSetMute(uint32_t streamReference, uint32_t dir, bool mute,
                                              std::shared_ptr<IQmiStreamSetMuteCallback> callback,
                                              void *userData = nullptr);

    telux::common::Status sendStreamGetMute(uint32_t streamReference, uint32_t dir,
                                              std::shared_ptr<IQmiStreamGetMuteCallback> callback,
                                              void *userData = nullptr);

    telux::common::Status sendStreamStart(uint32_t streamReference,
                                              std::shared_ptr<IQmiStreamStartCallback> callback,
                                              void *userData = nullptr);

    telux::common::Status sendStreamStop(uint32_t streamReference,
                                              std::shared_ptr<IQmiStreamStopCallback> callback,
                                              void *userData = nullptr);

    telux::common::Status sendDtmfCommand(uint32_t streamReference, audio_permit_v01 state, uint32_t
            lowFreq, uint32_t highFreq, uint32_t duration, uint16_t gain, uint32_t dir,
            std::shared_ptr<IQmiDtmfToneCallback> callback, void *userData = nullptr);

    telux::common::Status registerDtmfEvents(uint32_t streamReference, audio_permit_v01 state,
            std::shared_ptr<IQmiRegisterEventCallback> callback, void *userData);

    telux::common::Status sendStreamWrite(uint32_t streamReference, audio_stream_write_req_msg_v01*
                                      qmibuffer, std::shared_ptr<IQmiStreamWriteCallback> callback,
                                      void *userData = nullptr);

    telux::common::Status sendStreamRead(uint32_t streamReference, audio_stream_read_resp_msg_v01*
                                qmiBuffer, uint32_t size, std::shared_ptr<IQmiStreamReadCallback>
                                      callback, void *userData = nullptr);

    telux::common::Status sendStreamLoopback(uint32_t streamReference, audio_permit_v01 state,
            std::shared_ptr<IQmiLoopbackCallback> callback, void *userData = nullptr);

    telux::common::Status sendToneGenCommand(uint32_t streamReference, audio_permit_v01 state,
                        std::vector<uint16_t> freq, uint16_t duration, uint16_t gain,
                        std::shared_ptr<IQmiToneGenCallback> callback, void *userData = nullptr);

    telux::common::Status sendPlayStreamFlush(uint32_t streamReference,
            std::shared_ptr<IQmiPlayStreamFlushCallback> callback, void *userData = nullptr);

    telux::common::Status sendPlayStreamDrain(uint32_t streamReference,
            std::shared_ptr<IQmiPlayStreamDrainCallback> callback, void *userData = nullptr);

    telux::common::Status sendCreateTranscoder(uint32_t halReference,
            QmiTransCodeStreamConfig streamConfig,
            std::shared_ptr<IQmiCreateTranscoderCallback> callback, void *userData = nullptr);

    telux::common::Status sendTranscodeTearDown(uint32_t streamReference,
            std::shared_ptr<IQmiTranscoderTeardownCallback> callback, void *userData = nullptr);

    telux::common::Status sendGetCalInitStatus(uint32_t halReference,
        std::shared_ptr<IQmiGetCalInitStatusCallback> callback, void *userData = nullptr);

    void cleanup();

    telux::common::Status registerClientWithServer(audio_client_type_v01 clientType,
        uint32_t clientReference);

    AudioQmiClient();

    ~AudioQmiClient();

private:
    void notifyDtmfDetection(audio_dtmf_detection_ind_msg_v01 *indData);
    void notifyDrainComplete(const audio_stream_drain_ind_msg_v01 &indData);
    void notifyWriteReady(const audio_stream_write_ind_msg_v01 &indData);
    void notifyModemStatus(const audio_modem_status_ind_msg_v01 &indData);

    // deleting copy constructor, to implement AudioQmiClient as singleton
    AudioQmiClient(const AudioQmiClient &) = delete;
    // deleting assigning operator, to implement AudioQmiClient as singleton
    AudioQmiClient &operator=(const AudioQmiClient &) = delete;

    void handleLoadSubsystemResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetSupportedDevicesResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                       qmi_client_error_type transpErr,
                                       std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetSupportedStreamsResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                       qmi_client_error_type transpErr,
                                       std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleCreateStreamResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleDeleteStreamResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamSetDeviceResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamGetDeviceResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamSetVolumeResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamGetVolumeResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamSetMuteResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamGetMuteResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamStartResp(void *respCStruct, uint32_t respCStructLen, void *data,
                               qmi_client_error_type transpErr,
                               std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamStopResp(void *respCStruct, uint32_t respCStructLen, void *data,
                              qmi_client_error_type transpErr,
                              std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleDtmfResp(void *respCStruct, uint32_t respCStructLen, void *data,
                              qmi_client_error_type transpErr,
                              std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleRegisterEventResp(void *respCStruct, uint32_t respCStructLen, void *data,
                              qmi_client_error_type transpErr,
                              std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamWriteResp(void *respCStruct, uint32_t respCStructLen, void *data,
                              qmi_client_error_type transpErr,
                              std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleStreamReadResp(void *respCStruct, uint32_t respCStructLen, void *data,
                              qmi_client_error_type transpErr,
                              std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleLoopbackResp(void *respCStruct, uint32_t respCStructLen, void *data,
                              qmi_client_error_type transpErr,
                              std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleToneGenResp(void *respCStruct, uint32_t respCStructLen, void *data,
                            qmi_client_error_type transpErr,
                            std::weak_ptr<telux::common::ICommandCallback> callback);

    void handlePlayStreamFlushResp(void *respCStruct, uint32_t respCStructLen, void *data,
                            qmi_client_error_type transpErr,
                            std::weak_ptr<telux::common::ICommandCallback> callback);

    void handlePlayStreamDrainResp(void *respCStruct, uint32_t respCStructLen, void *data,
                            qmi_client_error_type transpErr,
                            std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleCreateTranscoderResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleTranscoderTeardownResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetCalInitStatusResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

    //Handler methods for indications
    int handleDtmfDetectionIndication(qmi_client_type userHandle, void *indBuf,
            unsigned int indBufLen);

    int handleDrainCompleteIndication(qmi_client_type userHandle, void *indBuf,
            unsigned int indBufLen);

    int handleWriteReadyIndication(qmi_client_type userHandle, void *indBuf,
            unsigned int indBufLen);

    int handleModemStatusIndication(qmi_client_type userHandle, void *indBuf,
            unsigned int indBufLen);

};

class IQmiAudioServiceListener : public IQmiListener {
public:
    /**
     * This function will be called when the qmi service status is changed.
     *
     * @param [in] status         service status
     */
    virtual void onServiceStatusChange(telux::common::ServiceStatus status) {}

    /**
     * This function will be called when the modem service status is changed.
     *
     * @param [in] status         service status
     */
    virtual void onModemStatusChange(telux::common::ServiceStatus status) {}

};

class IQmiAudioVoiceListener : public IQmiListener {
public:
    /**
     * This function will be called when the DTMF tone is detected
     *
     * @param [in] indData               Buffer holding the DTMF tone information
     */
    virtual void onDtmfToneDetection(audio_dtmf_detection_ind_msg_v01 *indData){};

};

class IQmiAudioPlayListener : public IQmiAudioServiceListener {
public:
    /**
     * This function will be called when the audio playback is stopped after playig pending buffers.
     *
     * @param [in] indData               buffer holding stream information
     */
    virtual void onPlayStoppedIndication(const audio_stream_drain_ind_msg_v01 & indData) {}

    /**
     * This function will be called when the audio pipeline is ready to accept new buffers to write
     * for compressed audio format playback.
     *
     * @param [in] indData               buffer holding stream information
     */
    virtual void onWriteReadyIndication(const audio_stream_write_ind_msg_v01 & indData) {}

};

class IQmiLoadSubsystemCallback : public telux::common::ICommandCallback {
public:
    virtual void onLoadSubsystemResponse(audio_load_subsystem_resp_msg_v01 *resp,
                                         int qmiError, void *userData) = 0;
};

class IQmiGetSupportedDevicesCallback : public telux::common::ICommandCallback {
public:
    virtual void onGetSupportedDevicesResponse(audio_get_supported_devices_resp_msg_v01 *resp,
                                               int qmiError, void *userData) = 0;
};

class IQmiGetSupportedStreamsCallback : public telux::common::ICommandCallback {
public:
    virtual void onGetSupportedStreamsResponse(audio_get_supported_streams_resp_msg_v01 *resp,
                                               int qmiError, void *userData) = 0;
};

class IQmiCreateStreamCallback : public telux::common::ICommandCallback {
public:
    virtual void onCreateStreamResponse(audio_create_stream_resp_msg_v01 *resp, int qmiError,
                                        void *userData) = 0;
};

class IQmiDeleteStreamCallback : public telux::common::ICommandCallback {
public:
    virtual void onDeleteStreamResponse(audio_close_stream_resp_msg_v01 *resp, int qmiError,
                                        void *userData) = 0;
};

class IQmiStreamSetDeviceCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamSetDeviceResponse(audio_stream_set_device_resp_msg_v01 *resp, int qmiError,
                                           void *userData) = 0;
};

class IQmiStreamGetDeviceCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamGetDeviceResponse(audio_stream_get_device_resp_msg_v01 *resp, int qmiError,
                                           void *userData) = 0;
};

class IQmiStreamSetVolumeCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamSetVolumeResponse(audio_stream_set_volume_resp_msg_v01 *resp, int qmiError,
                                           void *userData) = 0;
};

class IQmiStreamGetVolumeCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamGetVolumeResponse(audio_stream_get_volume_resp_msg_v01 *resp, int qmiError,
                                           void *userData) = 0;
};

class IQmiStreamSetMuteCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamSetMuteResponse(audio_stream_set_mute_resp_msg_v01 *resp, int qmiError,
                                         void *userData) = 0;
};

class IQmiStreamGetMuteCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamGetMuteResponse(audio_stream_get_mute_resp_msg_v01 *resp, int qmiError,
                                         void *userData) = 0;
};

class IQmiStreamStartCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamStartResponse(audio_stream_start_resp_msg_v01 *resp, int qmiError,
                                       void *userData) = 0;
};

class IQmiStreamStopCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamStopResponse(audio_stream_stop_resp_msg_v01 *resp, int qmiError,
                                      void *userData) = 0;
};

class IQmiDtmfToneCallback : public telux::common::ICommandCallback {
public:
    virtual void onDtmfResponse(audio_dtmf_generation_resp_msg_v01 *resp, int qmiError,
                                      void *userData) = 0;
};

class IQmiRegisterEventCallback : public telux::common::ICommandCallback {
public:
    virtual void onRegisterEventsResponse(audio_service_ind_register_resp_msg_v01* resp, int
            qmiError, void *userData) = 0;
};


class IQmiStreamWriteCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamWriteResponse(audio_stream_write_resp_msg_v01 *resp, int qmiError,
                                       void *userData) = 0;
};

class IQmiStreamReadCallback : public telux::common::ICommandCallback {
public:
    virtual void onStreamReadResponse(audio_stream_read_resp_msg_v01 *resp, int qmiError,
                                       void *userData) = 0;
};

class IQmiLoopbackCallback : public telux::common::ICommandCallback {
public:
    virtual void onLoopbackResponse(audio_stream_loopback_resp_msg_v01 *resp, int qmiError,
                                       void *userData) = 0;
};

class IQmiToneGenCallback : public telux::common::ICommandCallback {
public:
    virtual void onToneResponse(audio_tone_generation_resp_msg_v01 *resp, int qmiError,
                                    void *userData) = 0;
};

class IQmiPlayStreamFlushCallback : public telux::common::ICommandCallback {
public:
    virtual void onPlayStreamFlushResponse(audio_stream_flush_resp_msg_v01 *resp, int qmiError,
                                    void *userData) = 0;
};

class IQmiPlayStreamDrainCallback : public telux::common::ICommandCallback {
public:

    virtual void onPlayStreamDrainResponse(audio_stream_drain_resp_msg_v01 *resp, int qmiError,
                                    void *userData) = 0;
};

class IQmiCreateTranscoderCallback : public telux::common::ICommandCallback {
public:
    virtual void onCreateTranscoderResponse(audio_create_transcoder_resp_msg_v01 *resp,
                                         int qmiError, void *userData) = 0;
};

class IQmiTranscoderTeardownCallback : public telux::common::ICommandCallback {
public:
    virtual void onTranscoderTeardownResponse(audio_teardown_transcoder_resp_msg_v01 *resp,
            int qmiError, void *userData) = 0;
};

class IQmiGetCalInitStatusCallback : public telux::common::ICommandCallback {
public:
    virtual void onGetCalInitStatusResponse(audio_get_cal_init_status_resp_msg_v01 *resp,
            int qmiError, void *userData) = 0;
};

}  // end of namespace qmi
}  // end of namespace telux

#endif //AUDIOQMICLIENT_HPP
