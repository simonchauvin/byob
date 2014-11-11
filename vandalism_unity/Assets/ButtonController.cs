using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using System;

public class ButtonController : MonoBehaviour {

    public List<Image> images;
    public bool[] imageStates;
    public KeyCode[] buttons;
    public int[] recordedMelody;
    public int[] distilledMelody;
    public int melodyLength;
    public int currentNote;
    public int[] musicScale;
    public int fidelity;
    public int minumumLength;
    private Action state;
    private float timeCounter;

	void Start () {
        
        SetRecordState();
	}
	
	void Update () {
        if(state != null) state();
	}


    void UpdateAll()
    {
        for (int i = 0; i < images.Count; i++)
        {
            if(imageStates[i])
            {
                images[i].color = Color.red;
            }
            else
            {
                images[i].color = Color.white;
            }
        }
    }

    void KillTheLights()
    {
        for (int j = 0; j < imageStates.Length; j++)
        {
            imageStates[j] = false;
        }
    }

    void SetRecordState()
    {
        imageStates = new bool[images.Count];
        UpdateAll();
        recordedMelody = new int[melodyLength * fidelity];

        timeCounter = 0;
        Debug.Log("setting record");
        state = RecordState;

        currentNote = 0;
    }
    
    void RecordState()
    {
        if (currentNote > 0)
            timeCounter += Time.deltaTime;

        if (timeCounter >= melodyLength)
        {
            SetPlaybackState();
        }

        audio.Pause();
        for (int i = 0; i < buttons.Length; i++)
        {
            if (Input.GetKey(buttons[i]))
            {
                currentNote++;
                imageStates[i] = true;
                audio.Play();
                audio.pitch = 1 + (musicScale[i] * 1f/12f);
                recordedMelody[(int)(timeCounter * fidelity)] = i + 1;
            }
            else
            {
                imageStates[i] = false;
            }
        }
        UpdateAll();
    }

    void SetPlaybackState()
    {
        timeCounter = 0;
        Debug.Log("setting playback");
        state = PlaybackState;

        // Clean up recording
        int lastNote = recordedMelody[0];
        int duplicateCounter = 0;
        for (int i = 0; i < recordedMelody.Length; i++)
        {
            if(recordedMelody[i] == lastNote)
            {
                duplicateCounter++;
            }
            else
            {
                if(duplicateCounter <= minumumLength)
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

    void PlaybackState()
    {
        // Step the time, and go to next state
        timeCounter += Time.deltaTime;
        if (timeCounter >= melodyLength)
        {
            SetChallengeState();
            return;
        }

        audio.Pause();
        for (int j = 0; j < buttons.Length; j++)
        {
            imageStates[j] = false;
        }

        int i = (int)(timeCounter * fidelity);
        if (i < recordedMelody.Length)
        {
            int melodyInstance = recordedMelody[(int)(timeCounter * fidelity)] - 1;
            if (melodyInstance >= 0)
            {
                imageStates[melodyInstance] = true;
                audio.Play();
                audio.pitch = 1 +  (musicScale[melodyInstance] * 1f/12f);
            }
        }

        UpdateAll();
    }

    void SetChallengeState()
    {
        Debug.Log("setting challenge");
        state = ChallengeState;

        timeCounter = 0;

        // calculates the distilled melody
        int counter = 0;
        for (int i = 0; i < recordedMelody.Length-1; i++)
        {
            
            if(recordedMelody[i] != 0 && recordedMelody[i] != recordedMelody[i+1])
            {
                counter++;
            }
        }

        distilledMelody = new int[counter];
        counter =0;
        for (int i = 0; i < recordedMelody.Length - 1; i++)
        {

            if (recordedMelody[i] != 0 && recordedMelody[i] != recordedMelody[i + 1])
            {
                distilledMelody[counter] = recordedMelody[i];
                counter++;
            }
        }

        //reset current note to try
        currentNote = 0;
    }

    void ChallengeState()
    {
        // Step the time, and go to next state
        timeCounter += Time.deltaTime;
        if (timeCounter >= 2)
        {
            SetPlaybackState();
        }

        audio.Pause();
        for (int i = 0; i < buttons.Length; i++)
        {
            if(Input.GetKeyDown(buttons[i]))
            {

                if ((i+1) == distilledMelody[currentNote])
                {
                    
                    Debug.Log("YOU DID ONE CORRECTLY " + currentNote);
                    currentNote++;
                }
                else
                {
                    Debug.Log("WRONG " + (i+1) + " =/= " + distilledMelody[currentNote]);
                    currentNote = 0;
                    if ((i+1) == distilledMelody[currentNote])
                    {
                        currentNote = 1;
                    }
                    
                }
                if (currentNote >= distilledMelody.Length)
                {
                    Debug.Log("YOU WIN");
                    SetWinState();
                }
            }


            if (Input.GetKey(buttons[i]))
            {
                //Debug.Log("DO STUFFF");
                imageStates[i] = true;
                audio.Play();
                audio.pitch = 1 + (musicScale[i] * 1f/12f);
                timeCounter = 0;

            }
            else
            {
                imageStates[i] = false;
            }
        }
        UpdateAll();
    }


    void SetWinState()
    {
        state = WinState;
        timeCounter = 0;
        currentNote = 0;
        KillTheLights();
        UpdateAll();


    }

    void WinState()
    {
        timeCounter += Time.deltaTime;
        if(timeCounter > 0.05f)
        {
            timeCounter = 0;

            audio.Pause();

            for (int i = 0; i < imageStates.Length; i++)
            {
                if(!imageStates[i])
                {
                    imageStates[i] = true;
                    audio.Play();
                    audio.pitch = 1 + (i * 0.1f);
                    break;
                }
                if(i == imageStates.Length-1)
                {
                    KillTheLights();
                    currentNote++;
                }
            }
        }

        if(currentNote >=5)
        {
            SetRecordState();
        }
        UpdateAll();
    }


}
