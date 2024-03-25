/*
 *  Copyright (c) 2017-2020 The Linux Foundation. All rights reserved.
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
 * @file       SignalStrength.hpp
 * @brief      SignalStrength is a container class for obtaining the LTE, GSM, WCDMA, TDSCDMA and
 *             CDMA signal strength parameters.
 */
#ifndef SIGNALSTRENGTH_HPP
#define SIGNALSTRENGTH_HPP

#include <memory>

#define INVALID_SIGNAL_STRENGTH_VALUE 0x7FFFFFFF

namespace telux {

namespace tel {

/** @addtogroup telematics_phone
 * @{ */

// forward declarations
class LteSignalStrengthInfo;
class GsmSignalStrengthInfo;
class CdmaSignalStrengthInfo;
class WcdmaSignalStrengthInfo;
class TdscdmaSignalStrengthInfo;
class Nr5gSignalStrengthInfo;

/**
 * Defines all the signal levels that SignalStrength class can return
 * where level 1 is low and level 5 is high.
 */
enum class SignalStrengthLevel {
   LEVEL_1,
   LEVEL_2,
   LEVEL_3,
   LEVEL_4,
   LEVEL_5,
   LEVEL_UNKNOWN = -1,
};

/**
 * SignalStrength class provides access to LTE, GSM, CDMA, WCDMA, TDSCDMA signal strengths.
 */
class SignalStrength {
public:
   SignalStrength(std::shared_ptr<LteSignalStrengthInfo> lteSignalStrengthInfo,
                  std::shared_ptr<GsmSignalStrengthInfo> gsmSignalStrengthInfo,
                  std::shared_ptr<CdmaSignalStrengthInfo> cdmaSignalStrengthInfo,
                  std::shared_ptr<WcdmaSignalStrengthInfo> wcdmaSignalStrengthInfo,
                  std::shared_ptr<TdscdmaSignalStrengthInfo> tdscdmaSignalStrengthInfo,
                  std::shared_ptr<Nr5gSignalStrengthInfo> nr5gSignalStrengthInfo);
   /**
    * Gives LTE signal strength instance.
    *
    * @returns Pointer to LTE signal strength instance that can be used to get
    * lte dbm, signal level values.
    */
   std::shared_ptr<LteSignalStrengthInfo> getLteSignalStrength();

   /**
    * Gives GSM signal strength instance.
    *
    * @returns Pointer to GSM signal strength instance that can be used to get
    * GSM dbm, signal level values.
    */
   std::shared_ptr<GsmSignalStrengthInfo> getGsmSignalStrength();

   /**
    * Gives CDMA signal strength instance.
    *
    * @returns Pointer to CDMA signal strength instance that can be used to get
    * cdma/evdo dbm, signal level values.
    */
   std::shared_ptr<CdmaSignalStrengthInfo> getCdmaSignalStrength();

   /**
    * Gives WCDMA signal strength instance.
    *
    * @returns Pointer to WCDMA signal strength instance that can be used to get
    * WCDMA dbm, signal level values.
    */
   std::shared_ptr<WcdmaSignalStrengthInfo> getWcdmaSignalStrength();

   /**
    * Gives TDSWCDMA signal strength instance.
    *
    * @returns Pointer to TDSWCDMA signal strength instance that can be used to get
    * TDSCDMA RSCP value.
    *
    */
   std::shared_ptr<TdscdmaSignalStrengthInfo> getTdscdmaSignalStrength();

   /**
    * Gives 5G NR signal strength instance.
    *
    * @returns Pointer to 5G NR signal strength instance that can be used to get 5G NR dbm and snr
    *          values.
    *
    */
   std::shared_ptr<Nr5gSignalStrengthInfo> getNr5gSignalStrength();

private:
   std::shared_ptr<LteSignalStrengthInfo> lteSS_;
   std::shared_ptr<GsmSignalStrengthInfo> gsmSS_;
   std::shared_ptr<CdmaSignalStrengthInfo> cdmaSS_;
   std::shared_ptr<WcdmaSignalStrengthInfo> wcdmaSS_;
   std::shared_ptr<TdscdmaSignalStrengthInfo> tdscdmaSS_;
   std::shared_ptr<Nr5gSignalStrengthInfo> nr5gSS_;
};

/**
 * LTE signal strength class provides methods to get details of lte signals like
 * dbm, signal level, reference signal-to-noise ratio, channel quality indicator and
 * signal strength.
 */
class LteSignalStrengthInfo {
public:
   LteSignalStrengthInfo(int lteSignalStrength, int lteRsrp, int lteRsrq, int lteRssnr, int lteCqi,
                         int timingAdvance);
   /**
    * Get signal level in the range.
    *
    * @returns Signal levels indicates the quality of signal being received by
    * the device.
    */
   const SignalStrengthLevel getLevel() const;

