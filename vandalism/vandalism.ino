#include <Console.h>

//int noteFrequencies[] = { 3830, 3400, 3038, 2864, 2550, 2272};
int noteFrequencies[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
//int noteFrequencies[6];
// State names
int RECORD_STATE = 0;
int PLAYBACK_STATE = 1;
int CHALLENGE_STATE = 2;
int WIN_STATE = 3;

// Actual pins
int leds[6];
int buttons[6];
boolean pushed[sizeof(buttons) / sizeof(int)];
int buzzer = 3;

// Current states
int state;
boolean ledStates[6];

int currentNote;

// Time management
//float timeCounter;
int stepCounter;
float currentTime;
float lastTime;
float lastLoopTime;

int fps = 30;
int fidelity = 30; // Times per seconds
int minumumLength = 4; // Minimum length to hold on a note
int melodyLength = 3;
int recordedMelody[300];
int distilledMelody[300];
int actualMelodyLength;
int actualDistilledMelodyLength;
int currentNoteToPlayInLoop;

// the setup routine runs once when you press reset:
void setup()
{
  Serial.begin(9600);
  
  // LEDs
  for (int i = 0; i < sizeof(leds) / sizeof(int); i++)
  {
    
    if(i == 1)
    {
       pinMode(0, OUTPUT);
       leds[i] = 0;
       digitalWrite(0,LOW);
    }
    else
    {
       leds[i] = i + 2;
       pinMode(leds[i], OUTPUT);
    }
  }

  // Buttons
  for (int i = 0; i < sizeof(buttons) / sizeof(int); i++)
  {
    buttons[i] = i + 8;
    pinMode(buttons[i], INPUT_PULLUP);
  }

  // Piezo
  pinMode(buzzer, OUTPUT);
  
  actualMelodyLength = melodyLength * fidelity;
  
  // Start recording
  setRecordState();
}

// the loop routine runs over and over again forever:
void loop()
{
    if(currentNoteToPlayInLoop > -1)
    {
/*(      digitalWrite(buzzer, HIGH);
      delayMicroseconds(noteFrequencies[currentNoteToPlayInLoop]);
      digitalWrite(buzzer, LOW);
      delayMicroseconds(noteFrequencies[currentNoteToPlayInLoop]);*/
      tone(buzzer,noteFrequencies[currentNoteToPlayInLoop]);
    }
    else
    {
      noTone(buzzer);
    }
  
  // Save current timeZ
  currentTime = millis();
  //if (currentTime - lastTime >= 1 / fps)
  //{
    // State zoo machine
    if (state == RECORD_STATE)
    {
      //Serial.print("RECORD STATE");
      recordState();
    }
    else if (state == PLAYBACK_STATE)
    {
      //Serial.print("PLAYBACK STATE");
      playbackState();
    }
    else if (state == CHALLENGE_STATE)
    {
      //Serial.print("CHALLENGE STATE");
      challengeState();
    }
    else if (state == WIN_STATE)
    {
      //Serial.print("WIN STATE");
      winState();
    }
    
    //Serial.print(".");
    
    // Save time before the next tick
    //lastTime = currentTime;
  //}
  delay(((1000)/fps) - (millis() - currentTime));
  
  //lastLoopTime = currentTime;
}

void updateAll()
{
  for (int i = 0; i < sizeof(leds) / sizeof(int); i++)
  {
    if (ledStates[i])
    {
      digitalWrite(leds[i], HIGH);
    }
    else
    {
      digitalWrite(leds[i], LOW);
    }
  }
}

void killTheLights()
{
  for (int i = 0; i < sizeof(leds) / sizeof(int); i++)
  {
    ledStates[i] = false;
  }
}

void setRecordState()
{
  Serial.println("GOING TO RECORD STATE");
  
  killTheLights();
  updateAll();
  // Limited to 300
  for (int i = 0; i < actualMelodyLength; i++)
  {
    recordedMelody[i] = 0;
  }
  
//  timeCounter = 0;
  stepCounter = 0;
  state = RECORD_STATE;
  
  currentNote = 0;
}

void recordState()
{
  if (currentNote > 0)
  {
//    timeCounter += currentTime - lastTime;
      stepCounter++;
  }
  
  //if (timeCounter >= melodyLength)
  if (stepCounter >= actualMelodyLength)
  {
    setPlaybackState();
    return;
  }
  currentNoteToPlayInLoop =-1;
  for (int i = 0; i < sizeof(buttons) / sizeof(int); i++)
  {
    if (digitalRead(buttons[i]) == LOW)
    {
      currentNote++;
      ledStates[i] = true;
      
      currentNoteToPlayInLoop=i;
      //tone(buzzer,noteFrequencies[i]);
      
      //audio.play()
      // CHANGE TONE
      // audio.pitch = 1 + (musicScale[i] * 1f/12f);
      //recordedMelody[(int)(timeCounter * fidelity)] = i + 1;
      recordedMelody[stepCounter] = i + 1;
      break;
    }
    else
    {
      ledStates[i] = false;
    }
  }
  updateAll();
}

void setPlaybackState()
{
    Serial.println("GOING TO PLAYBACK STATE");

  
  //timeCounter = 0;
  stepCounter = 0;
  state = PLAYBACK_STATE;
  
  // Clean up
  
  int lastNote = recordedMelody[0];
  int duplicateCounter = 0;
  int len = melodyLength * fidelity;
  for (int i = 0; i < actualMelodyLength; i++)
  {
    if (recordedMelody[i] == lastNote)
    {
      duplicateCounter++;
    }
    else
    {
      if (duplicateCounter <= minumumLength)
      {
        recordedMelody[i] = lastNote;
        duplicateCounter++;
      }
      else
      {
        duplicateCounter = 0;
      }
    }
    lastNote = recordedMelody[i];
  }
}

void playbackState()
{
  //timeCounter += currentTime - lastTime;
  stepCounter++;
  //if (timeCounter >= melodyLength)
  if (stepCounter >= actualMelodyLength)
  {
    setChallengeState();
    return;
  }

  //audio.Pause();
  currentNoteToPlayInLoop =-1;
  for (int i = 0; i < sizeof(buttons) / sizeof(int); i++)
  {
    ledStates[i] = false;
  }

  //int i = (int)(timeCounter * fidelity);
  if (stepCounter < actualMelodyLength)
  {
    //int melodyInstance = recordedMelody[(int)(timeCounter * fidelity)] - 1;
    int melodyInstance = recordedMelody[stepCounter] - 1;
    if (melodyInstance >= 0)
    {
      ledStates[melodyInstance] = true;
      currentNoteToPlayInLoop = melodyInstance;
      //audio.Play();
      //tone(piezo, 1 +  (musicScale[melodyInstance] * 1/12));
      //261
      //311
      //349
      //392
      //
    }
  }

  updateAll();
}


void setChallengeState()
{
  Serial.println("GOING TO CHALLENGE STATE");

  state = CHALLENGE_STATE;
  //timeCounter = 0;
  stepCounter = 0;

  // calculates the distilled melody
  /*int counter = 0;
  for (int i = 0; i < actualMelodyLength - 1; i++)
  {
    if (recordedMelody[i] != 0 && recordedMelody[i] != recordedMelody[i + 1])
    {
      counter++;
    }
  }*/



  // DISTILLED STUFF
  for (int i = 0; i < actualMelodyLength; i++)
  {
    distilledMelody[i] = 0;
  }
  int counter = 0;
  for (int i = 0; i < actualMelodyLength - 1; i++)
  {
      if (recordedMelody[i] != 0 && recordedMelody[i] != recordedMelody[i + 1])
      {
          distilledMelody[counter] = recordedMelody[i];
          counter++;
      }
  }
  
  actualDistilledMelodyLength = counter;


  //reset current note to try
  currentNote = 0;
}

void challengeState()
{
  // Step the time, and go to next state
  //timeCounter += currentTime - lastTime;

  //if (timeCounter >= 2)
  if (stepCounter >= 2 * fidelity) // Wait 2 seconds to play the melody
  {
     setPlaybackState();
     return;
  }

  currentNoteToPlayInLoop =-1;
  for (int i = 0; i < sizeof(buttons) / sizeof(int); i++)
  {
    int buttonState = digitalRead(buttons[i]);

    if (buttonState == LOW && !pushed[i])
    {
      pushed[i] = true;
      if ((i + 1) == distilledMelody[currentNote])
      {
        currentNote++;
      }
      else
      {
        currentNote = 0;
        if ((i + 1) == distilledMelody[currentNote])
        {
          currentNote = 1;
        }
      }
      if (currentNote >= actualDistilledMelodyLength)
      {
        setWinState();
      }
    }

    if (buttonState == LOW)
    {
      ledStates[i] = true;
      currentNoteToPlayInLoop = i;
      //audio.pitch = 1 + (musicScale[i] * 1f/12f);
      //timeCounter = 0;
      stepCounter = 0;
      pushed[i] = true;
      Serial.println("RESET TIME COUNTER");
    }
    else
    {
      ledStates[i] = false;
      pushed[i] = false;
    }
  }
  updateAll();
  
  stepCounter++;
}

void setWinState()
{
      Serial.println("GOING TO WIN STATE");

  
  state = WIN_STATE;
  //timeCounter = 0;
  stepCounter = 0;
  currentNote = 0;
  killTheLights();
  updateAll();
}

void winState()
{
  //timeCounter += currentTime - lastTime;
  stepCounter++;
  //if (timeCounter > 0.05)
  if (stepCounter > (1 / 6) * fidelity)
  {
      //timeCounter = 0;
      stepCounter = 0;

      //audio.Pause();
      currentNoteToPlayInLoop =-1;

      for (int i = 0; i < sizeof(ledStates); i++)
      {
          if (!ledStates[i])
          {
              ledStates[i] = true;
              currentNoteToPlayInLoop = i;
              //audio.Play();
              //audio.pitch = 1 + (i * 0.1f);
              break;
          }
          if (i == sizeof(ledStates) - 1)
          {
              killTheLights();
              currentNote++;
          }
      }
  }

  if (currentNote >= 5)
  {
      setRecordState();
  }
  updateAll();
}
