/*
*  Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are
*  met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above
*      copyright notice, this list of conditions and the following
*      disclaimer in the documentation and/or other materials provided
*      with the distribution.
*    * Neither the name of The Linux Foundation nor the names of its
*      contributors may be used to endorse or promote products derived
*      from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
*  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
*  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
*  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
*  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
*  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
*  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
*  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * @file    AudioManager.hpp
 *
 * @brief   Audio Manager is a primary interface for audio operations. It provides
 *          APIs to manage Voice, Audio and Sound Cards.
 */

#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <future>
#include <memory>
#include <vector>

#include <telux/common/CommonDefines.hpp>
#include <telux/audio/AudioDefines.hpp>
#include <telux/audio/AudioListener.hpp>
#include <telux/audio/AudioTranscoder.hpp>

namespace telux {

namespace audio {
/** @addtogroup telematics_audio_stream
 * @{ */

class IAudioDevice;
class IAudioStream;
class IAudioVoiceStream;
class IAudioPlayStream;
class IAudioCaptureStream;
/**
 * @brief   Stream Buffer manages the buffer to be used for read and write operations on Audio
 *          Streams. For write operations, applications should request a stream buffer, populate
 *          it with the data and then pass it to the write operation and set the dataSize that is
 *          to be written to the stream. Similarly for read operations,the application should
 *          request a stream buffer and use that in the read operation.
 *          At the end of the read, the stream buffer will contain the data read. Once an operation
 *          (read/write) has completed, the stream buffer could be reused for a subsequent
 *          read/write operation, provided reset() API called on stream buffer between
 *          subsequent calls.
 *
 */
class IAudioBuffer {
public:
   /**
    * Returns the minimum size (in bytes) of data that caller needs to read/write
    * before calling a read/write operation on the stream.
    *
    * @returns    minimum size
    */
   virtual size_t getMinSize() = 0;

   /**
    * Returns the maximum size (in bytes) that the buffer can hold.
    *
    * @returns    maximum size
    */
   virtual size_t getMaxSize() = 0;

   /**
    * Gets the raw buffer that IStreamBuffer manages. Application should write in between(include)
    * of  getMinSize() to getMaxSize() number of bytes in this buffer. Application is not
    * responsible to free the raw buffer. It will be free'ed when the IStreamBuffer is destroyed.
    *
    * @returns    raw buffer
    */
   virtual uint8_t *getRawBuffer() = 0;

   /**
    * Gets the size (in bytes) of valid data present in the buffer.
    *
    *
    * @returns size of valid data in the buffer
    */
   virtual uint32_t getDataSize() = 0;

   /**
    * Sets the size (in bytes) of valid data present in the buffer.
    *
    *
    * @param size  size of valid data in the buffer
    */
   virtual void setDataSize(uint32_t size) = 0;

   /**
    * Reset all state and data of the buffer. This is to be called when reusing the same buffer
    * for multiple operations.
    *
    * @returns status   Status of the operation
    */
   virtual telux::common::Status reset() = 0;

   virtual ~IAudioBuffer() {};
};

class IStreamBuffer : virtual public IAudioBuffer {
public:

    virtual ~IStreamBuffer() {};
};

/**
 * This function is called with the response to getDevices API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] devices  Devices list.
 * @param [in] error    Return code which indicates whether the operation
 *                      succeeded or not.
 *                      @ref ErrorCode
 */
using GetDevicesResponseCb = std::function<void(std::vector<std::shared_ptr<IAudioDevice>> devices,
                                                telux::common::ErrorCode error)>;

/**
 * This function is called with the response to getStreamTypes API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] streamTypes  Stream type list.
 * @param [in] error        Return code which indicates whether the operation
 *                          succeeded or not.
 *                          @ref ErrorCode
 */
using GetStreamTypesResponseCb
   = std::function<void(std::vector<StreamType> streamTypes, telux::common::ErrorCode error)>;

/**
 * This function is called with the response to createStream API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] stream  Interface pointer of Stream created.
 *                     VOICE_CALL - Provides IAudioVoiceStream pointer
 *                     PLAY - Provides IAudioPlayStream pointer
 *                     CAPTURE - Provides IAudioCaptureStream pointer
 * @param [in] error   Return code which indicates whether the operation
 *                     succeeded or not.
 *                     @ref ErrorCode
 */
using CreateStreamResponseCb
   = std::function<void(std::shared_ptr<IAudioStream> &stream, telux::common::ErrorCode error)>;

/**
 * This function is called with the response to createTranscoder API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] transcoder  Interface pointer of transcoder.
 *
 * @param [in] error       Return code which indicates whether the operation succeeded or not.
 *                         @ref ErrorCode
 *
 */
using CreateTranscoderResponseCb = std::function<void(
        std::shared_ptr<ITranscoder> &transcoder, telux::common::ErrorCode error)>;

/**
 * This function is called with the response to deleteStream API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] error  Return code which indicates whether the operation
 *                    succeeded or not.
 *                    @ref ErrorCode
 */
using DeleteStreamResponseCb = std::function<void(telux::common::ErrorCode error)>;

/**
 * This function is called with the response to getCalibrationInitStatus API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] calInitStatus  State of calibration initialization.
 * @param [in] error          Return code which indicates whether the operation
 *                            succeeded or not.
 *                            @ref ErrorCode
 */
using GetCalInitStatusResponseCb
   = std::function<void(CalibrationInitStatus calInitStatus, telux::common::ErrorCode error)>;

/**
 * @brief   Audio Manager is a primary interface for audio operations. It provide
 *          APIs to manage Streams ( like voice, play, record etc) and sound cards.
 */
class IAudioManager {
public:
   /**
    * Checks the status of audio subsystems and returns the result.
    *
    * @returns    If true that means AudioManager is ready for performing audio operations.
    *
    * @deprecated Use getServiceStatus API
    */
   virtual bool isSubsystemReady() = 0;

