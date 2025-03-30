# 1 "/Users/gabriel/Documents/Arduino/game/game.ino"
// #define I2C_SDA_PIN 22
// #define I2C_SCL_PIN 18
// #include <Adafruit_seesaw.h>
# 5 "/Users/gabriel/Documents/Arduino/game/game.ino" 2
# 6 "/Users/gabriel/Documents/Arduino/game/game.ino" 2
# 7 "/Users/gabriel/Documents/Arduino/game/game.ino" 2
# 8 "/Users/gabriel/Documents/Arduino/game/game.ino" 2
# 9 "/Users/gabriel/Documents/Arduino/game/game.ino" 2
# 10 "/Users/gabriel/Documents/Arduino/game/game.ino" 2
// #define FASTLED_RMT_BUILTIN_DRIVER 1
// #define FASTLED_FORCE_SOFTWARE_SPI
# 13 "/Users/gabriel/Documents/Arduino/game/game.ino" 2
// #define FULL_DMA_BUFFER
// #include "I2SClocklessLedDriver.h"
# 25 "/Users/gabriel/Documents/Arduino/game/game.ino"
const int chipSelect = 13;
AudioBoardStream i2s(AudioKitEs8388V1); // final output of decoded stream
EncodedAudioStream decoder(&i2s, new MP3DecoderHelix()); // Decoding stream
// Adafruit_seesaw ss;
StreamCopy copier;
File audioFile;
File songConfigFile;
CRGBArray<120> leds;
// uint8_t leds[NUM_LEDS];
TaskHandle_t audioTask;
// I2SClocklessLedDriver driver;
int correctButtonPresses = 0;
int incorrectButtonPresses = 0;
// You can customize the column colors here:
const CHSV colColors[4] = {
    CHSV(96, 255, 255), // Column for "d" (Greenish)
    CHSV(0, 255, 255), // Column for "f" (Red)
    CHSV(160, 255, 255), // Column for "j" (Blue-ish)
    CHSV(32, 255, 255) // Column for "k" (Orange)
};

typedef struct
{
  float time;
  float height_d;
  float height_f;
  float height_j;
  float height_k;
  CHSV color;
} piano_note_t;

float volume = 0.5; // Set initial volume
char *songName = "youbelongwithmeeasy";
bool playing = false;
float songLength;
char *gameMapping;
piano_note_t *notes;
int totalNotes;
long startTime;
long currentTime;
float secondsElapsed;

