#include <Arduino.h>
#line 1 "/Users/gabriel/Documents/Arduino/game/game.ino"
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
// #define FASTLED_RMT_BUILTIN_DRIVER 1
// #define FASTLED_FORCE_SOFTWARE_SPI
#include <FastLED.h>
// #define FULL_DMA_BUFFER
// #include "I2SClocklessLedDriver.h"


#define NUM_LEDS 300

const int chipSelect = PIN_AUDIO_KIT_SD_CARD_CS;
AudioBoardStream i2s(AudioKitEs8388V1);                  // final output of decoded stream
EncodedAudioStream decoder(&i2s, new MP3DecoderHelix()); // Decoding stream
StreamCopy copier;
File audioFile;
File songConfigFile;
CRGBArray<NUM_LEDS> leds;
// uint8_t leds[NUM_LEDS]; 
TaskHandle_t audioTask;
// I2SClocklessLedDriver driver;

typedef struct
{
  float time;
  float height_d;
  float height_f;
  float height_j;
  float height_k;
} piano_note_t;

float volume = 0.3; // Set initial volume
char *songName = "youbelongwithme";
bool playing = false;
float songLength;
char *gameMapping;
piano_note_t *notes;
int totalNotes;
long startTime;
long currentTime;
float secondsElapsed;

#line 47 "/Users/gabriel/Documents/Arduino/game/game.ino"
void actionKeyIncreaseVolume(bool active, int pin, void *ptr);
#line 61 "/Users/gabriel/Documents/Arduino/game/game.ino"
void actionKeyDecreaseVolume(bool active, int pin, void *ptr);
#line 75 "/Users/gabriel/Documents/Arduino/game/game.ino"
void setupActions();
#line 84 "/Users/gabriel/Documents/Arduino/game/game.ino"
void setupTiles();
#line 148 "/Users/gabriel/Documents/Arduino/game/game.ino"
void getMappings(const char *mapping);
#line 172 "/Users/gabriel/Documents/Arduino/game/game.ino"
piano_note_t jsonObjToNote(JsonObject obj);
#line 205 "/Users/gabriel/Documents/Arduino/game/game.ino"
void displayNotes();
#line 241 "/Users/gabriel/Documents/Arduino/game/game.ino"
void setup();
#line 270 "/Users/gabriel/Documents/Arduino/game/game.ino"
void loop();
#line 298 "/Users/gabriel/Documents/Arduino/game/game.ino"
void onBothButtonsPressed();
#line 304 "/Users/gabriel/Documents/Arduino/game/game.ino"
void setupLEDs(void *params);
#line 315 "/Users/gabriel/Documents/Arduino/game/game.ino"
void runLEDs();
#line 323 "/Users/gabriel/Documents/Arduino/game/game.ino"
void setupAudio(void *params);
#line 332 "/Users/gabriel/Documents/Arduino/game/game.ino"
void audioPlayer();
#line 47 "/Users/gabriel/Documents/Arduino/game/game.ino"
void actionKeyIncreaseVolume(bool active, int pin, void *ptr)
{
  if (active)
  {
    volume += 0.1;
    if (volume > 1.0)
      volume = 1.0; // Max volume
    i2s.setVolume(volume);
    Serial.print("Volume increased to: ");
    Serial.println(volume);
  }
}

// Action function for decreasing the volume
void actionKeyDecreaseVolume(bool active, int pin, void *ptr)
{
  if (active)
  {
    volume -= 0.1;
    if (volume < 0.0)
      volume = 0.0; // Min volume
    i2s.setVolume(volume);
    Serial.print("Volume decreased to: ");
    Serial.println(volume);
  }
}

// Setup actions for buttons
void setupActions()
{
  auto act_low = AudioActions::ActiveLow;

  // Assign button 1 to increase volume and button 2 to decrease volume
  // i2s.audioActions().add(i2s.getKey(2), actionKeyIncreaseVolume, NULL, act_low, NULL);
  // i2s.audioActions().add(i2s.getKey(3), actionKeyDecreaseVolume, NULL, act_low, NULL);
}

