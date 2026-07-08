#include "audio.h"

Audio::Audio(HardwareSerial &serial, int rx, int tx)
  : mySerial(serial), mp3(serial) {
  rxPin = rx;
  txPin = tx;
}

void Audio::begin() {
  mySerial.begin(9600, SERIAL_8N1, rxPin, txPin);

  delay(1000);
  mp3.reset();
  delay(500);

  mp3.setVolume(30);  // 🔊 Max volume
  mp3.setLoopMode(MP3_LOOP_ONE_STOP); // 🔥 Important
}

// ===== PRIVATE =====
void Audio::playTrack(uint8_t track) {
  mp3.playFileByIndexNumber(track);
}

// ===== AUDIO FUNCTIONS =====
void Audio::playAlarm() {
  playTrack(1);
}

void Audio::playClick() {
  playTrack(2);
}

void Audio::playFunny() {
  playTrack(3);
}

void Audio::playScanning() {
  playTrack(4);
}

void Audio::playStartup() {
  playTrack(5);
}

void Audio::setVolume(uint8_t vol) {
  if (vol > 30) vol = 30;
  mp3.setVolume(vol);
}
