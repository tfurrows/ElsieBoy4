/*
   ElsieBoy4 v0.3.2 - Turn your Arduboy into an air-gapped LC4 appliance.
   This is free and unencumbered software released into the public domain.
*/
#include <Arduboy2.h>

Arduboy2 arduboy;
BeepPin1 beep;

char verHead[] = "ElsieBoy4 0.3";
int menuChoice = 0;
String options[5] = {"> Key", "> Nonce", "> Encode", "> Decode", "> Status"};

// Valid characters for LC4, #_23456789abcdefghijklmnopqrstuvwxyz
int validChars[36] = {
  35, 95, 50, 51, 52, 53,
  54, 55, 56, 57, 97, 98,
  99, 100, 101, 102, 103, 104,
  105, 106, 107, 108, 109, 110,
  111, 112, 113, 114, 115, 116,
  117, 118, 119, 120, 121, 122
};

int lc4Key[36] = {0};
int lc4Nonce[6] = {0};
int lc4KeyState[36] = {0};
int marker[2] = {0, 0};
int steps = 0;

void setup() {
  arduboy.begin();
  beep.begin();
  arduboy.setTextWrap(true);
  showOpts(menuChoice);  
}

void loop() {
  if (arduboy.anyPressed(UP_BUTTON | DOWN_BUTTON)) {
    if (arduboy.pressed(UP_BUTTON)) menuChoice--;
    if (arduboy.pressed(DOWN_BUTTON)) menuChoice++;
    if (menuChoice < 0) menuChoice = 4;
    if (menuChoice > 4) menuChoice = 0;
    showOpts(menuChoice);
    delay(125);
  }

  if (arduboy.pressed(B_BUTTON)) {
    delay(200);
    if (menuChoice == 0) keyMod();
    if (menuChoice == 1) nonceMod();
    if (menuChoice == 2) dencode(0);
    if (menuChoice == 3) dencode(1);
    if (menuChoice == 4) showStatus();
    delay(200);
    showOpts(menuChoice);
  }
}

void initView(int showLoc) {
  // Setup the basic screen elements
  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
  arduboy.setTextSize(1);
  arduboy.clear();
  arduboy.drawFastHLine(0, 10, 128);

  arduboy.print(verHead);
  if (showLoc > 0) arduboy.print(options[menuChoice]);

  arduboy.setCursor(0, 14);
  arduboy.display();
}

void showOpts(int selectedOpt) {
  // Display main menu
  initView(0);

  for (int i = 0; i < 5; i++) {
    if (i == menuChoice) {
      arduboy.setTextSize(2);
    } else {
      arduboy.setTextSize(1);
    }
    arduboy.print(options[i] + "\n");
  }
  arduboy.display();
}

void showKey(int *viewArray, int selected) {
  // Display key or key state, with highlight
  for (int i = 0; i < 36; i++) {
    if ((i > 0) && (i % 6 == 0)) arduboy.print("\n");
    if (i == selected) {
      arduboy.setTextBackground(WHITE);
      arduboy.setTextColor(BLACK);
    } else {
      arduboy.setTextBackground(BLACK);
      arduboy.setTextColor(WHITE);
    }
    if (viewArray[i] == 0) {
      arduboy.print(".");
    } else {
      arduboy.write(viewArray[i]);
    }
  }

  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
  arduboy.display();
}

void showNonce(int pos) {
  // Display the nonce, with highlight
  arduboy.setCursor(8, 24);
  arduboy.setTextSize(3);

  for (int i = 0; i < 6; i++) {
    if (i == pos) {
      arduboy.setTextBackground(WHITE);
      arduboy.setTextColor(BLACK);
    } else {
      arduboy.setTextBackground(BLACK);
      arduboy.setTextColor(WHITE);
    }
    if (lc4Nonce[i] == 0) {
      arduboy.print(".");
    } else {
      arduboy.write(lc4Nonce[i]);
    }
  }

  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
  arduboy.setTextSize(1);
  arduboy.display();
}

void showStatus() {
  // Display keystate and marker
  initView(1);
  arduboy.drawFastVLine(40, 10, 86);

  showKey(lc4KeyState, marker[1] * 6 + marker[0]);

  arduboy.setCursor(46, 14);
  arduboy.print((String)"Step: " + steps);

  arduboy.setCursor(46, 22);
  arduboy.print((String)"Mark: " + marker[0] + "," + marker[1]);
  arduboy.display();

  while (!arduboy.anyPressed(A_BUTTON | B_BUTTON)) {
  }
}