void actionKeyIncreaseVolume(bool active, int pin, void *ptr)
{
  if (active)
  {
    volume += 0.1;
    if (volume > 1.0)
      volume = 1.0; // Max volume
    i2s.setVolume(volume);
    Serial0.print("Volume increased to: ");
    Serial0.println(volume);
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
    Serial0.print("Volume decreased to: ");
    Serial0.println(volume);
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
  Serial0.printf("--- Setting up tiles with game %s ---\n", songName);
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
  const char *mapping = "{\"version\":1,\"title\":\"\",\"startAt\":null,\"endAt\":null,\"keys\":4,\"difficulty\":1,\"length\":224.82,\"noteCount\":349,\"visualizerName\":\"\",\"clonedFrom\":\"\",\"mappings\":[{\"t\":9.286,\"k\":\"d\"},{\"t\":12.425,\"k\":\"f\"},{\"t\":15.333,\"k\":\"j\"},{\"t\":17.267,\"k\":\"k\"},{\"t\":18.957,\"k\":\"f\"},{\"t\":19.785,\"k\":\"f\"},{\"t\":25.614,\"k\":\"d\"},{\"t\":28.868,\"k\":\"j\"},{\"t\":32.015,\"k\":\"d\"},{\"t\":32.205,\"k\":\"k\"},{\"t\":33.772,\"k\":\"d\",\"h\":{\"d\":34.73636401335144}},{\"t\":36.69,\"k\":\"k\",\"h\":{\"k\":38.032124908447265}},{\"t\":39.113,\"k\":\"f\",\"h\":{\"f\":40.41450008392334}},{\"t\":42.247,\"k\":\"j\",\"h\":{\"j\":43.47073504196167}},{\"t\":44.251,\"k\":\"d\",\"h\":{\"d\":45.48130982643127}},{\"t\":51.135,\"k\":\"k\",\"h\":{\"k\":52.39404503433227}},{\"t\":51.197,\"k\":\"d\",\"h\":{\"d\":52.44504491416931}},{\"t\":53.265,\"k\":\"k\",\"h\":{\"k\":53.59533594087219}},{\"t\":53.732,\"k\":\"k\",\"h\":{\"k\":54.95999795422363}},{\"t\":55.087,\"k\":\"k\",\"h\":{\"k\":57.646980091552734}},{\"t\":60.27,\"k\":\"d\"},{\"t\":60.564,\"k\":\"k\"},{\"t\":60.901,\"k\":\"k\"},{\"t\":61.282,\"k\":\"k\"},{\"t\":61.396,\"k\":\"d\"},{\"t\":61.603,\"k\":\"k\"},{\"t\":61.865,\"k\":\"d\"},{\"t\":62.13,\"k\":\"k\"},{\"t\":62.507,\"k\":\"d\"},{\"t\":62.74,\"k\":\"k\"},{\"t\":62.998,\"k\":\"d\"},{\"t\":63.189,\"k\":\"k\"},{\"t\":63.793,\"k\":\"d\"},{\"t\":63.964,\"k\":\"k\"},{\"t\":64.635,\"k\":\"j\"},{\"t\":65.298,\"k\":\"f\"},{\"t\":65.488,\"k\":\"j\"},{\"t\":65.733,\"k\":\"f\"},{\"t\":67.365,\"k\":\"j\"},{\"t\":67.49,\"k\":\"f\"},{\"t\":67.687,\"k\":\"j\"},{\"t\":69.342,\"k\":\"d\"},{\"t\":69.485,\"k\":\"k\"},{\"t\":70.484,\"k\":\"j\"},{\"t\":71.001,\"k\":\"d\"},{\"t\":72.117,\"k\":\"f\"},{\"t\":72.743,\"k\":\"d\"},{\"t\":73.337,\"k\":\"d\"},{\"t\":73.629,\"k\":\"d\"},{\"t\":73.865,\"k\":\"d\"},{\"t\":74.17,\"k\":\"d\",\"h\":{\"d\":75.59835706294251}},{\"t\":76.168,\"k\":\"j\",\"h\":{\"j\":79.56983505149842}},{\"t\":77.869,\"k\":\"d\",\"h\":{\"d\":79.56083498855591}},{\"t\":80.876,\"k\":\"d\"},{\"t\":81.51,\"k\":\"k\"},{\"t\":82.217,\"k\":\"j\"},{\"t\":83.604,\"k\":\"d\"},{\"t\":85.121,\"k\":\"d\"},{\"t\":85.419,\"k\":\"f\"},{\"t\":86.509,\"k\":\"k\"},{\"t\":87.053,\"k\":\"k\"},{\"t\":87.412,\"k\":\"k\"},{\"t\":87.606,\"k\":\"k\"},{\"t\":87.832,\"k\":\"k\"},{\"t\":88.045,\"k\":\"k\"},{\"t\":88.296,\"k\":\"k\"},{\"t\":89.762,\"k\":\"f\"},{\"t\":90.147,\"k\":\"d\"},{\"t\":92,\"k\":\"k\"},{\"t\":92.449,\"k\":\"j\"},{\"t\":93.086,\"k\":\"k\"},{\"t\":93.347,\"k\":\"k\"},{\"t\":93.748,\"k\":\"j\"},{\"t\":93.936,\"k\":\"j\"},{\"t\":94.229,\"k\":\"j\"},{\"t\":94.482,\"k\":\"j\"},{\"t\":94.616,\"k\":\"f\"},{\"t\":94.963,\"k\":\"j\"},{\"t\":95.362,\"k\":\"f\"},{\"t\":95.911,\"k\":\"f\"},{\"t\":96.092,\"k\":\"j\"},{\"t\":96.326,\"k\":\"f\"},{\"t\":96.515,\"k\":\"j\"},{\"t\":96.729,\"k\":\"f\"},{\"t\":96.893,\"k\":\"k\"},{\"t\":97.078,\"k\":\"f\"},{\"t\":97.267,\"k\":\"k\"},{\"t\":97.446,\"k\":\"f\"},{\"t\":97.65,\"k\":\"j\"},{\"t\":97.793,\"k\":\"f\"},{\"t\":98.003,\"k\":\"j\"},{\"t\":98.213,\"k\":\"f\"},{\"t\":98.364,\"k\":\"k\"},{\"t\":98.75,\"k\":\"f\"},{\"t\":99.136,\"k\":\"j\"},{\"t\":99.686,\"k\":\"d\"},{\"t\":100.148,\"k\":\"d\"},{\"t\":100.544,\"k\":\"d\"},{\"t\":100.923,\"k\":\"d\"},{\"t\":101.361,\"k\":\"d\"},{\"t\":101.835,\"k\":\"k\",\"h\":{\"k\":103.51463488746643}},{\"t\":103.764,\"k\":\"f\",\"h\":{\"f\":104.74087892179871}},{\"t\":106.078,\"k\":\"j\",\"h\":{\"j\":108.95916378446961}},{\"t\":109.286,\"k\":\"d\",\"h\":{\"d\":110.16422187792969}},{\"t\":110.092,\"k\":\"k\",\"h\":{\"k\":111.51699904577637}},{\"t\":111.571,\"k\":\"d\"},{\"t\":111.715,\"k\":\"k\",\"h\":{\"k\":112.0663579332428}},{\"t\":112.024,\"k\":\"d\",\"h\":{\"d\":112.34210492370606}},{\"t\":112.482,\"k\":\"k\"},{\"t\":112.684,\"k\":\"d\"},{\"t\":112.902,\"k\":\"j\",\"h\":{\"j\":113.38409218119811}},{\"t\":113.393,\"k\":\"f\",\"h\":{\"f\":113.71581616593933}},{\"t\":115.025,\"k\":\"k\",\"h\":{\"k\":115.44150289700318}},{\"t\":115.428,\"k\":\"d\"},{\"t\":115.632,\"k\":\"k\",\"h\":{\"k\":116.08103807057189}},{\"t\":116.377,\"k\":\"j\"},{\"t\":116.745,\"k\":\"j\"},{\"t\":116.931,\"k\":\"j\"},{\"t\":117.096,\"k\":\"j\"},{\"t\":117.258,\"k\":\"j\"},{\"t\":117.414,\"k\":\"j\"},{\"t\":117.745,\"k\":\"j\",\"h\":{\"j\":119.7414520553131}},{\"t\":119.995,\"k\":\"d\",\"h\":{\"d\":120.43039704768371}},{\"t\":120.364,\"k\":\"f\",\"h\":{\"f\":121.06434917929077}},{\"t\":122.95,\"k\":\"d\",\"h\":{\"d\":123.49254704768371}},{\"t\":123.246,\"k\":\"k\",\"h\":{\"k\":124.24471597138977}},{\"t\":124.405,\"k\":\"d\"},{\"t\":124.903,\"k\":\"d\"},{\"t\":125.485,\"k\":\"kf\",\"h\":{\"k\":125.96284880545043}},{\"t\":125.844,\"k\":\"f\"},{\"t\":126.079,\"k\":\"j\",\"h\":{\"j\":126.44314616403199}},{\"t\":126.312,\"k\":\"d\"},{\"t\":126.579,\"k\":\"k\"},{\"t\":126.761,\"k\":\"f\"},{\"t\":126.961,\"k\":\"j\"},{\"t\":127.194,\"k\":\"d\"},{\"t\":127.575,\"k\":\"d\"},{\"t\":128.009,\"k\":\"j\",\"h\":{\"j\":128.324096}},{\"t\":128.195,\"k\":\"f\",\"h\":{\"f\":128.52977705722046}},{\"t\":128.509,\"k\":\"j\"},{\"t\":128.783,\"k\":\"fk\"},{\"t\":129.045,\"k\":\"k\"},{\"t\":129.286,\"k\":\"d\",\"h\":{\"d\":129.63016516403198}},{\"t\":129.531,\"k\":\"k\"},{\"t\":129.744,\"k\":\"j\"},{\"t\":129.953,\"k\":\"d\"},{\"t\":130.13,\"k\":\"k\"},{\"t\":130.5,\"k\":\"kf\",\"h\":{\"f\":130.8910600858307}},{\"t\":130.763,\"k\":\"j\",\"h\":{\"j\":131.28472186839295}},{\"t\":131.447,\"k\":\"j\",\"h\":{\"j\":133.03359397329712}},{\"t\":133.297,\"k\":\"k\",\"h\":{\"k\":134.19916689891053}},{\"t\":134.121,\"k\":\"d\",\"h\":{\"d\":134.87569801335144}},{\"t\":134.992,\"k\":\"f\",\"h\":{\"f\":135.53196601335145}},{\"t\":135.55,\"k\":\"j\"},{\"t\":136.027,\"k\":\"j\"},{\"t\":136.484,\"k\":\"k\",\"h\":{\"k\":136.84375505340577}},{\"t\":137.061,\"k\":\"f\",\"h\":{\"f\":137.73667000953674}},{\"t\":138.375,\"k\":\"d\",\"h\":{\"d\":138.79716491607667}},{\"t\":138.732,\"k\":\"k\",\"h\":{\"k\":139.42555500572203}},{\"t\":139.231,\"k\":\"f\",\"h\":{\"f\":139.9238079408722}},{\"t\":139.712,\"k\":\"j\",\"h\":{\"j\":140.50523589318848}},{\"t\":141.71,\"k\":\"f\",\"h\":{\"f\":142.10464093515014}},{\"t\":141.994,\"k\":\"k\"},{\"t\":142.354,\"k\":\"dk\",\"h\":{\"k\":142.83133000190736}},{\"t\":142.933,\"k\":\"d\",\"h\":{\"d\":143.85835802098083}},{\"t\":143.811,\"k\":\"k\",\"h\":{\"k\":144.68766395994567}},{\"t\":143.945,\"k\":\"f\"},{\"t\":145.14,\"k\":\"j\",\"h\":{\"j\":145.58430395803833}},{\"t\":145.622,\"k\":\"f\"},{\"t\":145.808,\"k\":\"k\",\"h\":{\"k\":146.20557487220765}},{\"t\":146.09,\"k\":\"f\"},{\"t\":146.71,\"k\":\"d\"},{\"t\":147.091,\"k\":\"d\"},{\"t\":148.499,\"k\":\"k\"},{\"t\":148.744,\"k\":\"k\"},{\"t\":148.949,\"k\":\"k\"},{\"t\":149.149,\"k\":\"k\"},{\"t\":149.349,\"k\":\"k\"},{\"t\":149.558,\"k\":\"k\"},{\"t\":149.75,\"k\":\"k\"},{\"t\":149.944,\"k\":\"k\"},{\"t\":150.161,\"k\":\"k\"},{\"t\":150.596,\"k\":\"j\"},{\"t\":151.144,\"k\":\"f\",\"h\":{\"f\":151.8226101449585}},{\"t\":151.941,\"k\":\"d\"},{\"t\":152.341,\"k\":\"d\",\"h\":{\"d\":152.71926001716614}},{\"t\":152.686,\"k\":\"k\",\"h\":{\"k\":152.9948800076294}},{\"t\":152.89,\"k\":\"d\"},{\"t\":153.101,\"k\":\"k\"},{\"t\":153.275,\"k\":\"d\"},{\"t\":153.424,\"k\":\"k\"},{\"t\":153.611,\"k\":\"dk\"},{\"t\":153.861,\"k\":\"kd\"},{\"t\":154.091,\"k\":\"k\"},{\"t\":154.193,\"k\":\"d\"},{\"t\":154.339,\"k\":\"k\"},{\"t\":154.516,\"k\":\"k\"},{\"t\":154.574,\"k\":\"d\"},{\"t\":154.726,\"k\":\"k\"},{\"t\":154.897,\"k\":\"d\"},{\"t\":154.949,\"k\":\"k\"},{\"t\":155.128,\"k\":\"k\"},{\"t\":155.246,\"k\":\"d\",\"h\":{\"d\":157.0774719847412}},{\"t\":155.356,\"k\":\"k\",\"h\":{\"k\":157.0834717882843}},{\"t\":158.095,\"k\":\"kd\",\"h\":{\"k\":158.92853892179872,\"d\":158.93753898474122}},{\"t\":159.273,\"k\":\"kd\"},{\"t\":159.444,\"k\":\"k\"},{\"t\":159.5,\"k\":\"d\"},{\"t\":159.643,\"k\":\"k\"},{\"t\":159.743,\"k\":\"d\"},{\"t\":159.842,\"k\":\"k\"},{\"t\":159.975,\"k\":\"d\"},{\"t\":160.026,\"k\":\"k\"},{\"t\":160.211,\"k\":\"dk\"},{\"t\":160.443,\"k\":\"kd\"},{\"t\":160.66,\"k\":\"k\"},{\"t\":160.719,\"k\":\"d\"},{\"t\":160.894,\"k\":\"k\"},{\"t\":160.98,\"k\":\"d\"},{\"t\":161.112,\"k\":\"k\"},{\"t\":161.21,\"k\":\"d\"},{\"t\":161.327,\"k\":\"k\"},{\"t\":161.44,\"k\":\"d\"},{\"t\":161.519,\"k\":\"k\"},{\"t\":161.705,\"k\":\"dk\"},{\"t\":161.909,\"k\":\"k\"},{\"t\":161.975,\"k\":\"d\"},{\"t\":162.196,\"k\":\"j\"},{\"t\":162.325,\"k\":\"f\"},{\"t\":162.499,\"k\":\"j\"},{\"t\":162.699,\"k\":\"fj\"},{\"t\":162.949,\"k\":\"j\"},{\"t\":163.147,\"k\":\"j\"},{\"t\":163.339,\"k\":\"j\"},{\"t\":163.515,\"k\":\"j\"},{\"t\":163.705,\"k\":\"j\"},{\"t\":163.922,\"k\":\"j\"},{\"t\":164.135,\"k\":\"j\"},{\"t\":164.36,\"k\":\"j\"},{\"t\":164.56,\"k\":\"j\"},{\"t\":164.792,\"k\":\"fj\"},{\"t\":165.039,\"k\":\"f\"},{\"t\":165.108,\"k\":\"j\"},{\"t\":165.292,\"k\":\"fj\"},{\"t\":165.489,\"k\":\"fj\"},{\"t\":168.48,\"k\":\"jd\"},{\"t\":168.661,\"k\":\"j\"},{\"t\":168.714,\"k\":\"d\"},{\"t\":168.855,\"k\":\"j\"},{\"t\":168.924,\"k\":\"d\"},{\"t\":169.05,\"k\":\"j\"},{\"t\":169.166,\"k\":\"d\"},{\"t\":169.258,\"k\":\"j\"},{\"t\":169.405,\"k\":\"d\"},{\"t\":169.479,\"k\":\"j\"},{\"t\":169.656,\"k\":\"dj\"},{\"t\":169.9,\"k\":\"jd\"},{\"t\":170.108,\"k\":\"j\"},{\"t\":170.176,\"k\":\"d\"},{\"t\":170.338,\"k\":\"j\"},{\"t\":170.426,\"k\":\"d\"},{\"t\":170.555,\"k\":\"j\"},{\"t\":170.725,\"k\":\"d\"},{\"t\":171.021,\"k\":\"j\"},{\"t\":171.276,\"k\":\"d\"},{\"t\":171.874,\"k\":\"f\"},{\"t\":173.398,\"k\":\"f\"},{\"t\":173.845,\"k\":\"f\"},{\"t\":174.21,\"k\":\"f\"},{\"t\":174.472,\"k\":\"k\"},{\"t\":175.306,\"k\":\"k\"},{\"t\":175.505,\"k\":\"k\"},{\"t\":176.992,\"k\":\"d\"},{\"t\":179.054,\"k\":\"f\"},{\"t\":180.892,\"k\":\"j\"},{\"t\":182.122,\"k\":\"k\"},{\"t\":182.711,\"k\":\"k\"},{\"t\":183.217,\"k\":\"k\"},{\"t\":184.007,\"k\":\"k\"},{\"t\":184.255,\"k\":\"k\"},{\"t\":184.52,\"k\":\"k\"},{\"t\":185.555,\"k\":\"k\"},{\"t\":185.728,\"k\":\"k\"},{\"t\":185.904,\"k\":\"k\"},{\"t\":186.061,\"k\":\"k\"},{\"t\":186.294,\"k\":\"k\"},{\"t\":186.479,\"k\":\"k\"},{\"t\":186.871,\"k\":\"k\"},{\"t\":187.041,\"k\":\"k\"},{\"t\":187.271,\"k\":\"k\"},{\"t\":187.486,\"k\":\"k\"},{\"t\":187.691,\"k\":\"k\"},{\"t\":187.906,\"k\":\"k\"},{\"t\":188.299,\"k\":\"f\"},{\"t\":190.495,\"k\":\"j\"},{\"t\":190.672,\"k\":\"j\"},{\"t\":190.826,\"k\":\"j\"},{\"t\":190.98,\"k\":\"j\",\"h\":{\"j\":191.55388691798402}},{\"t\":191.607,\"k\":\"j\",\"h\":{\"j\":192.3911270972748}},{\"t\":192.802,\"k\":\"f\"},{\"t\":193.455,\"k\":\"k\",\"h\":{\"k\":193.85534693133545}},{\"t\":193.871,\"k\":\"d\",\"h\":{\"d\":194.30681811253356}},{\"t\":194.27,\"k\":\"k\",\"h\":{\"k\":194.9047339923706}},{\"t\":194.845,\"k\":\"d\"},{\"t\":195.007,\"k\":\"k\",\"h\":{\"k\":195.386292}},{\"t\":195.309,\"k\":\"d\"},{\"t\":195.52,\"k\":\"k\"},{\"t\":195.737,\"k\":\"d\"},{\"t\":195.87,\"k\":\"j\",\"h\":{\"j\":196.353502043869}},{\"t\":196.259,\"k\":\"f\"},{\"t\":197.941,\"k\":\"d\",\"h\":{\"d\":198.63983219073486}},{\"t\":198.988,\"k\":\"d\"},{\"t\":199.272,\"k\":\"j\"},{\"t\":199.66,\"k\":\"k\"},{\"t\":200.24,\"k\":\"j\"},{\"t\":200.459,\"k\":\"k\"},{\"t\":200.642,\"k\":\"k\"},{\"t\":200.855,\"k\":\"k\"},{\"t\":201.059,\"k\":\"j\"},{\"t\":201.592,\"k\":\"f\"},{\"t\":201.785,\"k\":\"d\"},{\"t\":202.54,\"k\":\"d\"},{\"t\":202.662,\"k\":\"f\"},{\"t\":203.063,\"k\":\"j\"},{\"t\":203.24,\"k\":\"k\",\"h\":{\"k\":203.5572478703003}},{\"t\":204.612,\"k\":\"k\"},{\"t\":204.87,\"k\":\"j\"},{\"t\":205.346,\"k\":\"f\"},{\"t\":205.889,\"k\":\"d\",\"h\":{\"d\":207.96821301716614}},{\"t\":208.586,\"k\":\"k\"},{\"t\":208.911,\"k\":\"j\",\"h\":{\"j\":210.30360202288819}},{\"t\":210.669,\"k\":\"f\"},{\"t\":211.822,\"k\":\"d\",\"h\":{\"d\":212.47827597520447}},{\"t\":213.687,\"k\":\"d\"},{\"t\":213.882,\"k\":\"f\"},{\"t\":214.142,\"k\":\"f\"},{\"t\":215.102,\"k\":\"j\"},{\"t\":215.34,\"k\":\"j\"},{\"t\":215.545,\"k\":\"k\"},{\"t\":215.773,\"k\":\"k\"},{\"t\":216.821,\"k\":\"k\"},{\"t\":216.954,\"k\":\"j\"},{\"t\":217.352,\"k\":\"f\"},{\"t\":217.456,\"k\":\"d\"},{\"t\":217.787,\"k\":\"d\",\"h\":{\"d\":218.75808881307984}},{\"t\":218.737,\"k\":\"f\"},{\"t\":219.178,\"k\":\"j\"},{\"t\":219.319,\"k\":\"k\"},{\"t\":219.82,\"k\":\"kd\",\"h\":{\"k\":220.5681130267029,\"d\":220.64469489700318}}]}";
  const char *srcref = "a7xmtB_HWgE";
  float length = 224.82;
  int noteCount = 349;

  // Serial.print("Mapping: ");
  // Serial.println(mapping ? mapping : "Not found");

  // Serial.print("SrcRef: ");
  // Serial.println(srcref ? srcref : "Not found");

  if (!mapping || !srcref)
  {
    Serial0.println("Returning because mapping/srcref are not set");
    return;
  }

  totalNotes = noteCount;
  songLength = length;
  getMappings(mapping);
  for (int i = 0; i < totalNotes; i++)
  {
    piano_note_t note = notes[i];
    // printNote(note);
  }
  char songFileName[strlen(srcref) + 11];
  snprintf(songFileName, sizeof(songFileName), "/mp3s/%s.mp3", srcref);
  Serial0.printf("Song config file name: %s\n", songFileName);

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
    Serial0.print("JSON parsing of mapping failed: ");
    Serial0.println(error.f_str());
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
    // Serial.printf("Added note %d\n", i);
  }
}