   /**
    * This status indicates whether the object is in a usable state or not.
    *
    * @returns SERVICE_AVAILABLE    -  if audio manager is ready to use.
    *          SERVICE_UNAVAILABLE  -  if audio manager is temporarily unavailable to use.
    *          SERVICE_FAILED       -  if audio manager encountered an irrecoverable failure and
    *                                  can not be used.
    */
   virtual telux::common::ServiceStatus getServiceStatus() = 0;

   /**
    * Wait for Audio subsystem to be ready.
    *
    * @returns    A future that caller can wait on to be notified when audio
    *             subsystem is ready.
    *
    * @deprecated Use InitResponseCb callback in factory API getAudioManager.
    */
   virtual std::future<bool> onSubsystemReady() = 0;

   /**
    * Get the list of supported audio devices, which are currently supported in the audio subsystem
    *
    * @param [in] callback    callback pointer to get the response of getDevices.
    *
    * @returns Status of request i.e. success or suitable status code.
    */
   virtual telux::common::Status getDevices(GetDevicesResponseCb callback = nullptr) = 0;

   /**
    * Get the list of supported audio streams types, which are currently supported in the audio
    * subsystem
    *
    * @param [in] callback    callback pointer to get the response of getStreamTypes.
    *
    * @returns Status of request i.e. success or suitable status code.
    */
   virtual telux::common::Status getStreamTypes(GetStreamTypesResponseCb callback = nullptr) = 0;

   /**
    * Creates the stream for audio operation
    *
    * @param [in] streamConfig    stream configuration.
    * @param [in] callback        callback pointer to get the response of createStream.
    *
    * @returns Status of request i.e. success or suitable status code.
    */
   virtual telux::common::Status createStream(StreamConfig streamConfig,
                                              CreateStreamResponseCb callback = nullptr)
      = 0;

   /**
    * Creates an instance of transcoder that can be used for transcoding operations. The supported
    * transcoding is real time transcoding, which takes the playback time of file for completing the
    * opearation.
    * Each instance returned can be used for single transcoding operation. The instance can not
    * be used for multiple transcoding operation.
    *
    * @param [in] input      configuration of input buffers that needs to be transcoded.
    * @param [in] output     configuration of transcoded output buffers.
    * @param [in] callback   callback pointer to get the response of createTranscoder.
    *
    * @returns Status of request i.e. success or suitable status code.
    *
    */
    virtual telux::common::Status createTranscoder(FormatInfo input, FormatInfo output,
            CreateTranscoderResponseCb callback) = 0;

   /**
    * Deletes the specified stream which was created before
    *
    * @param [in] stream      reference to stream to be deleted.
    * @param [in] callback    callback pointer to get the response of deleteStream.
    *
    * @returns Status of request i.e. success or suitable status code.
    */
   virtual telux::common::Status deleteStream(std::shared_ptr<IAudioStream> stream,
                                              DeleteStreamResponseCb callback = nullptr)
      = 0;

   /**
    * Register a listener to get notified when service status changes.
    *
    * @param [in] listener     Pointer of IServiceListener object that processes the notification
    *
    * @returns Status of registerListener i.e success or suitable status code.
    *
    */
   virtual telux::common::Status registerListener(std::weak_ptr<IAudioListener> listener) = 0;