void keyMod() {
  // Key entry/modification (required)
  int pos = 0, posInValid=0, selectChar = 0, isTaken, isOpen, incVal;

  initView(1);
  arduboy.drawFastVLine(40, 10, 86);
  showKey(lc4Key, pos);

  while (!arduboy.pressed(A_BUTTON)) {
    if (arduboy.anyPressed(UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON | B_BUTTON)) {
      if (arduboy.pressed(B_BUTTON)) lc4Key[pos] = 0;
      if (arduboy.anyPressed(UP_BUTTON | DOWN_BUTTON)) {
        // Check if any part of the key is still set to 0
        isOpen = 0;
        for (int i = 0; i < 36; i++) {
          if (lc4Key[i] == 0) isOpen = 1;
        }

        // There are unset parts of key, allow changes to character
        if (isOpen == 1) {
          if (arduboy.pressed(UP_BUTTON)) incVal = 1;
          if (arduboy.pressed(DOWN_BUTTON)) incVal = -1;

          // Make sure they aren't selecting a used character
          lc4Key[pos] = nextAvail(lc4Key, 36, incVal, selectChar);
          for (int i = 0; i < 36; i++) {
            if (validChars[i] == lc4Key[pos]) selectChar = i;
          }
        }
        delay(100);
      }

      if (arduboy.pressed(LEFT_BUTTON)) pos--;
      if (arduboy.pressed(RIGHT_BUTTON)) pos++;
      if (pos < 0) pos = 35;
      if (pos > 35) pos = 0;

      // For finding the marker indexes
      for (int i = 0; i < 36; i++) {
        if (lc4Key[pos] == validChars[i]) posInValid = i;
      }

      // Display character and marker indexes
      arduboy.setCursor(78, 24);
      arduboy.setTextSize(3);
      arduboy.write(lc4Key[pos]);

      arduboy.setTextSize(1);
      arduboy.setCursor(104, 32);
      arduboy.print(posInValid % 6);
      arduboy.setCursor(83, 54);
      arduboy.print(posInValid / 6);

      arduboy.setCursor(0, 14);
      showKey(lc4Key, pos);

      delay(100);
    }
  }
}

void nonceMod() {
  // Nonce entry/modification (optional)
  int pos = 0, selectChar = 0, isTaken, incVal = 0;

  initView(2);
  showNonce(pos);

  while (!arduboy.pressed(A_BUTTON)) {
    if (arduboy.anyPressed(UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON | B_BUTTON)) {
      if (arduboy.pressed(B_BUTTON)) lc4Nonce[pos] = 0;
      if (arduboy.anyPressed(UP_BUTTON | DOWN_BUTTON)) {
        if (arduboy.pressed(UP_BUTTON)) incVal = 1;
        if (arduboy.pressed(DOWN_BUTTON)) incVal = -1;

        // Make sure they aren't selecting a used character
        lc4Nonce[pos] = nextAvail(lc4Nonce, 6, incVal, selectChar);
        for (int i = 0; i < 36; i++) {
          if (validChars[i] == lc4Nonce[pos]) selectChar = i;
        }

        delay(100);
      }

      if (arduboy.pressed(LEFT_BUTTON)) pos--;
      if (arduboy.pressed(RIGHT_BUTTON)) pos++;
      if (pos < 0) pos = 5;
      if (pos > 5) pos = 0;

      showNonce(pos);

      delay(100);
    }
  }
}