piano_note_t jsonObjToNote(JsonObject obj)
{
  piano_note_t tempNote;
  // Serial.printf("Creating a note!\n");
  // printNote(tempNote);
  tempNote.time = obj["t"];

  // Get the string that tells us which notes need to be set.
  String notesToSet = obj["k"];
  JsonObject h = obj["h"];

  // Serial.printf("Notestoset=%s, d=%d, f=%d, j=%d, k=%d", notesToSet, notesToSet.indexOf('d'), notesToSet.indexOf('f'), notesToSet.indexOf('j'), notesToSet.indexOf('k'));
  if (notesToSet.indexOf('d') != -1)
  {
    // Serial.printf("inside of d\n");
    // If "d" exists and is a float, use its value; otherwise, default to 0.5.
    tempNote.height_d = h["d"].is<float>() ? h["d"].as<float>() - tempNote.time : 0.25;
  }
  else
  {
    tempNote.height_d = 0;
  }

  if (notesToSet.indexOf('f') != -1)
  {
    // Serial.printf("inside of f\n");
    tempNote.height_f = h["f"].is<float>() ? h["f"].as<float>() - tempNote.time : 0.25;
  }
  else
  {
    tempNote.height_f = 0;
  }

  if (notesToSet.indexOf('j') != -1)
  {
    // Serial.printf("inside of j\n");
    tempNote.height_j = h["j"].is<float>() ? h["j"].as<float>() - tempNote.time : 0.25;
  }
  else
  {
    tempNote.height_j = 0;
  }

  if (notesToSet.indexOf('k') != -1)
  {
    // Serial.printf("inside of k\n");
    tempNote.height_k = h["k"].is<float>() ? h["k"].as<float>() - tempNote.time : 0.25;
  }
  else
  {
    tempNote.height_k = 0;
  }

  // Serial.printf("Here's what the note looks like now!\n");
  // printNote(tempNote);
  return tempNote;
}