   /**
    * Remove a previously registered listener.
    *
    * @param [in] listener Previously registered IServiceListener that needs to be removed
    *
    * @returns Status of deRegisterListener, success or suitable status code
    *
    */
   virtual telux::common::Status deRegisterListener(std::weak_ptr<IAudioListener> listener) = 0;

   /**
    * Get calibration status. Returns whether audio subsystem was able to successfully initialize
    * calibration in system. Calibration init status is available after the initialization
    * of the audio subsystem or after re-intialization of audio subsytem in case of sub system
    * restart is triggered.
    *
    *  @param [in] callback    callback pointer to get the response of getCalibrationInitStatus.
    *
    * @returns @ref Status of getCalibrationInitStatus, success or suitable status code.
    *
    * @note    Eval: This is a new API and is being evaluated.It is subject to change
    *          and could break backwards compatibility.
    */
   virtual telux::common::Status getCalibrationInitStatus(GetCalInitStatusResponseCb callback) = 0;

   virtual ~IAudioManager() {};
};

/**
 * @brief   Audio device and it's characteristics like Direction (Sink or Source), type
 */
class IAudioDevice {
public:
   /**
    * Get the type of Device (i.e SPEAKER, MIC etc)
    *
    * @returns    DeviceType
    */
   virtual DeviceType getType() = 0;

   /**
    * Provide direction of device whether is Sink for audio data ( RX i.e. speaker, etc)
    * or Source for audio data ( TX i.e. mic, etc)
    *
    * @returns    DeviceDirection
    */
   virtual DeviceDirection getDirection() = 0;

   virtual ~IAudioDevice() {};

 };

/**
 * This function is called with the response to stream getDevice API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] devices   Devices list.
 * @param [in] error     Return code which indicates whether the operation
 *                       succeeded or not.
 *                       @ref ErrorCode
 */
using GetStreamDeviceResponseCb
   = std::function<void(std::vector<DeviceType> devices, telux::common::ErrorCode error)>;

/**
 * This function is called with the response to stream getVolume API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] volume   stream volume details.
 * @param [in] error    Return code which indicates whether the operation
 *                      succeeded or not.
 *                      @ref ErrorCode
 */
using GetStreamVolumeResponseCb
   = std::function<void(StreamVolume volume, telux::common::ErrorCode error)>;

/**
 * This function is called with the response to stream getMute API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] mute   stream mute details.
 * @param [in] error  Return code which indicates whether the operation
 *                    succeeded or not.
 *                    @ref ErrorCode
 */
using GetStreamMuteResponseCb
   = std::function<void(StreamMute mute, telux::common::ErrorCode error)>;

/**
 * @brief   IAudioStream represents single audio stream with base properties
 */
class IAudioStream {
public:
   /**
    * Get the stream type like VOICE, PLAY, CAPTURE
    *
    * @returns    StreamType
    */
   virtual StreamType getType() = 0;

   /**
    * Set Device of audio stream
    *
    * @param [in] devices     Devices list.
    * @param [in] callback    callback to get the response of setDevice.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status setDevice(std::vector<DeviceType> devices,
                                           telux::common::ResponseCallback callback = nullptr)
      = 0;

   /**
    * Get Device of audio stream
    *
    * @param [in] callback    callback to get the response of getDevice
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status getDevice(GetStreamDeviceResponseCb callback = nullptr) = 0;

   /**
    * Set Volume of audio stream. Application needs to provide direction of the stream. Currently
    * TX direction of @ref VOICE_CALL is not supported.
    *
    * @param [in] volume     volume setting per channel for direction.
    * @param [in] callback   callback to get the response of setVolume.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status setVolume(StreamVolume volume,
                                           telux::common::ResponseCallback callback = nullptr)
      = 0;

   /**
    * Get Volume of audio stream. Application needs to provide direction of the stream. Currently
    * TX direction of @ref VOICE_CALL is not supported.
    *
    * @param [in] dir         Stream Direction to query volume details.
    * @param [in] callback    callback to get the response of getVolume.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status getVolume(StreamDirection dir,
                                           GetStreamVolumeResponseCb callback = nullptr)
      = 0;

   /**
    * Set Mute of audio stream
    *
    * @param [in] mute        mute setting for direction.
    * @param [in] callback    callback to know the status of the request.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status setMute(StreamMute mute,
                                         telux::common::ResponseCallback callback = nullptr)
      = 0;

   /**
    * Get Mute of audio stream
    *
    * @param [in] dir         Stream Direction to query mute details.
    * @param [in] callback    callback to get the response of getMute.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status getMute(StreamDirection dir,
                                         GetStreamMuteResponseCb callback = nullptr) = 0;

   virtual ~IAudioStream() {};
};

/**
 * @brief   IAudioVoiceStream represents single voice stream
 */
class IAudioVoiceStream : virtual public IAudioStream {
public:

   /**
    * Starts audio stream
    *
    * @param [in] callback    callback to get the response of startAudio.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status startAudio(telux::common::ResponseCallback callback = nullptr) = 0;

   /**
    * Stops audio stream
    *
    * @param [in] callback    callback to get the response of stopAudio.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status stopAudio(telux::common::ResponseCallback callback = nullptr) = 0;

   /**
    * Plays in-band DTMF tone on the active voice stream. This API supports DTMF tone playback on
    * local device on the RX path, for playing DTMF on the TX path so that it is heard on far end
    * use @ref telux::tel::ICall::startDtmfTone API.
    *
    * @param [in] dtmfTone     DTMF tone properties
    *        [in] duration     Duration (in milliseconds) for which the tone needs to be played. The
    *                          constant infiniteDtmfDuration(=0xFFFF) represents infinite duration.
    *        [in] gain         DTMF tone gain
    *        [in] callback     callback to get the response of playDtmfTone.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status playDtmfTone(DtmfTone dtmfTone, uint16_t duration, uint16_t gain,
                    telux::common::ResponseCallback callback = nullptr) = 0;

   /**
    * Stops the DTMF tone which is being played (i.e duration not expired) on the active voice
    * stream
    *
    * @param [in] direction   Direction associated with the DTMF tone
    * @      [in] callback    callback to get the response of stopDtmfTone.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status stopDtmfTone(StreamDirection direction,
                    telux::common::ResponseCallback callback = nullptr) = 0;

   /**
    * Register a listener to get notified when a DTMF tone is detected in the active voice stream
    *
    * @param [in] listener     Pointer of IVoiceListener object that processes the notification
    *        [in] callback     callback to get the response of registerListener
    *
    * @returns Status of registerListener i.e success or suitable status code.
    */
   virtual telux::common::Status registerListener(std::weak_ptr<IVoiceListener> listener,
                    telux::common::ResponseCallback callback = nullptr) = 0;

   /**
    * Remove a previously registered listener.
    *
    * @param [in] listener Previously registered IVoiceListener that needs to be removed
    *
    * @returns Status of deRegisterListener, success or suitable status code
    */
   virtual telux::common::Status deRegisterListener(std::weak_ptr<IVoiceListener> listener) = 0;

   virtual ~IAudioVoiceStream() {};
};

/**
 * This function is called with the response to IAudioPlayStream::write().
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] buffer       Buffer that was used for the write operation. Application could call
 *                          IStreamBuffer::reset() and reuse this buffer for subsequent write
 *                          operations on the same stream.
 *
 * @param [in] bytesWritten Return how many bytes are written to the stream.
 *
 * @param [in] error        Return code which indicates whether the operation
 *                          succeeded or not.
 *                          @ref ErrorCode
 */
using WriteResponseCb
    = std::function<void(std::shared_ptr<IStreamBuffer> buffer, uint32_t bytesWritten,
                                                            telux::common::ErrorCode error)>;

/**
 * @brief   IAudioPlayStream represents single audio playback stream
 */
class IAudioPlayStream : virtual public IAudioStream {
public:

   /**
    * Get an Audio StreamBuffer to be used for playback operations
    *
    * @returns            an Audio Buffer or a nullptr in case of error
    */
    virtual std::shared_ptr<IStreamBuffer> getStreamBuffer() = 0;

   /**
    * Write Samples\Frames to audio stream. First write starts playback operation.
    *
    * Write in case of compressed audio format maintains a pipeline, if the callback returns with
    * same number of bytes written as requested and no error occured, user can send next buffer.
    * If the number of bytes returned are not equal to the requested write size, then need to resend
    * the buffer again from the leftover offset after waiting for the @onReadyForWrite() event.
    * Once the last buffer is sent and the playback operation is complete, delete the playback
    * stream to avoid receiving silent packets on RX path.
    *
    * @param [in] buffer       stream buffer for write.
    * @param [in] callback     callback to get the response of write.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
    virtual telux::common::Status write(std::shared_ptr<IStreamBuffer> buffer,
                    WriteResponseCb callback = nullptr) = 0;

   /**
     * This API is to be used to stop playback. It is applicable only for compressed
     * audio format playback.
     *
     * @param [in] callback      callback to get the response of stopAudio.
     * @param [in] stopType      it specifies type of stop for stopping audio playback.
     *
     * @returns Status of the request i.e. success or suitable status code.
     *
     */
    virtual telux::common::Status stopAudio(StopType stopType,
                    telux::common::ResponseCallback callback = nullptr) = 0;