void dencode(int opt) {
  // Encode/Decode characters (0 encode, 1 decode)
  int isZero, answer = 0, pos = 0;

  initView(3);

  // Check if key is complete (required)
  isZero = 0;
  for (int i = 0; i < 36; i++) {
    if (lc4Key[i] == 0) isZero = 1;
  }
  if (isZero == 1) {
    arduboy.print("Key is not set.");
    arduboy.display();
    delay(1000);
    return;
  }

  // Make sure key state is initialized
  for (int i = 0; i < 36; i++) {
    if (lc4KeyState[i] == 0) lc4KeyState[i] = lc4Key[i];
  }

  // Check if key state is baseline / equal to the key
  isZero = 1;
  for (int i = 0; i < 36; i++) {
    if (lc4KeyState[i] != lc4Key[i]) isZero = 0;
  }
  if (isZero == 0) {
    // State isn't zero, offer to reset it
    arduboy.print("Keystate is altered. Reset it?");
    answer = yesNo(24, 34, 2);
    initView(3);

    if (answer == 1) {
      for (int i = 0; i < 36; i++) {
        lc4KeyState[i] = lc4Key[i];
      }

      // also reset step count and marker location
      steps = 0;
      marker[0] = 0;
      marker[1] = 0;

      arduboy.print("Keystate Reset.");
      arduboy.display();
      delay(800);
      return;
    }
  }

  // Check if nonce is fully set and step is 0, apply nonce to state
  isZero = 0;
  for (int i = 0; i < 6; i++) {
    if (lc4Nonce[i] == 0) isZero = 1;
  }
  if (isZero == 0 && steps == 0) {
    for (int i = 0; i < 6; i++) {
      shiftState(lc4Nonce[i], getDenc(lc4Nonce[i], 0));
      steps++;
    }
  }

  // Start work on (de)encoding...
  while (true) {
    arduboy.setTextSize(1);
    arduboy.setCursor(0, 16);
    arduboy.print((String)"Step: " + steps);

    arduboy.setTextSize(3);
    arduboy.setCursor(24, 28);
    arduboy.write(validChars[pos]);
    arduboy.setCursor(56, 28);
    arduboy.print(">");
    arduboy.setCursor(88, 28);
    arduboy.write(getDenc(validChars[pos], opt));

    arduboy.display();

    // Character selection
    while (!arduboy.pressed(B_BUTTON)) {
      if (arduboy.pressed(A_BUTTON)) return; // Cancel (de)encoding, return to menu
      if (arduboy.anyPressed(UP_BUTTON | DOWN_BUTTON | RIGHT_BUTTON | LEFT_BUTTON)) {
        if (arduboy.pressed(UP_BUTTON)) pos += 1;
        if (arduboy.pressed(DOWN_BUTTON)) pos += -1;
        if (arduboy.pressed(RIGHT_BUTTON)) pos += 6;
        if (arduboy.pressed(LEFT_BUTTON)) pos += -6;

        if (pos < 0) pos = 35;
        if (pos > 35) pos = 0;

        arduboy.setCursor(24, 28);
        arduboy.write(validChars[pos]);
        arduboy.setCursor(56, 28);
        arduboy.print(">");
        arduboy.setCursor(88, 28);
        arduboy.write(getDenc(validChars[pos], opt));

        arduboy.display();
        delay(200);
      }
    }

    // Complete shifting and update counters, display Last
    arduboy.setTextSize(1);
    arduboy.setCursor(0, 56);

    if (opt == 0) {
      arduboy.print((String)"Last: " + (char)validChars[pos] + " > " + (char)getDenc(validChars[pos], 0));
      shiftState(validChars[pos], getDenc(validChars[pos], 0));
    } else {
      arduboy.print((String)"Last: " + (char)getDenc(validChars[pos], 1) + " > " + (char)validChars[pos]);
      shiftState(getDenc(validChars[pos], 1), validChars[pos]);
    }
    steps++;
    beep.tone(beep.freq(240));
    delay(50);
    beep.noTone();
    delay(250);
  }
}

int getDenc(int inChar, int opt) {
  // Given plain/cipher text, keystate, and marker, reveal the corresponding character (0 encode, 1 decode)
  int posUnder, charPos, newX, newY;

  // Find indexes for movement based on current marker
  for (int i = 0; i < 36; i++) {
    if (validChars[i] == lc4KeyState[(marker[1] * 6 + marker[0])]) posUnder = i;
  }

  // Find position of character in key state
  for (int i = 0; i < 36; i++) {
    if (lc4KeyState[i] == inChar) charPos = i;
  }

  // Find the corresponding character position
  if (opt == 0) {
    newX = (charPos % 6) + (posUnder % 6);
    newY = (charPos / 6) + (posUnder / 6);
    if (newX > 5) newX -= 6;
    if (newY > 5) newY -= 6;
  } else {
    newX = (charPos % 6) - (posUnder % 6);
    newY = (charPos / 6) - (posUnder / 6);
    if (newX < 0) newX += 6;
    if (newY < 0) newY += 6;
  }

  return lc4KeyState[(newY * 6 + newX)];
}