void displayNotes()
{
  // Clear the LED array each time for a fresh display.
  FastLED.clear();

  // For our 120 LED strip divided into 4 columns,
  // each column gets:
  int colSegment = 120 / 4; // e.g., 120/4 = 30 LEDs per column

  // Loop through all notes.
  for (int i = 0; i < totalNotes; i++)
  {
    // Check if the note has at least one positive height value in any column.
    if ((notes[i].height_d <= 0.0) && (notes[i].height_f <= 0.0) &&
        (notes[i].height_j <= 0.0) && (notes[i].height_k <= 0.0))
      continue;

    // We want to display a note if any portion of it occurs during the 10-second window.
    // That is, if noteEnd > secondsElapsed and noteStart < secondsElapsed+10.
    // We'll process each column separately.
    float noteStart = notes[i].time; // full note start time

    // Column 0 ("d")
    if (notes[i].height_d > 0.0)
    {
      float duration = notes[i].height_d; // duration in seconds
      float noteEnd = noteStart + duration;

      // Skip if the note (or its tail) is completely before or after our display window.
      if (noteEnd <= secondsElapsed || noteStart >= secondsElapsed + 10.0)
      {
        // not visible in the current window, so skip.
      }
      else
      {
        // Clip the start and end to our display window.
        float effectiveStart = (noteStart < secondsElapsed) ? secondsElapsed : noteStart;
        float effectiveEnd = (noteEnd > secondsElapsed + 10.0) ? (secondsElapsed + 10.0) : noteEnd;

        // Relative times in the window (0 to 10 sec)
        float relStart = effectiveStart - secondsElapsed;
        float relEnd = effectiveEnd - secondsElapsed;

        // Map these times to LED indices within our column.
        int ledStart = (int)(relStart * (colSegment / 10.0));
        int ledEnd = (int)(relEnd * (colSegment / 10.0));
        if (ledEnd <= ledStart)
        {
          ledEnd = ledStart + 1; // Ensure at least one LED is lit.
        }
        int baseIndex = 0 * colSegment; // Column 0 starts at LED 0.
        for (int led = baseIndex + ledStart; led < baseIndex + ledEnd && led < baseIndex + colSegment; led++)
        {
          leds[led] = colColors[0];
        }
      }
    }

    // Column 1 ("f")
    if (notes[i].height_f > 0.0)
    {
      float duration = notes[i].height_f;
      float noteEnd = noteStart + duration;
      if (noteEnd <= secondsElapsed || noteStart >= secondsElapsed + 10.0)
      {
        // Note not in window.
      }
      else
      {
        float effectiveStart = (noteStart < secondsElapsed) ? secondsElapsed : noteStart;
        float effectiveEnd = (noteEnd > secondsElapsed + 10.0) ? (secondsElapsed + 10.0) : noteEnd;

        float relStart = effectiveStart - secondsElapsed;
        float relEnd = effectiveEnd - secondsElapsed;

        int ledStart = (int)(relStart * (colSegment / 10.0));
        int ledEnd = (int)(relEnd * (colSegment / 10.0));
        if (ledEnd <= ledStart)
        {
          ledEnd = ledStart + 1;
        }
        int baseIndex = 1 * colSegment; // Column 1 base index.
        for (int led = baseIndex + ledStart; led < baseIndex + ledEnd && led < baseIndex + colSegment; led++)
        {
          leds[led] = colColors[1];
        }
      }
    }

    // Column 2 ("j")
    if (notes[i].height_j > 0.0)
    {
      float duration = notes[i].height_j;
      float noteEnd = noteStart + duration;
      if (noteEnd <= secondsElapsed || noteStart >= secondsElapsed + 10.0)
      {
        // Note not visible.
      }
      else
      {
        float effectiveStart = (noteStart < secondsElapsed) ? secondsElapsed : noteStart;
        float effectiveEnd = (noteEnd > secondsElapsed + 10.0) ? (secondsElapsed + 10.0) : noteEnd;

        float relStart = effectiveStart - secondsElapsed;
        float relEnd = effectiveEnd - secondsElapsed;

        int ledStart = (int)(relStart * (colSegment / 10.0));
        int ledEnd = (int)(relEnd * (colSegment / 10.0));
        if (ledEnd <= ledStart)
        {
          ledEnd = ledStart + 1;
        }
        int baseIndex = 2 * colSegment; // Column 2 base index.
        for (int led = baseIndex + ledStart; led < baseIndex + ledEnd && led < baseIndex + colSegment; led++)
        {
          leds[led] = colColors[2];
        }
      }
    }

    // Column 3 ("k")
    if (notes[i].height_k > 0.0)
    {
      float duration = notes[i].height_k;
      float noteEnd = noteStart + duration;
      if (noteEnd <= secondsElapsed || noteStart >= secondsElapsed + 10.0)
      {
        // Note not visible.
      }
      else
      {
        float effectiveStart = (noteStart < secondsElapsed) ? secondsElapsed : noteStart;
        float effectiveEnd = (noteEnd > secondsElapsed + 10.0) ? (secondsElapsed + 10.0) : noteEnd;

        float relStart = effectiveStart - secondsElapsed;
        float relEnd = effectiveEnd - secondsElapsed;

        int ledStart = (int)(relStart * (colSegment / 10.0));
        int ledEnd = (int)(relEnd * (colSegment / 10.0));
        if (ledEnd <= ledStart)
        {
          ledEnd = ledStart + 1;
        }
        int baseIndex = 3 * colSegment; // Column 3 base index.
        for (int led = baseIndex + ledStart; led < baseIndex + ledEnd && led < baseIndex + colSegment; led++)
        {
          leds[led] = colColors[3];
        }
      }
    }
  }

  // Update the LED strip.
  FastLED.show();
}