   /**
    * Get the signal strength in dBm.
    * (Valid value range [-140, -44] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @returns LTE dBm value.
    */
   const int getDbm() const;

   /**
    * Get the LTE signal strength.
    * (Valid value range [0, 31] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @returns LTE signal strength.
    */
   const int getLteSignalStrength() const;

   /**
    * Get LTE reference signal receive quality in dB.
    * (Valid value range [-20, -3] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @returns LteRsrq.
    */
   const int getLteReferenceSignalReceiveQuality() const;

   /**
    * Get LTE reference signal signal-to-noise ratio, multiply by 0.1 to get SNR in dB.
    * (Valid value range [-200, +300] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    * (-200 = -20.0 dB, +300 = 30dB).
    *
    * @returns LteSnr.
    */
   const int getLteReferenceSignalSnr() const;

   /**
    * Get LTE channel quality indicator.
    * (Valid value range [0, 15] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @deprecated This API not being supported
    *
    * @returns LteCqI.
    */
   const int getLteChannelQualityIndicator() const;

   /**
    * Get the timing advance in micro seconds.
    * (Valid value range [0, 0x7FFFFFFE] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @deprecated This API not being supported
    *
    * @returns Timing advance value.
    *
    */
   const int getTimingAdvance() const;

private:
   int lteSignalStrength_;
   int lteRsrp_;
   int lteRsrq_;
   int lteRssnr_;
   int lteCqi_;
   int timingAdvance_;
};

/**
 * GSM signal strength provides methods to get GSM signal strength in dBm and GSM signal
 * level.
 */
class GsmSignalStrengthInfo {
public:
   GsmSignalStrengthInfo(int gsmSignalStrength, int gsmBitErrorRate, int timingAdvance);
   /**
    * Get signal level in the range.
    *
    * @returns Signal levels indicates the quality of signal being received by
    * the device.
    */
   const SignalStrengthLevel getLevel() const;

   /**
    * Get the signal strength in dBm.
    * (Valid value range [-113, -51] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    *
    * @returns GSM signal strength in dBm.
    */
   const int getDbm() const;

   /**
    * Get the GSM signal strength.
    * (Valid value range [0, 31] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    *
    * @returns GSM signal strength.
    */
   const int getGsmSignalStrength() const;

   /**
    * Get the GSM bit error rate.
    * (Valid value range [0, 7] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @deprecated This API not being supported
    *
    * @returns GSM bit error rate.
    */
   const int getGsmBitErrorRate() const;

   /**
    * Get the timing advance in bit periods . 1 bit period = 48/13 us
    * (Valid value range [0, 219] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @deprecated This API not being supported
    *
    * @returns timing advance.
    */
   const int getTimingAdvance();

private:
   int gsmSignalStrength_;
   int gsmBitErrorRate_;
   int timingAdvance_;
};

/**
 * CDMA signal strength provides methods to get details of CDMA and EVDO like signal strength
 * in dBm and signal level.
 */
class CdmaSignalStrengthInfo {
public:
   CdmaSignalStrengthInfo(int cdmaDbm, int cdmaEcio, int evdoDbm, int evdoEcio,
                          int evdoSignalNoiseRatio);
   /**
    * Get signal level in the range.
    *
    * @returns Signal levels indicates the quality of signal being received by
    * the device.
    */
   const SignalStrengthLevel getLevel() const;

   /**
    * Get the signal strength in dBm.
    *
    * @returns Minimum value of Evdo dBm and Cdma dBm.
    */
   const int getDbm() const;

   /**
    * Get the CDMA Ec/Io in dB.
    *
    * @returns CDMA Ec/Io.
    */
   const int getCdmaEcio() const;