void shiftState(int plainChar, int encChar) {
  // Shift the key state, move the marker
  int plainPos, plainRow, shiftRow[6], i1, cipherPos, cipherCol;

  //// First, in plaintext row, shift right
  // Find plaintext row in current state
  for (int i = 0; i < 36; i++) {
    if (lc4KeyState[i] == plainChar) plainPos = i;
  }
  plainRow = plainPos / 6;

  // Get array of this row's values
  i1 = 0;
  for (int i = (plainRow * 6); i < (plainRow * 6 + 6); i++) {
    shiftRow[i1] = lc4KeyState[i];
    i1++;
  }

  // Is marker in this row? shift is so
  if (marker[1] == plainRow) {
    marker[0]++;
    if (marker[0] > 5) marker[0] -= 6;
  }

  // Shift temp array
  shiftArray(shiftRow, 6);

  // Replace the new values into the original
  i1 = 0;
  for (int i = (plainRow * 6); i < (plainRow * 6 + 6); i++) {
    lc4KeyState[i] = shiftRow[i1];
    i1++;
  }

  //// Second, in ciphertext row, shift down
  // Find plaintext column in current shifted state
  for (int i = 0; i < 36; i++) {
    if (lc4KeyState[i] == encChar) cipherPos = i;
  }
  cipherCol = cipherPos % 6;

  // Get array of this column's values
  i1 = 0;
  for (int i = cipherCol; i < (cipherCol + 36); i += 6) {
    shiftRow[i1] = lc4KeyState[i];
    i1++;
  }

  // Is marker in this col? shift is so
  if (marker[0] == cipherCol) {
    marker[1] ++;
    if (marker[1] > 5) marker[1] -= 6;
  }

  // Shift temp array
  shiftArray(shiftRow, 6);

  // Replace the new values into the original
  i1 = 0;
  for (int i = cipherCol; i < (cipherCol + 36); i += 6) {
    lc4KeyState[i] = shiftRow[i1];
    i1++;
  }

  //// Move the marker right/down using ciphertext indexes
  // Get ciphertext indexes
  for (int i = 0; i < 36; i++) {
    if (validChars[i] == encChar) cipherPos = i;
  }

  marker[0] += cipherPos % 6;
  if (marker[0] > 5) marker[0] -= 6;

  marker[1] += cipherPos / 6;
  if (marker[1] > 5) marker[1] -= 6;
}

int yesNo(int x, int y, int tsize) {
  // Get a yes or no answer (1 or 0)
  int answer = 0;

  delay(200);
  arduboy.setTextSize(tsize);

  while (!arduboy.anyPressed(A_BUTTON | B_BUTTON)) {
    arduboy.setCursor(x, y);

    if (arduboy.pressed(LEFT_BUTTON)) answer = 1;
    if (arduboy.pressed(RIGHT_BUTTON)) answer = 0;

    for (int i = 0; i < 2; i++) {
      if (i != answer) {
        arduboy.setTextBackground(WHITE);
        arduboy.setTextColor(BLACK);
      } else {
        arduboy.setTextBackground(BLACK);
        arduboy.setTextColor(WHITE);
      }
      if (i == 0) arduboy.print(" Y ");
      if (i == 1) arduboy.print(" N ");
      arduboy.display();

    }
  }
  delay(200);

  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
  arduboy.setTextSize(1);

  return answer;
}

int nextAvail(int *inArray, int arrSize, int incVal, int selectChar) {
  // Return next unused valid character for an array
  int isTaken = 1;

  while (isTaken == 1) {
    isTaken = 0;

    selectChar += incVal;

    if (selectChar < 0) selectChar = 35;
    if (selectChar > 35) selectChar = 0;

    for (int i = 0; i < arrSize; i++) {
      if (validChars[selectChar] == inArray[i]) isTaken = 1;
    }
  }

  return validChars[selectChar];
}

void shiftArray(int *arrVals, int arrSize) {
  // Shift an array
  int temp = arrVals[arrSize - 1], temp1;

  for (int i = 0; i < arrSize; i++) {
    temp1 = arrVals[i];
    arrVals[i] = temp;
    temp = temp1;
  }
}

void playTone(unsigned int frequency, unsigned long duration) {
  if (arduboy.audio.enabled() == true) {
    tone(PIN_SPEAKER_1, frequency, duration);
  }
}