void setup()
{
  // Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
  Serial0.begin(115200);
  // AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  Serial0.println("--- Piano Tiles starting! ---");
  // Seesaw
  //  Serial.println(F("Waiting for seesaw to start..."));
  // if(!ss.begin()){
  //   while(1) delay(10);
  // }
  // Serial.println(F("Seesaw started..."));

  // Set up button actions
  setupActions();
  // Setup audiokit before SD
  setupAudio((void *)0);

  // Setup file
  SD.begin(chipSelect);

  // FastLED.addLeds<NEOPIXEL, 21>(leds, NUM_LEDS);
  // FastLED.clear();
  // FastLED.show();
  setupTiles();
  xTaskCreatePinnedToCore(
      setupLEDs, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000, /* Stack size in words */
      
# 441 "/Users/gabriel/Documents/Arduino/game/game.ino" 3 4
     __null
# 441 "/Users/gabriel/Documents/Arduino/game/game.ino"
         , /* Task input parameter */
      0, /* Priority of the task */
      &audioTask, /* Task handle. */
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
    Serial0.printf("Seconds elapsed %9.6f\n", secondsElapsed);
    // bool button1 = ss.digitalRead(BUTTON_1_PIN);
    // bool button2 = ss.digitalRead(BUTTON_2_PIN);
    // bool button3 = ss.digitalRead(BUTTON_3_PIN);
    // bool button4 = ss.digitalRead(BUTTON_4_PIN);
    handleButtonPress(button1, button2, button3, button4);
  }

  i2s.processActions(); // Process individual button actions

  // Read both button states
  // bool button1State = !digitalRead(i2s.getKey(1)); // Assuming active-low
  // bool button2State = !digitalRead(i2s.getKey(2)); // Assuming active-low

  // // Check if both are pressed at the same time
  // if (button1State && button2State)
  // {
  //   Serial.println("Both buttons pressed!");
  //   onBothButtonsPressed(); // Call function for this action
  // }
}

