#pragma once

#include "CustomLR2021.h"
#include "RadioLibWrappers.h"

#ifndef USE_LR2021
#define USE_LR2021
#endif

class CustomLR2021Wrapper : public RadioLibWrapper {
public:
  CustomLR2021Wrapper(CustomLR2021& radio, mesh::MainBoard& board) : RadioLibWrapper(radio, board) { }

  bool isReceivingPacket() override {
    return ((CustomLR2021 *)_radio)->isReceiving();
  }

  float getCurrentRSSI() override {
    float rssi = -110;
    ((CustomLR2021 *)_radio)->getRssiInst(&rssi);
    return rssi;
  }

  void onSendFinished() override {
    RadioLibWrapper::onSendFinished();
    _radio->setPreambleLength(16);
  }

  float getLastRSSI() const override {
    return ((CustomLR2021 *)_radio)->getRSSI();
  }

  float getLastSNR() const override {
    return ((CustomLR2021 *)_radio)->getSNR();
  }

    float packetScore(float snr, int packet_len) override {
      int sf = ((CustomLR2021 *)_radio)->getSpreadingFactor();
      return packetScoreInt(snr, sf, packet_len);
    }

  void powerOff() override {
    ((CustomLR2021 *)_radio)->sleep(false, 0);
  }

  void doResetAGC() override {
    CustomLR2021* r = (CustomLR2021*)_radio;
    float freq = r->getFreqMHz();
    r->sleep(true, 0);
    r->standby(RADIOLIB_LR2021_STANDBY_RC, true);
    r->calibrate(RADIOLIB_LR2021_CALIBRATE_ALL);
    r->setFrequency(freq);
    r->setRxBoostedGainMode(RADIOLIB_LR2021_RX_BOOST_LF);
  }

  void setRxBoostedGainMode(bool en) override {
    ((CustomLR2021 *)_radio)->setRxBoostedGainMode(en ? RADIOLIB_LR2021_RX_BOOST_LF : 0);
  }

  bool getRxBoostedGainMode() const override {
    return ((CustomLR2021 *)_radio)->getRxBoostedGainMode();
  }
};