void setupTiles()
{
  Serial.printf("--- Setting up tiles with game %s ---\n", songName);
  // char songConfigFileName[strlen(songName) + 15];
  // snprintf(songConfigFileName, sizeof(songConfigFileName), "/songs/%s.json", songName);

  // Serial.printf("Opening song config file %s\n", songConfigFileName);
  // songConfigFile = SD.open(songConfigFileName);
  // if (!songConfigFile) {
  //   Serial.println("Failed to open song config file!");
  //   return;
  // }
  // String songConfigFileContents = songConfigFile.readString();
  // Serial.printf("Config file contents: %s\n", songConfigFileContents);

  // JsonDocument doc;
  // DeserializationError error = deserializeJson(doc, songConfigFileContents);
  // songConfigFile.close(); // Close file after reading

  // if (error) {
  //   Serial.print("JSON parsing failed: ");
  //   Serial.println(error.f_str());
  //   return;
  // }

  // const char* mapping;
  // const char* srcref;

  // mapping = doc["mapping"];
  // srcref = doc["srcref"];
  const char *mapping = "{\"version\":1,\"title\":\"\",\"startAt\":null,\"endAt\":221.99,\"keys\":4,\"difficulty\":4,\"length\":221.99,\"noteCount\":429,\"visualizerName\":\"\",\"clonedFrom\":\"\",\"mappings\":[{\"t\":1.142,\"k\":\"d\",\"h\":{\"d\":1.6219388302459716}},{\"t\":1.834,\"k\":\"k\",\"h\":{\"k\":2.3741809809265138}},{\"t\":2.486,\"k\":\"j\",\"h\":{\"j\":2.9583979752044676}},{\"t\":3.08,\"k\":\"f\"},{\"t\":3.582,\"k\":\"d\"},{\"t\":4.07,\"k\":\"k\"},{\"t\":4.509,\"k\":\"j\",\"h\":{\"j\":5.006307855041504}},{\"t\":5.478,\"k\":\"d\"},{\"t\":6.206,\"k\":\"f\"},{\"t\":6.71,\"k\":\"k\"},{\"t\":6.885,\"k\":\"d\"},{\"t\":7.382,\"k\":\"j\"},{\"t\":7.87,\"k\":\"k\"},{\"t\":8.247,\"k\":\"j\",\"h\":{\"j\":16.735240996185304}},{\"t\":9.254,\"k\":\"f\"},{\"t\":9.999,\"k\":\"d\"},{\"t\":10.974,\"k\":\"f\"},{\"t\":11.766,\"k\":\"d\"},{\"t\":12.742,\"k\":\"f\"},{\"t\":13.766,\"k\":\"d\"},{\"t\":14.494,\"k\":\"f\"},{\"t\":14.95,\"k\":\"d\"},{\"t\":15.399,\"k\":\"f\"},{\"t\":15.917,\"k\":\"f\"},{\"t\":16.173,\"k\":\"d\"},{\"t\":16.654,\"k\":\"k\"},{\"t\":16.886,\"k\":\"j\"},{\"t\":17.476,\"k\":\"j\"},{\"t\":17.75,\"k\":\"d\"},{\"t\":18.174,\"k\":\"k\"},{\"t\":18.67,\"k\":\"fj\"},{\"t\":18.902,\"k\":\"dk\"},{\"t\":19.366,\"k\":\"jf\"},{\"t\":20.11,\"k\":\"jf\",\"h\":{\"j\":21.19917103051758,\"f\":21.15717097520447}},{\"t\":21.136,\"k\":\"dk\",\"h\":{\"d\":22.08839794468689,\"k\":22.117397988555908}},{\"t\":22.077,\"k\":\"jf\",\"h\":{\"j\":22.901170956130983,\"f\":22.86917112969971}},{\"t\":23.054,\"k\":\"k\"},{\"t\":23.477,\"k\":\"f\"},{\"t\":23.694,\"k\":\"j\"},{\"t\":23.926,\"k\":\"d\"},{\"t\":24.447,\"k\":\"j\"},{\"t\":24.574,\"k\":\"f\"},{\"t\":24.836,\"k\":\"k\"},{\"t\":24.998,\"k\":\"d\"},{\"t\":25.357,\"k\":\"j\"},{\"t\":25.833,\"k\":\"f\"},{\"t\":26.078,\"k\":\"k\"},{\"t\":26.541,\"k\":\"j\"},{\"t\":27.046,\"k\":\"j\"},{\"t\":27.216,\"k\":\"f\"},{\"t\":27.454,\"k\":\"k\"},{\"t\":27.662,\"k\":\"d\"},{\"t\":27.966,\"k\":\"j\"},{\"t\":28.54,\"k\":\"j\"},{\"t\":29.07,\"k\":\"f\"},{\"t\":29.486,\"k\":\"k\"},{\"t\":29.766,\"k\":\"d\"},{\"t\":30.157,\"k\":\"j\"},{\"t\":30.614,\"k\":\"f\"},{\"t\":30.894,\"k\":\"k\"},{\"t\":31.309,\"k\":\"d\"},{\"t\":31.599,\"k\":\"j\"},{\"t\":32.253,\"k\":\"f\"},{\"t\":32.557,\"k\":\"j\"},{\"t\":32.988,\"k\":\"d\"},{\"t\":33.389,\"k\":\"k\"},{\"t\":33.633,\"k\":\"j\"},{\"t\":34.166,\"k\":\"jf\",\"h\":{\"j\":36.8136601411438,\"f\":36.82466005912781}},{\"t\":35.047,\"k\":\"dk\",\"h\":{\"d\":36.804660078201294,\"k\":36.817660089645386}},{\"t\":37.508,\"k\":\"j\"},{\"t\":37.821,\"k\":\"f\"},{\"t\":38.199,\"k\":\"d\"},{\"t\":38.621,\"k\":\"k\"},{\"t\":39.07,\"k\":\"fdjk\"},{\"t\":39.62,\"k\":\"j\"},{\"t\":40.021,\"k\":\"f\"},{\"t\":40.415,\"k\":\"k\"},{\"t\":40.925,\"k\":\"jkfd\"},{\"t\":41.461,\"k\":\"j\"},{\"t\":41.833,\"k\":\"f\"},{\"t\":42.293,\"k\":\"k\"},{\"t\":42.446,\"k\":\"d\"},{\"t\":42.773,\"k\":\"fdjk\"},{\"t\":43.34,\"k\":\"j\"},{\"t\":43.717,\"k\":\"f\"},{\"t\":44.006,\"k\":\"d\"},{\"t\":44.214,\"k\":\"k\"},{\"t\":44.653,\"k\":\"fdjk\"},{\"t\":45.19,\"k\":\"j\"},{\"t\":45.383,\"k\":\"f\"},{\"t\":45.582,\"k\":\"k\"},{\"t\":45.724,\"k\":\"d\"},{\"t\":46.093,\"k\":\"j\"},{\"t\":46.605,\"k\":\"j\"},{\"t\":46.783,\"k\":\"f\"},{\"t\":47.053,\"k\":\"k\"},{\"t\":47.247,\"k\":\"d\"},{\"t\":47.509,\"k\":\"j\"},{\"t\":47.725,\"k\":\"f\"},{\"t\":48.221,\"k\":\"j\"},{\"t\":48.453,\"k\":\"k\"},{\"t\":48.6,\"k\":\"d\"},{\"t\":49.109,\"k\":\"j\"},{\"t\":49.301,\"k\":\"f\"},{\"t\":49.534,\"k\":\"k\"},{\"t\":50.021,\"k\":\"k\"},{\"t\":50.247,\"k\":\"j\"},{\"t\":50.437,\"k\":\"f\"},{\"t\":50.701,\"k\":\"d\"},{\"t\":51.046,\"k\":\"j\"},{\"t\":51.166,\"k\":\"k\"},{\"t\":51.661,\"k\":\"j\"},{\"t\":51.861,\"k\":\"f\"},{\"t\":52.157,\"k\":\"d\"},{\"t\":52.383,\"k\":\"k\"},{\"t\":52.59,\"k\":\"j\",\"h\":{\"j\":55.90906614877319}},{\"t\":53.389,\"k\":\"f\",\"h\":{\"f\":55.87706608392334}},{\"t\":54.414,\"k\":\"dk\",\"h\":{\"d\":55.869066186920165,\"k\":55.91406602479553}},{\"t\":56.269,\"k\":\"j\",\"h\":{\"j\":59.78310190272522}},{\"t\":57.15,\"k\":\"f\",\"h\":{\"f\":59.75710211825562}},{\"t\":58.077,\"k\":\"d\",\"h\":{\"d\":59.7611020667572}},{\"t\":58.95,\"k\":\"k\",\"h\":{\"k\":59.78910194468689}},{\"t\":59.936,\"k\":\"fdjk\",\"h\":{\"f\":60.284811055313114,\"d\":60.26081112588501,\"j\":60.2888110038147,\"k\":60.29381111825562}},{\"t\":60.677,\"k\":\"j\"},{\"t\":60.784,\"k\":\"f\"},{\"t\":61.237,\"k\":\"dk\"},{\"t\":62.318,\"k\":\"j\"},{\"t\":62.533,\"k\":\"f\"},{\"t\":62.783,\"k\":\"k\"},{\"t\":63.2,\"k\":\"d\"},{\"t\":63.622,\"k\":\"j\",\"h\":{\"j\":66.940813917984}},{\"t\":64.317,\"k\":\"d\",\"h\":{\"d\":66.92481412397765}},{\"t\":64.91,\"k\":\"f\",\"h\":{\"f\":66.94481410490417}},{\"t\":65.79,\"k\":\"k\",\"h\":{\"k\":66.94881405340575}},{\"t\":67.309,\"k\":\"jf\"},{\"t\":68.23,\"k\":\"kd\"},{\"t\":69.136,\"k\":\"jf\"},{\"t\":70.037,\"k\":\"kd\"},{\"t\":70.973,\"k\":\"j\"},{\"t\":71.4,\"k\":\"f\"},{\"t\":71.685,\"k\":\"k\"},{\"t\":71.933,\"k\":\"d\"},{\"t\":72.433,\"k\":\"j\"},{\"t\":72.605,\"k\":\"f\"},{\"t\":73.077,\"k\":\"k\"},{\"t\":73.277,\"k\":\"d\"},{\"t\":73.83,\"k\":\"j\"},{\"t\":74.054,\"k\":\"f\"},{\"t\":74.421,\"k\":\"k\"},{\"t\":74.95,\"k\":\"d\"},{\"t\":75.199,\"k\":\"j\"},{\"t\":75.4,\"k\":\"f\"},{\"t\":75.693,\"k\":\"k\"},{\"t\":75.936,\"k\":\"d\"},{\"t\":76.189,\"k\":\"j\"},{\"t\":76.653,\"k\":\"j\"},{\"t\":77.022,\"k\":\"k\"},{\"t\":77.589,\"k\":\"d\"},{\"t\":78.016,\"k\":\"j\"},{\"t\":78.816,\"k\":\"f\"},{\"t\":79.285,\"k\":\"k\"},{\"t\":79.749,\"k\":\"d\"},{\"t\":80.233,\"k\":\"j\"},{\"t\":80.733,\"k\":\"f\"},{\"t\":80.983,\"k\":\"k\"},{\"t\":81.217,\"k\":\"d\"},{\"t\":81.486,\"k\":\"j\"},{\"t\":81.886,\"k\":\"jf\",\"h\":{\"j\":82.83313310108947,\"f\":82.80413305722045}},{\"t\":83.109,\"k\":\"jf\"},{\"t\":83.574,\"k\":\"kd\"},{\"t\":84.013,\"k\":\"j\"},{\"t\":84.726,\"k\":\"f\"},{\"t\":85.725,\"k\":\"j\"},{\"t\":86.183,\"k\":\"d\"},{\"t\":86.406,\"k\":\"k\"},{\"t\":86.718,\"k\":\"f\"},{\"t\":87.132,\"k\":\"j\"},{\"t\":87.366,\"k\":\"d\"},{\"t\":87.67,\"k\":\"k\"},{\"t\":87.884,\"k\":\"f\"},{\"t\":88.317,\"k\":\"j\"},{\"t\":88.717,\"k\":\"k\"},{\"t\":89.417,\"k\":\"jf\",\"h\":{\"j\":90.05245296376037,\"f\":89.98245303051758}},{\"t\":90.367,\"k\":\"dk\",\"h\":{\"d\":90.96671601335144,\"k\":91.0007159332428}},{\"t\":91.293,\"k\":\"j\"},{\"t\":91.75,\"k\":\"f\"},{\"t\":91.967,\"k\":\"k\"},{\"t\":92.19,\"k\":\"d\"},{\"t\":92.533,\"k\":\"j\"},{\"t\":93.02,\"k\":\"j\",\"h\":{\"j\":93.85199483215332}},{\"t\":94.013,\"k\":\"f\",\"h\":{\"f\":94.77200508010864}},{\"t\":94.908,\"k\":\"k\",\"h\":{\"k\":96.3663881926422}},{\"t\":95.433,\"k\":\"d\",\"h\":{\"d\":96.292661917984}},{\"t\":95.937,\"k\":\"fj\"},{\"t\":96.82,\"k\":\"j\"},{\"t\":97.308,\"k\":\"f\"},{\"t\":97.526,\"k\":\"k\"},{\"t\":97.701,\"k\":\"d\"},{\"t\":97.892,\"k\":\"j\"},{\"t\":98.053,\"k\":\"f\"},{\"t\":98.269,\"k\":\"k\"},{\"t\":98.693,\"k\":\"j\"},{\"t\":99.188,\"k\":\"f\"},{\"t\":99.566,\"k\":\"d\"},{\"t\":100.277,\"k\":\"k\"},{\"t\":100.573,\"k\":\"j\"},{\"t\":100.988,\"k\":\"f\"},{\"t\":101.46,\"k\":\"d\"},{\"t\":101.893,\"k\":\"d\"},{\"t\":102.373,\"k\":\"k\"},{\"t\":102.837,\"k\":\"j\"},{\"t\":103.221,\"k\":\"k\"},{\"t\":103.709,\"k\":\"jkfd\"},{\"t\":104.221,\"k\":\"j\"},{\"t\":104.637,\"k\":\"f\"},{\"t\":105.076,\"k\":\"d\"},{\"t\":105.541,\"k\":\"fjdk\"},{\"t\":106.077,\"k\":\"k\"},{\"t\":106.525,\"k\":\"j\"},{\"t\":106.909,\"k\":\"f\"},{\"t\":107.4,\"k\":\"jfdk\"},{\"t\":107.984,\"k\":\"j\"},{\"t\":108.341,\"k\":\"d\"},{\"t\":108.805,\"k\":\"k\"},{\"t\":109.292,\"k\":\"f\"},{\"t\":109.588,\"k\":\"j\"},{\"t\":109.845,\"k\":\"d\"},{\"t\":110.037,\"k\":\"k\"},{\"t\":110.461,\"k\":\"j\"},{\"t\":110.973,\"k\":\"d\"},{\"t\":111.204,\"k\":\"j\"},{\"t\":111.416,\"k\":\"k\"},{\"t\":111.837,\"k\":\"f\"},{\"t\":112.033,\"k\":\"j\"},{\"t\":112.26,\"k\":\"j\",\"h\":{\"j\":112.7727110705719}},{\"t\":112.973,\"k\":\"f\"},{\"t\":113.069,\"k\":\"d\"},{\"t\":113.646,\"k\":\"j\"},{\"t\":113.861,\"k\":\"k\"},{\"t\":114.367,\"k\":\"j\",\"h\":{\"j\":115.80468409727479}},{\"t\":115.253,\"k\":\"f\",\"h\":{\"f\":118.96785498855591}},{\"t\":116.204,\"k\":\"k\"},{\"t\":117.173,\"k\":\"d\"},{\"t\":118.06,\"k\":\"j\"},{\"t\":118.509,\"k\":\"j\"},{\"t\":118.984,\"k\":\"k\",\"h\":{\"k\":122.34103699046325}},{\"t\":119.972,\"k\":\"f\"},{\"t\":120.836,\"k\":\"d\"},{\"t\":122.709,\"k\":\"k\",\"h\":{\"k\":125.23644392370605}},{\"t\":123.333,\"k\":\"j\",\"h\":{\"j\":125.18844406484985}},{\"t\":124.068,\"k\":\"f\",\"h\":{\"f\":125.10844390272521}},{\"t\":125.001,\"k\":\"d\"},{\"t\":125.351,\"k\":\"j\"},{\"t\":125.533,\"k\":\"f\"},{\"t\":125.933,\"k\":\"k\"},{\"t\":126.373,\"k\":\"jf\",\"h\":{\"j\":127.17311395994568,\"f\":127.15711392752075}},{\"t\":127.101,\"k\":\"dk\",\"h\":{\"d\":127.78846101716614,\"k\":127.81646089509583}},{\"t\":127.783,\"k\":\"jf\",\"h\":{\"j\":128.53296495040894,\"f\":128.60496497711182}},{\"t\":128.718,\"k\":\"k\"},{\"t\":129.077,\"k\":\"d\"},{\"t\":129.589,\"k\":\"j\"},{\"t\":130.093,\"k\":\"k\"},{\"t\":130.509,\"k\":\"d\"},{\"t\":130.834,\"k\":\"j\"},{\"t\":131.461,\"k\":\"f\"},{\"t\":131.709,\"k\":\"k\"},{\"t\":132.205,\"k\":\"j\"},{\"t\":132.389,\"k\":\"d\"},{\"t\":132.868,\"k\":\"j\"},{\"t\":133.334,\"k\":\"j\"},{\"t\":133.74,\"k\":\"k\"},{\"t\":134.201,\"k\":\"d\"},{\"t\":134.518,\"k\":\"j\"},{\"t\":135.14,\"k\":\"f\"},{\"t\":135.405,\"k\":\"k\"},{\"t\":135.869,\"k\":\"d\"},{\"t\":136.117,\"k\":\"j\"},{\"t\":136.584,\"k\":\"f\"},{\"t\":136.951,\"k\":\"k\"},{\"t\":137.493,\"k\":\"jf\",\"h\":{\"j\":138.87706609918212,\"f\":138.86906596376036}},{\"t\":138.173,\"k\":\"kd\",\"h\":{\"k\":138.8720659847412,\"d\":138.83306595040892}},{\"t\":138.966,\"k\":\"jf\",\"h\":{\"j\":139.6206129485016,\"f\":139.60461291607666}},{\"t\":139.768,\"k\":\"k\"},{\"t\":139.869,\"k\":\"d\"},{\"t\":140.183,\"k\":\"j\"},{\"t\":140.619,\"k\":\"f\"},{\"t\":141.218,\"k\":\"jf\",\"h\":{\"j\":143.33453208010863,\"f\":143.32453208964537}},{\"t\":141.832,\"k\":\"kd\",\"h\":{\"k\":143.30853205722045,\"d\":143.26053195994567}},{\"t\":143.551,\"k\":\"j\"},{\"t\":143.757,\"k\":\"f\"},{\"t\":144.016,\"k\":\"k\"},{\"t\":144.401,\"k\":\"d\"},{\"t\":144.653,\"k\":\"j\"},{\"t\":145.293,\"k\":\"k\"},{\"t\":146.157,\"k\":\"f\"},{\"t\":147.101,\"k\":\"d\"},{\"t\":148.018,\"k\":\"j\"},{\"t\":148.941,\"k\":\"k\"},{\"t\":149.853,\"k\":\"f\"},{\"t\":150.751,\"k\":\"d\"},{\"t\":151.685,\"k\":\"k\"},{\"t\":152.557,\"k\":\"f\"},{\"t\":153.551,\"k\":\"d\"},{\"t\":154.461,\"k\":\"k\"},{\"t\":155.373,\"k\":\"f\"},{\"t\":156.309,\"k\":\"d\"},{\"t\":157.293,\"k\":\"d\"},{\"t\":157.618,\"k\":\"j\",\"h\":{\"j\":159.16793402479553}},{\"t\":158.588,\"k\":\"d\"},{\"t\":158.901,\"k\":\"f\"},{\"t\":159.524,\"k\":\"k\",\"h\":{\"k\":166.66110697138976}},{\"t\":160.373,\"k\":\"j\",\"h\":{\"j\":166.65310707438658}},{\"t\":161.437,\"k\":\"f\"},{\"t\":161.885,\"k\":\"d\"},{\"t\":162.401,\"k\":\"f\"},{\"t\":163.26,\"k\":\"d\"},{\"t\":163.801,\"k\":\"f\"},{\"t\":164.253,\"k\":\"d\"},{\"t\":164.818,\"k\":\"f\"},{\"t\":165.277,\"k\":\"d\"},{\"t\":165.685,\"k\":\"f\"},{\"t\":166.068,\"k\":\"f\"},{\"t\":166.973,\"k\":\"fdjk\",\"h\":{\"f\":168.51775808392333,\"d\":168.51075787602232,\"j\":168.55075807629393,\"k\":168.52675790844725}},{\"t\":168.861,\"k\":\"jf\"},{\"t\":169.277,\"k\":\"dk\"},{\"t\":169.717,\"k\":\"jf\",\"h\":{\"j\":170.46067392370605,\"f\":170.4046739294281}},{\"t\":170.685,\"k\":\"k\"},{\"t\":170.951,\"k\":\"d\"},{\"t\":171.157,\"k\":\"j\"},{\"t\":171.351,\"k\":\"f\"},{\"t\":171.602,\"k\":\"j\",\"h\":{\"j\":171.90936082643128}},{\"t\":172.077,\"k\":\"k\"},{\"t\":172.285,\"k\":\"d\"},{\"t\":172.585,\"k\":\"j\"},{\"t\":172.785,\"k\":\"f\"},{\"t\":172.968,\"k\":\"k\"},{\"t\":173.402,\"k\":\"jf\",\"h\":{\"j\":175.8212341525879,\"f\":175.78523413923645}},{\"t\":174.405,\"k\":\"kd\",\"h\":{\"k\":175.85323397901917,\"d\":175.81723396566773}},{\"t\":176.277,\"k\":\"j\"},{\"t\":176.693,\"k\":\"f\"},{\"t\":177.093,\"k\":\"k\"},{\"t\":177.557,\"k\":\"d\"},{\"t\":178.061,\"k\":\"j\"},{\"t\":178.552,\"k\":\"f\"},{\"t\":178.957,\"k\":\"k\"},{\"t\":179.421,\"k\":\"d\"},{\"t\":179.934,\"k\":\"j\"},{\"t\":180.367,\"k\":\"f\"},{\"t\":180.767,\"k\":\"k\"},{\"t\":181.261,\"k\":\"d\"},{\"t\":181.629,\"k\":\"j\"},{\"t\":182.102,\"k\":\"f\"},{\"t\":182.309,\"k\":\"k\"},{\"t\":182.677,\"k\":\"j\"},{\"t\":183.541,\"k\":\"jkfd\",\"h\":{\"j\":184.156818,\"k\":184.15981802098082,\"f\":184.1638179694824,\"d\":184.15081795803832}},{\"t\":184.384,\"k\":\"jfkd\",\"h\":{\"j\":184.89308503623963,\"f\":184.8880849217987,\"k\":184.8970849847412,\"d\":184.87608483787537}},{\"t\":185.005,\"k\":\"fjkd\",\"h\":{\"f\":185.80227307629394,\"j\":185.80527309727478,\"k\":185.8082731182556,\"d\":185.81327299427795}},{\"t\":185.973,\"k\":\"j\"},{\"t\":186.189,\"k\":\"f\"},{\"t\":186.389,\"k\":\"k\"},{\"t\":186.773,\"k\":\"d\"},{\"t\":187.277,\"k\":\"jkfd\",\"h\":{\"j\":187.8849879485016,\"k\":187.88798796948242,\"f\":187.891987917984,\"d\":187.86813210299684}},{\"t\":188.069,\"k\":\"jfkd\",\"h\":{\"j\":188.52553200953673,\"f\":188.53053188555907,\"k\":188.5335319065399,\"d\":188.51953196757506}},{\"t\":188.67,\"k\":\"jkfd\",\"h\":{\"j\":189.39212001335144,\"k\":189.38512004386902,\"f\":189.40512002479554,\"d\":189.3891199923706}},{\"t\":189.694,\"k\":\"j\"},{\"t\":190.102,\"k\":\"d\"},{\"t\":190.357,\"k\":\"k\"},{\"t\":190.586,\"k\":\"j\"},{\"t\":191.054,\"k\":\"jfkd\",\"h\":{\"j\":191.51758800190734,\"f\":191.49358807247924,\"k\":191.52158795040893,\"d\":191.49758802098083}},{\"t\":191.751,\"k\":\"jfk\",\"h\":{\"j\":192.2852460514984,\"f\":192.26924601907348,\"k\":192.289246}},{\"t\":191.801,\"k\":\"d\",\"h\":{\"d\":192.2722460400543}},{\"t\":192.619,\"k\":\"j\"},{\"t\":193.069,\"k\":\"k\"},{\"t\":193.318,\"k\":\"d\"},{\"t\":193.462,\"k\":\"j\"},{\"t\":193.821,\"k\":\"j\"},{\"t\":194.184,\"k\":\"k\"},{\"t\":194.702,\"k\":\"j\"},{\"t\":195.141,\"k\":\"d\"},{\"t\":195.374,\"k\":\"k\"},{\"t\":195.974,\"k\":\"k\"},{\"t\":196.186,\"k\":\"j\"},{\"t\":196.406,\"k\":\"f\"},{\"t\":196.685,\"k\":\"d\"},{\"t\":197.006,\"k\":\"j\"},{\"t\":197.325,\"k\":\"k\"},{\"t\":197.885,\"k\":\"jf\"},{\"t\":198.385,\"k\":\"kd\",\"h\":{\"k\":199.56814103623964,\"d\":199.53514104386903}},{\"t\":199.062,\"k\":\"fj\",\"h\":{\"f\":199.5521410038147,\"j\":199.57214098474122}},{\"t\":199.737,\"k\":\"jf\",\"h\":{\"j\":200.40257902861023,\"f\":200.40557904959107}},{\"t\":200.648,\"k\":\"k\"},{\"t\":200.91,\"k\":\"d\"},{\"t\":201.202,\"k\":\"j\"},{\"t\":201.602,\"k\":\"f\"},{\"t\":202.07,\"k\":\"dk\",\"h\":{\"d\":202.6937299885559,\"k\":202.70973002098083}},{\"t\":202.854,\"k\":\"j\",\"h\":{\"j\":203.3570521335144}},{\"t\":203.44,\"k\":\"f\",\"h\":{\"f\":204.30289197901917}},{\"t\":204.43,\"k\":\"j\"},{\"t\":204.839,\"k\":\"d\"},{\"t\":205.253,\"k\":\"k\"},{\"t\":205.719,\"k\":\"jf\",\"h\":{\"j\":206.48869082643128,\"f\":206.47769090844727}},{\"t\":206.667,\"k\":\"kd\",\"h\":{\"k\":207.3357949961853,\"d\":207.26179487602235}},{\"t\":207.605,\"k\":\"j\"},{\"t\":208.005,\"k\":\"f\"},{\"t\":208.44,\"k\":\"k\"},{\"t\":208.893,\"k\":\"d\"},{\"t\":209.325,\"k\":\"jkfd\",\"h\":{\"j\":210.01933311253356,\"k\":210.02333306103515,\"f\":210.02633308201598,\"d\":209.9893331411438}},{\"t\":210.304,\"k\":\"jfkd\",\"h\":{\"j\":210.9037910038147,\"f\":210.88679080545043,\"k\":210.90679102479552,\"d\":210.88979082643127}},{\"t\":211.253,\"k\":\"j\"},{\"t\":211.677,\"k\":\"f\"},{\"t\":212.03,\"k\":\"d\"},{\"t\":212.541,\"k\":\"k\"},{\"t\":213.135,\"k\":\"jkfd\",\"h\":{\"j\":213.79813403242494,\"k\":213.78913396948244,\"f\":213.79313391798402,\"d\":213.76944908392335}},{\"t\":213.986,\"k\":\"jkfd\",\"h\":{\"j\":214.38512206866454,\"k\":214.38912201716613,\"f\":214.39212203814697,\"d\":214.36912203623962}},{\"t\":214.519,\"k\":\"jkdf\",\"h\":{\"j\":215.2199190705719,\"k\":215.22291909155274,\"d\":215.20591889318848,\"f\":215.225918874115}},{\"t\":215.44,\"k\":\"j\"},{\"t\":215.654,\"k\":\"f\"},{\"t\":215.958,\"k\":\"k\"},{\"t\":216.294,\"k\":\"d\"},{\"t\":216.773,\"k\":\"jkfd\",\"h\":{\"j\":217.3353449961853,\"k\":217.33834501716615,\"f\":217.34134503814698,\"d\":217.30934497329713}},{\"t\":217.569,\"k\":\"fjkd\",\"h\":{\"f\":217.9853651926422,\"j\":218.01936511253356,\"k\":218.02336506103515,\"d\":217.9893651411438}},{\"t\":218.141,\"k\":\"jkfd\",\"h\":{\"j\":218.893125125885,\"k\":218.89812500190735,\"f\":218.92512495231628,\"d\":218.93712503623962}},{\"t\":219.171,\"k\":\"j\"},{\"t\":219.358,\"k\":\"f\"},{\"t\":219.647,\"k\":\"d\"},{\"t\":220.094,\"k\":\"k\"},{\"t\":220.554,\"k\":\"jf\",\"h\":{\"j\":221.20313416212463,\"f\":221.05413399427795}},{\"t\":221.24,\"k\":\"j\",\"h\":{\"j\":221.99}},{\"t\":221.303,\"k\":\"f\",\"h\":{\"f\":221.99}}]}";
  const char *srcref = "VuNIsY6JdUw";
  float length = 221.99;
  int noteCount = 429;

  Serial.print("Mapping: ");
  Serial.println(mapping ? mapping : "Not found");

  Serial.print("SrcRef: ");
  Serial.println(srcref ? srcref : "Not found");

  if (!mapping || !srcref)
  {
    Serial.println("Returning because mapping/srcref are not set");
    return;
  }

  totalNotes = noteCount;
  songLength = length;
  getMappings(mapping);
  char songFileName[strlen(srcref) + 11];
  snprintf(songFileName, sizeof(songFileName), "/mp3s/%s.mp3", srcref);
  Serial.printf("Song config file name: %s\n", songFileName);

  audioFile = SD.open(songFileName);

  decoder.begin();
  copier.begin(decoder, audioFile);
  playing = true;
  startTime = millis();
  currentTime = millis();
  secondsElapsed = 0;
}

