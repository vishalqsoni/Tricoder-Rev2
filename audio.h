#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <JQ6500_Serial.h>
#include <HardwareSerial.h>

class Audio {
  public:
    Audio(HardwareSerial &serial, int rx, int tx);

    void begin();

    // 🎵 Audio-based functions
    void playAlarm();
    void playClick();
    void playFunny();
    void playScanning();
    void playStartup();

    void setVolume(uint8_t vol);

  private:
    HardwareSerial &mySerial;
    JQ6500_Serial mp3;
    int rxPin, txPin;

    void playTrack(uint8_t track);
};

#endif