   /**
    * Register a listener to get notified for events of Play Stream
    *
    * @param [in] listener     Pointer of IPlayListener object that processes the notification
    *        [in] callback     callback to get the response of registerListener
    *
    * @returns Status of registerListener i.e success or suitable status code.
    *
    */
    virtual telux::common::Status registerListener(std::weak_ptr<IPlayListener> listener) = 0;

   /**
    * Remove a previously registered listener.
    *
    * @param [in] listener Previously registered IPlayListener that needs to be removed
    *
    * @returns Status of deRegisterListener, success or suitable status code
    *
    */
    virtual telux::common::Status deRegisterListener(std::weak_ptr<IPlayListener> listener) = 0;

    virtual ~IAudioPlayStream() {};
};


/**
 * This function is called with the response to IAudioCaptureStream::read().
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] buffer Buffer that was used to capture the data from the read operation.
 *                    Applications could call IStreamBuffer::reset() and reuse this buffer for
 *                    subsequent read operations on the same stream. Also buffer.getDataSize()
 *                    will represent the number of bytes read.
 *
 * @param [in] error  Return code which indicates whether the operation
 *                    succeeded or not.
 *                    @ref ErrorCode
 */
using ReadResponseCb
    = std::function<void(std::shared_ptr<IStreamBuffer> buffer,
                                        telux::common::ErrorCode error)>;

/**
 * @brief   IAudioCaptureStream represents single audio capture stream
 */
class IAudioCaptureStream : virtual public IAudioStream {
public:

   /**
    * Get an Audio Stream Buffer to be used for capture operations
    *
    * @returns            an Audio Buffer or nullptr in case of failure
    */
   virtual std::shared_ptr<IStreamBuffer> getStreamBuffer() = 0;

   /**
    * Read Samples from audio stream. First read starts capture operation.
    *
    * @param [in] buffer       stream buffer for read.
    * @param [in] bytesToRead  specifying how many bytes to be read from stream.
    * @param [in] callback     callback to get the response of read.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status read(std::shared_ptr<IStreamBuffer> buffer, uint32_t bytesToRead,
                                      ReadResponseCb callback = nullptr) = 0;

   virtual ~IAudioCaptureStream() {};
};

/**
 * @brief   IAudioLoopbackStream represents audio loopback stream
 */
class IAudioLoopbackStream : virtual public IAudioStream {
public:

  /**
    * Start loopback between source and sink devices
    *
    * @param [in] callback     callback to get the response of start loopback.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status
            startLoopback(telux::common::ResponseCallback callback = nullptr) = 0;

   /**
    * Stop loopback between source and sink devices
    *
    * @param [in] callback     callback to get the response of stop loopback.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status
            stopLoopback(telux::common::ResponseCallback callback = nullptr) = 0;

   virtual ~IAudioLoopbackStream() {};
};

/**
 * @brief   IAudioToneGeneratorStream represents tone generator stream
 */
class IAudioToneGeneratorStream : virtual public IAudioStream {
public:

  /**
    * Play a tone on sink devices. As the duartion expires, the generated tone
    * terminates automatically.
    *
    * @param  [in] freq         Accepts the composition of frequencies (in Hz) to be played
    *                           such as single tone or dual tone. Any additional
    *                           frequencies provided will be ignored.
    * @param  [in] duration     Duration (in milliseconds) for which the tone needs to be played.
    *                           The constant infiniteToneDuration(=0xFFFF) represents infinte
    *                           duration.
    * @param  [in] gain         Tone Gain.
    * @param  [in] callback     callback to get the response of play tone.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status playTone(std::vector<uint16_t> freq, uint16_t duration,
               uint16_t gain, telux::common::ResponseCallback callback = nullptr) = 0;

   /**
    * Stops the tone which is being played (i.e duration not expired) on the active Tone generator
    * stream.
    *
    * @param [in] callback     callback to get the response of stop tone.
    *
    * @returns Status of the request i.e. success or suitable status code.
    */
   virtual telux::common::Status stopTone(telux::common::ResponseCallback callback = nullptr) = 0;

   virtual ~IAudioToneGeneratorStream() {};
};

/** @} */ /* end_addtogroup telematics_audio_stream */
}  // End of namespace audio

}  // End of namespace telux

#endif  // end of AUDIOMANAGER_HPP