void getMappings(const char *mapping)
{
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, mapping);

  if (error)
  {
    Serial.print("JSON parsing of mapping failed: ");
    Serial.println(error.f_str());
    return;
  }

  notes = (piano_note_t *)malloc(sizeof(piano_note_t) * totalNotes);
  int i = 0;
  JsonArray nestedArr = doc["mappings"];
  for (JsonVariant v : nestedArr)
  {
    JsonObject obj = v.as<JsonObject>();
    piano_note_t n = jsonObjToNote(obj);
    notes[i++] = n;
    Serial.printf("Added note %d\n", i);
  }
}

piano_note_t jsonObjToNote(JsonObject obj)
{
  piano_note_t tempNote;
  tempNote.time = obj["t"];

  // Get the string that tells us which notes need to be set.
  String notesToSet = obj["k"];
  JsonObject h = obj["h"];

  if (notesToSet.indexOf('d') != -1)
  {
    // If "d" exists and is a float, use its value; otherwise, default to 0.5.
    tempNote.height_d = h["d"].is<float>() ? h["d"].as<float>() : 0.5;
  }

  if (notesToSet.indexOf('f') != -1)
  {
    tempNote.height_f = h["f"].is<float>() ? h["f"].as<float>() : 0.5;
  }

  if (notesToSet.indexOf('j') != -1)
  {
    tempNote.height_j = h["j"].is<float>() ? h["j"].as<float>() : 0.5;
  }

  if (notesToSet.indexOf('k') != -1)
  {
    tempNote.height_k = h["k"].is<float>() ? h["k"].as<float>() : 0.5;
  }

  return tempNote;
}

