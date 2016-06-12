//////////////////////////////////////////
// CHECK INTERFACE POTS BUTTONS ETC //////
/////////////////////////////////////////

// A moving average of previous 5 values
void checkInterface(){

  int channel;
  unsigned long time;
  // This is set to true when chan changes and back to false
  // when reset is performed the next time.
  static boolean chanHasChanged = false;
  boolean buttonUp = false;
  boolean buttonDown = false;

  // READ & AVERAGE POTS

  int chanPot = 0;
  int chanCV = 0;
  int timPot = 0;
  int timCV = 0;

  for (int i = 0; i < SAMPLEAVERAGE; i++){
    chanPot += analogRead(CHAN_POT_PIN);
    chanCV += analogRead(CHAN_CV_PIN);
    timPot += analogRead(TIME_POT_PIN);
    timCV += analogRead(TIME_CV_PIN);
  }

  chanPot = chanPot / SAMPLEAVERAGE;
  chanCV = chanCV / SAMPLEAVERAGE;
  timPot = timPot / SAMPLEAVERAGE;
  timCV = timCV / SAMPLEAVERAGE;

  // Snap small values to zero.
  if (timPot <= timHyst)
    timPot = 0;
  if (timCV <= timHyst)
    timCV = 0;

  // IDENTIFY POT / CV CHANGES

  boolean chanPotChange = (abs(chanPot - chanPotOld) > chanHyst);
  boolean chanCVChange = (abs(chanCV - chanCVOld) > chanHyst);
  boolean timPotChange = (abs(timPot - timPotOld) > timHyst);
  boolean timCVChange = (abs(timCV - timCVOld) > timHyst);

  // MAP INPUTS TO CURRENT SITUATION

  channel = chanCV;
  int speed = constrain(chanPot, 0, 1023);
  pRawPlayer->set_speed(speed / 32);
  channel = constrain(channel, 0, 1023);
  channel = map(channel,0,1024,0,FILE_COUNT[PLAY_BANK]); // Highest pot value = 1 above what's possible (ie 1023+1) and file count is one above the number of the last file (zero indexed)

  time = timPot + timCV;
  time = constrain(time, 0U, 1023U);
  time = (time / StartCVDivider) * StartCVDivider; // Quantizes start position
  time  = time * (FILE_NAMES[PLAY_BANK][PLAY_CHANNEL].size / 1023);

  // IDENTIFY AND DEPLOY RELEVANT CHANGES

  if (channel != PLAY_CHANNEL && chanPotChange) {
    NEXT_CHANNEL = channel;
    CHAN_CHANGED = ChanPotImmediate;
    chanPotOld = chanPot;
  };

  if (channel != PLAY_CHANNEL && chanCVChange) {
    NEXT_CHANNEL = channel;
    CHAN_CHANGED = ChanCVImmediate;
    chanCVOld = chanCV;
  };

  // chanHasChanged status has different lifespan from CHAN_CHANGED,
  // it is set back to false when reset is performed next time.
  if (CHAN_CHANGED)
    chanHasChanged = true;

  // time pot or cv changes may cause reset if
  // enabled in settings.txt
  if (timPotChange){
    playhead = time;
    RESET_CHANGED =  StartPotImmediate;
    timPotOld = timPot;
  }

  if (timCVChange){
    playhead = time;
    RESET_CHANGED =  StartCVImmediate;
    timCVOld = timCV;
  }

  // Reset Button & CV
  if ( resetSwitch.update() ) {
    resetButton = resetSwitch.read();
//    Serial.println(resetButton);
  }
  // If button is up and was previously down, it's a button up event
  if (!resetButton && prevResetButton) {
    buttonUp = true;
  } else if (resetButton && !prevResetButton) {
    buttonDown = true;
    bankTimer = 0;
  }
  prevResetButton = resetButton;

  if ((buttonDown || resetCVHigh) && !bankChangeMode)
  {
    // We must set the playhead on reset if we previously have changed the channel.
    // But only once so that the further resets will guarantee to reset to the same spot again.
    if (chanHasChanged) {
      playhead = time;
      chanHasChanged = false;
    }
    RESET_CHANGED = true;
    buttonDown = false;
#if DEBUG
    Serial.print("Timpot value: ");
    Serial.println(timPot);
    Serial.print("TimCV value: ");
    Serial.println(timCV);
#endif
  }
  resetCVHigh = false;

  // Hold Reset Button to Change Bank
//  bankTimer = bankTimer * digitalRead(RESET_BUTTON);

  if (buttonUp) {
//    Serial.print("Button released, time = ");
//    Serial.println(bankTimer);

    if (bankTimer > SPECOPSTIME) {
      Serial.println("SPECOPS time!");
    } else if (bankTimer > HOLDTIME){
      bankChangeMode = !bankChangeMode;
#if DEBUG
      Serial.print("bankChangeMode = ");
      Serial.println(bankChangeMode);
#endif
    } else if (bankChangeMode) { // Bankchange is executed on button up to prevent the extra change once you want out of this mode.
      PLAY_BANK++;
      if (PLAY_BANK > ACTIVE_BANKS) PLAY_BANK = 0;
      if (NEXT_CHANNEL >= FILE_COUNT[PLAY_BANK]) NEXT_CHANNEL = FILE_COUNT[PLAY_BANK]-1;
      CHAN_CHANGED = true;
      bankTimer = 0;
      meterDisplay = 0;
      EEPROM.write(BANK_SAVE, PLAY_BANK);
    }
    buttonUp = false;
    bankTimer = 0;
  }
}