   /**
    * Get the EVDO Ec/Io in dB.
    *
    * @returns EVDO Ec/Io.
    */
   const int getEvdoEcio() const;

   /**
    * Get the EVDO signal noise ratio.
    * (Valid value range [0, 8] and 8 is the highest signal to noise ratio.
    *
    * @returns EVDO SNR.
    */
   const int getEvdoSignalNoiseRatio() const;

private:
   // Helper functions used to calculate EVDO dBm and EVDO Level
   const int getEvdoDbm() const;
   const SignalStrengthLevel getEvdoLevel() const;
   // Helper functions used  to calculate CDMA dBm and CDMA Level
   const int getCdmaDbm() const;
   const SignalStrengthLevel getCdmaLevel() const;
   int cdmaDbm_;
   int cdmaEcio_;
   int evdoDbm_;
   int evdoEcio_;
   int evdoSignalNoiseRatio_;
};

/**
 * WCDMA signal strength provides methods to get WCDMA signal strength in dBm and WCDMA
 * signal level.
 */
class WcdmaSignalStrengthInfo {
public:
   WcdmaSignalStrengthInfo(int signalStrength, int bitErrorRate);
   /**
    * Get signal level in the range.
    *
    * @returns Signal levels indicates the quality of signal being received by
    * the device.
    *
    */
   const SignalStrengthLevel getLevel() const;

   /**
    * Get the signal strength in dBm.
    * (Valid value range [-113, -51] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @returns WCDMA signal strength in dBm.
    *
    */
   const int getDbm() const;

   /**
    * Get the WCDMA signal strength.
    * (Valid value range [0, 31] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @returns WCDMA signal strength.
    *
    */
   const int getSignalStrength() const;

   /**
    * Get the WCDMA bit error rate.
    * (Valid value range [0, 7] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @deprecated This API not being supported
    *
    * @returns WCDMA bit error rate.
    *
    */
   const int getBitErrorRate() const;

private:
   int signalStrength_;
   int bitErrorRate_;
};

/**
 * Tdscdma signal strength provides methods to get received signal code power.
 */
class TdscdmaSignalStrengthInfo {
public:
   TdscdmaSignalStrengthInfo(int rscp);

   /**
    * Get TdScdma received signal code power in dBm.
    *(Valid Range [-120,-25], and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    *
    * @returns TdScdma signal code power.
    *
    */
   const int getRscp() const;

private:
   int rscp_;
};

/**
 * 5G NR signal strength provides methods to get signal strength and signal-to-noise ratio.
 */
class Nr5gSignalStrengthInfo {
public:
   Nr5gSignalStrengthInfo(int rsrp, int rsrq, int rssnr);
   /**
    * Get signal level in the range.
    *
    * @returns Signal levels indicates the quality of signal being received by
    *          the device.
    */
   const SignalStrengthLevel getLevel() const;

   /**
    * Get the signal strength in dBm.
    * (Valid value range [-140, -44] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    * INVALID_SIGNAL_STRENGTH_VALUE indicates that modem is not in ENDC connected mode.
    *
    * @returns 5G NR dBm value.
    */
   const int getDbm() const;

   /**
    * Get 5G NR reference signal receive quality in dB.
    * (Valid value range [-20, -3] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    * INVALID_SIGNAL_STRENGTH_VALUE indicates that modem is not in ENDC connected mode.
    *
    * @returns 5G NR rsrq.
    */
   const int getReferenceSignalReceiveQuality() const;

   /**
    * Get 5G NR reference signal signal-to-noise ratio, multiply by 0.1 to get SNR in dB.
    * (Valid value range [-200, +300] and INVALID_SIGNAL_STRENGTH_VALUE i.e. unavailable).
    * (-200 = -20.0 dB, +300 = 30dB).
    * INVALID_SIGNAL_STRENGTH_VALUE indicates that modem is not in ENDC connected mode.
    *
    * @returns 5G NR signal-to-noise.
    */
   const int getReferenceSignalSnr() const;

private:
   int rsrp_;
   int rsrq_;
   int rssnr_;
};

/** @} */ /* end_addtogroup telematics_phone */

}  // End of namespace tel

}  // End of namespace telux

#endif