void displayNotes()
{
  // Clear the LED array for a fresh display.
  FastLED.clear();

  // Loop through all notes.
  for (int i = 0; i < totalNotes; i++)
  {
    // If this note is scheduled in the next 10 seconds, display it.
    if (notes[i].time >= secondsElapsed && notes[i].time < (secondsElapsed + 10.0))
    {
      // Calculate how far (in seconds) into the 10 second window this note is.
      float relativeTime = notes[i].time - secondsElapsed;

      // Map the relative time (0.0 to 10.0 seconds) to an LED index (0 to NUM_LEDS-1).
      int ledIndex = (int)(relativeTime * (NUM_LEDS / 10.0));

      // Safety clamp in case of rounding issues.
      if (ledIndex < 0)
        ledIndex = 0;
      if (ledIndex >= NUM_LEDS)
        ledIndex = NUM_LEDS - 1;

      // Convert the note's height_d value (assumed between 0 and 1) into an 8-bit brightness value.
      uint8_t brightness = (uint8_t)constrain(notes[i].height_d * 255, 0, 255);

      // Set the LED in the computed slot to a chosen color.
      // Here we use a green color using CHSV (Hue=96 corresponds to green, saturation full).
      leds[ledIndex] = CHSV(96, 255, brightness);
    }
  }

  // Send the updated LED data to the strip.
  FastLED.show();
}