void setupLEDs(void *params)
{
  FastLED.addLeds<NEOPIXEL, 23>(leds, 120);
  FastLED.clear();
  // driver.initled((uint8_t*)leds,pins,NUMSTRIPS,NUM_LED_PER_STRIP,ORDER_GRBW);

  FastLED.show();
  // Serial.print("LED Task running on core: ");
  // Serial.println(xPortGetCoreID());
  runLEDs();
}

void runLEDs()
{
  for (;;)
  {
    // Serial.print("LED Task running on core: ");
    // Serial.println(xPortGetCoreID());
    displayNotes();
    // displayColumns();
  }
}

void setupAudio(void *params)
{
  auto config = i2s.defaultConfig(TX_MODE);
  config.sd_active = true;
  i2s.begin(config);
  // Setup I2S based on sampling rate provided by decoder
  i2s.setVolume(volume); // Set initial volume
  // audioPlayer();
}

void audioPlayer()
{
  for (;;)
  {
    if (playing)
    {
      copier.copy();
    }
  }
}

void printNote(piano_note_t note)
{
  Serial0.print("Note<Time: ");
  Serial0.print(note.time, 2); // print the time with two decimal places
  Serial0.print(" sec, height_d: ");
  Serial0.print(note.height_d, 2);
  Serial0.print(", height_f: ");
  Serial0.print(note.height_f, 2);
  Serial0.print(", height_j: ");
  Serial0.print(note.height_j, 2);
  Serial0.print(", height_k: ");
  Serial0.print(note.height_k, 2);
  Serial0.println(">");
}

void handleButtonPress(bool button1, bool button2, bool button3, bool button4)
{

}

void displayColumns()
{
  // Clear all LEDs.
  FastLED.clear();

  // Compute the spacing per column.
  int colSpacing = 120 / 4; // in this case 30

  // Define the colors for each column.
  CRGB colColors[4] = {CRGB::Red, CRGB::Blue, CRGB::Green, CRGB::Orange};

  // Set one LED per column: the first LED in each column will be lit with its assigned color.
  for (int col = 0; col < 4; col++)
  {
    int ledIndex = col * colSpacing;
    if (ledIndex < 120)
    {
      leds[ledIndex] = colColors[col];
    }
  }

  // Update the LED strip so that the colors are shown.
  FastLED.show();
}
