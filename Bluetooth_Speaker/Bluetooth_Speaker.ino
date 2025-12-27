#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

#define BUTTON_PLAY_PAUSE 5
#define BUTTON_PREVIOUS 18
#define BUTTON_NEXT 19
#define BUTTON_VOLUME_UP 21
#define BUTTON_VOLUME_DOWN 22

bool lastPlayPauseState = HIGH;
bool lastPreviousState = HIGH;
bool lastNextState = HIGH;
bool lastVolumeUpState = HIGH;
bool lastVolumeDownState = HIGH;

bool isPlaying = false;
int currentVolume = 64; // Initial volume (range 0–127)

void setup() {
  Serial.begin(115200);

  i2s_pin_config_t my_pin_config = {
    .bck_io_num = 27,    // BCLK
    .ws_io_num = 26,     // LRC
    .data_out_num = 25,  // DIN
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  a2dp_sink.set_pin_config(my_pin_config);
  a2dp_sink.start("Shoron");

  pinMode(BUTTON_PLAY_PAUSE, INPUT_PULLUP);
  pinMode(BUTTON_PREVIOUS, INPUT_PULLUP);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_VOLUME_UP, INPUT_PULLUP);
  pinMode(BUTTON_VOLUME_DOWN, INPUT_PULLUP);

  a2dp_sink.set_volume(currentVolume);
}

void loop() {
  bool currentPlayPauseState = digitalRead(BUTTON_PLAY_PAUSE);
  bool currentPreviousState = digitalRead(BUTTON_PREVIOUS);
  bool currentNextState = digitalRead(BUTTON_NEXT);
  bool currentVolumeUpState = digitalRead(BUTTON_VOLUME_UP);
  bool currentVolumeDownState = digitalRead(BUTTON_VOLUME_DOWN);

  // Play/Pause
  if (currentPlayPauseState == LOW && lastPlayPauseState == HIGH) {
    //Serial.println("Play/Pause button pressed");
    if (isPlaying) {
      a2dp_sink.pause();
      isPlaying = false;
      //Serial.println("Paused");
    } else {
      a2dp_sink.play();
      isPlaying = true;
      //Serial.println("Playing");
    }
    delay(150);
  }
  lastPlayPauseState = currentPlayPauseState;

  // Previous Track
  if (currentPreviousState == LOW && lastPreviousState == HIGH) {
    //Serial.println("Previous button pressed");
    a2dp_sink.previous();
    delay(150);
  }
  lastPreviousState = currentPreviousState;

  // Next Track
  if (currentNextState == LOW && lastNextState == HIGH) {
    //Serial.println("Next button pressed");
    a2dp_sink.next();
    delay(150);
  }
  lastNextState = currentNextState;

  // Volume Up
  if (currentVolumeUpState == LOW && lastVolumeUpState == HIGH) {
    currentVolume = constrain(currentVolume + 8, 0, 127);
    a2dp_sink.set_volume(currentVolume);
    //Serial.print("Volume Increased: ");
    //Serial.println(currentVolume);
    delay(150);
  }
  lastVolumeUpState = currentVolumeUpState;

  // Volume Down
  if (currentVolumeDownState == LOW && lastVolumeDownState == HIGH) {
    currentVolume = constrain(currentVolume - 8, 0, 127);
    a2dp_sink.set_volume(currentVolume);
    //Serial.print("Volume Decreased: ");
    //Serial.println(currentVolume);
    delay(150);
  }
  lastVolumeDownState = currentVolumeDownState;
}