void setup()
{
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  Serial.println("--- Piano Tiles starting! ---");
  // Set up button actions
  setupActions();
  // Setup audiokit before SD
  setupAudio((void*)0);

  // Setup file
  SD.begin(chipSelect);


  // FastLED.addLeds<NEOPIXEL, 21>(leds, NUM_LEDS);
  // FastLED.clear();
  // FastLED.show();
  setupTiles();
  xTaskCreatePinnedToCore(
      setupLEDs, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &audioTask,  /* Task handle. */
      0); /* Core where the task should run */
}

void loop()
{
  // Serial.print("Audio running on core: ");
  // Serial.println(xPortGetCoreID());
  if (playing)
  {
    copier.copy();
    currentTime = millis();
    secondsElapsed = (currentTime - startTime) / 1000.0;
    // Serial.printf("Seconds elapsed %9.6f\n", secondsElapsed);
    // displayNotes();
  }

  i2s.processActions(); // Process individual button actions

  // Read both button states
  bool button1State = !digitalRead(i2s.getKey(1)); // Assuming active-low
  bool button2State = !digitalRead(i2s.getKey(2)); // Assuming active-low

  // Check if both are pressed at the same time
  if (button1State && button2State)
  {
    Serial.println("Both buttons pressed!");
    onBothButtonsPressed(); // Call function for this action
  }
}

// Function to handle both buttons being pressed
void onBothButtonsPressed()
{
  Serial.println("Performing special action!");
  // Add any action you want here
}

void setupLEDs(void *params) {
  FastLED.addLeds<NEOPIXEL, 23>(leds, NUM_LEDS);
  FastLED.clear();
  // driver.initled((uint8_t*)leds,pins,NUMSTRIPS,NUM_LED_PER_STRIP,ORDER_GRBW);

  FastLED.show();
  // Serial.print("LED Task running on core: ");
  // Serial.println(xPortGetCoreID());
  runLEDs();
}

void runLEDs() {
  for (;;) {
    // Serial.print("LED Task running on core: ");
    // Serial.println(xPortGetCoreID());
    displayNotes();
  }
}

void setupAudio(void *params) {
  auto config = i2s.defaultConfig(TX_MODE);
  config.sd_active = true;
  i2s.begin(config);
  // Setup I2S based on sampling rate provided by decoder
  i2s.setVolume(volume); // Set initial volume
  // audioPlayer();
}

void audioPlayer() {
  for(;;){
    if(playing) {
      copier.copy();
    }
  }
}
