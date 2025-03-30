#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h" // install https://github.com/pschatzmann/arduino-libhelix
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "WiFi.h"

URLStream url("It Burns When IP","i already told you all lowercase");
//I2SStream i2s; // final output of decoded stream
AudioBoardStream i2s(AudioKitEs8388V1); // final output of decoded stream
MP3DecoderHelix codec;
EncodedAudioStream dec(&i2s, &codec); // Decoding stream
StreamCopy copier(dec, url); // copy url to decoder

void setup(){
  Serial.begin(115200);
//  Serial.print("WiFi status: ");
//  WiFi.begin("It Burns When IP","i already told you all lowercase");
//  Serial.println(WiFi.status());
  
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  // setup i2s
  auto config = i2s.defaultConfig(TX_MODE);
  // you could define e.g your pins and change other settings
  //config.pin_ws = 10;
  //config.pin_bck = 11;
  //config.pin_data = 12;
  //config.mode = I2S_STD_FORMAT;
  i2s.begin(config);

  // setup I2S based on sampling rate provided by decoder
  dec.begin();

// mp3 radio
  if (!url.begin("http://stream.srg-ssr.ch/m/rsj/mp3_128", "audio/mp3")) {
    Serial.println("Error starting URL stream.");
  }

}

void loop(){
//  Serial.println(WiFi.status());
//  delay(500);
  copier.copy();
}
