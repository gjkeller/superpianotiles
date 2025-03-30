#include <SPI.h>
#include <SD.h>
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

const int chipSelect = PIN_AUDIO_KIT_SD_CARD_CS;
AudioBoardStream i2s(AudioKitEs8388V1); // final output of decoded stream
EncodedAudioStream decoder(&i2s, new MP3DecoderHelix()); // Decoding stream
StreamCopy copier; 
File audioFile;

float volume = 0.5; // Set initial volume

// Action function for increasing the volume
void actionKeyIncreaseVolume(bool active, int pin, void* ptr){
  if (active) {
    volume += 0.1;
    if (volume > 1.0) volume = 1.0; // Max volume
    i2s.setVolume(volume);
    Serial.print("Volume increased to: ");
    Serial.println(volume);
  }
}

// Action function for decreasing the volume
void actionKeyDecreaseVolume(bool active, int pin, void* ptr){
  if (active) {
    volume -= 0.1;
    if (volume < 0.0) volume = 0.0; // Min volume
    i2s.setVolume(volume);
    Serial.print("Volume decreased to: ");
    Serial.println(volume);
  }
}

// Setup actions for buttons
void setupActions(){
  auto act_low = AudioActions::ActiveLow;
  
  // Assign button 1 to increase volume and button 2 to decrease volume
  i2s.audioActions().add(i2s.getKey(1), actionKeyIncreaseVolume, NULL, act_low, NULL);
  i2s.audioActions().add(i2s.getKey(2), actionKeyDecreaseVolume, NULL, act_low, NULL);
}

void setup(){
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);  

  // Setup audiokit before SD
  auto config = i2s.defaultConfig(TX_MODE);
  config.sd_active = true;
  i2s.begin(config);

  // Setup file
  SD.begin(chipSelect);
  audioFile = SD.open("/song.mp3");

  // Setup I2S based on sampling rate provided by decoder
  i2s.setVolume(volume); // Set initial volume
  decoder.begin();

  // Begin copy
  copier.begin(decoder, audioFile);

  // Set up button actions
  setupActions();
}

void loop(){
  copier.copy();
  i2s.processActions(); // Process button actions
}
