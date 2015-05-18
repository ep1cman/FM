#include <SPI.h>
#include <Wire.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>

//Initialises the SD card and MP3 player objects.
SdFat sd;
SFEMP3Shield MP3player;


byte receivedI2C = 0x00;
byte I2Creply[48];
int replyLength = 0;
boolean newI2C = FALSE;
boolean moreI2C = FALSE;
boolean paused = FALSE;
int recordingState = 0;
int trackNumber = -1;


// Filename of the recorder plugin, located in the root of the SD card. 
char pluginfilename[] = "e44k2q05.vs"; // Music profile number 5: typical 135kbps stereo

uint8_t ledstate = 0;

void setup() {
	//Sets up the I2C slave events.	
        Wire.begin(55);
        Wire.onRequest(requestEvent);
        Wire.onReceive(receiveEvent);
 
	//Initialize the SdCard.
	if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();
	if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

	//Initialises the VS1053B
	StartMP3();
}

void loop()
{
  if (newI2C == TRUE) {
	//Then we have a new I2C command waiting for us to deal with it.
    newI2C = FALSE;
	//Checks the state of the recorder chip.
    recordingState = MP3player.isRecording();
    if(receivedI2C == 0x01) {
     //Then a start recording command has been received.
      if(moreI2C == TRUE && Wire.available() == 1) {
		//Then the proper command format has been sent, including a file number.
        moreI2C = FALSE;
                   
        if(recordingState == 0) {
          //Then we're not currently recording and can successfully begin recording. 

		  //Reads the file number from the I2C bus and sets the I2C reply.
          trackNumber = (int)Wire.read();
          I2Creply[0] = 0x77;
          replyLength = 1;  
          
          // This is a horrendous way of doing this, but it's 2am and there's some weird bug with Serial.print wihch is making this very hard.
          switch(trackNumber) {
            case 1:
            strcpy(trackname, "01.ogg");
            break;
            case 2:
            strcpy(trackname, "02.ogg");
            break;
            case 3:
            strcpy(trackname, "03.ogg");
            break;
            case 4:
            strcpy(trackname, "04.ogg");
            break;
            case 5:
            strcpy(trackname, "05.ogg");
            break;
            case 6:
            strcpy(trackname, "06.ogg");
            break;
            case 7:
            strcpy(trackname, "07.ogg");
            break;
            case 8:
            strcpy(trackname, "08.ogg");
            break;
            case 9:
            strcpy(trackname, "09.ogg");
            break;
            case 10:
            strcpy(trackname, "10.ogg");
            break;
          }      
          
		  //Starts the VS1053 recording.        
          MP3player.startRecordOgg(trackname, pluginfilename);
          paused = FALSE;
          
       } else {
			//Then we don't have a correctly formatted command.
          I2Creply[0] = 0x99;
          replyLength = 1;
        }    
          
      }
    } else if(receivedI2C == 0x02) {
	//Then we've received a stop recording command.
  
     if (recordingState == 1){
			//Then we're currently recording and can stop it.
          I2Creply[0] = 0x77;
          replyLength = 1;
	  MP3player.stopRecording(); 
	} else {
			//Then we're not currently recording, so can't stop recording.
          I2Creply[0] = 0x99;
          replyLength = 1;
        }
    } else if(receivedI2C == 0x03) {
		//Command for selecting a file to play back. NOT CURRENTLY USED.
      I2Creply[0] = 0x77;
      replyLength = 1;
    } else if(receivedI2C == 0x04) {
		//Command received for playing back a file.
      if (moreI2C = TRUE && Wire.available() == 1) {
		//Then we have a properly formatted command with a file number which we can read off the I2C bus.
        if (recordingState == 0) {
		//Then we're not currently recording and can start playback.
          I2Creply[0] = 0x77;
          replyLength = 1;
          trackNumber = (int)Wire.read();
          
           switch(trackNumber) {
            case 1:
            strcpy(trackname, "01.ogg");
            break;
            case 2:
            strcpy(trackname, "02.ogg");
            break;
            case 3:
            strcpy(trackname, "03.ogg");
            break;
            case 4:
            strcpy(trackname, "04.ogg");
            break;
            case 5:
            strcpy(trackname, "05.ogg");
            break;
            case 6:
            strcpy(trackname, "06.ogg");
            break;
            case 7:
            strcpy(trackname, "07.ogg");
            break;
            case 8:
            strcpy(trackname, "08.ogg");
            break;
            case 9:
            strcpy(trackname, "09.ogg");
            break;
            case 10:
            strcpy(trackname, "10.ogg");
            break;
          }      
          //Restarts the VS1053 in playback mode.
        	StartMP3(); 
		  //Begins playback of specified file.
        	MP3player.playMP3(trackname);
        }
        } else {
		//Improperly formatted command received.
          I2Creply[0] = 0x99;
          replyLength = 1;
        }          
    } else if(receivedI2C == 0x05) {
		//Then we've recieved a toggle pause playback command.
      if (recordingState == 0 && MP3player.isPlaying() == 1 && paused == FALSE) {
		//Then we have a file currently being played back which isn't paused, so can pause it.
        I2Creply[0] = 0x77;
        replyLength = 1;
        MP3player.pauseMusic();
        paused = TRUE;
      } else if (recordingState == 0 && paused == TRUE) {
		//Then we have a file currently paused, so we can unpause it.
        I2Creply[0] = 0x77;
        replyLength = 1;
        MP3player.resumeMusic();
        paused = FALSE;
      } else if(recordingState == 1){
		//Then we're currently recording and thus cannot pause.
        I2Creply[0] = 0x99;
        replyLength = 1;
      } else if(MP3player.isPlaying() == 0 || paused == FALSE) {
		//Then we're not playing back anything and therefore can't pause it.
        I2Creply[0] = 0x33;
        replyLength = 1;
      }
    } else if(receivedI2C == 0x06) {
	//Then we've recieved a stop playback command.
      if (recordingState == 0 && MP3player.isPlaying() == 1) {
		//Then we're not recording anything but are playing something back, so can stop it.
        I2Creply[0] = 0x77;
        replyLength = 1;
        MP3player.stopTrack();
      } else if (recordingState == 0 && MP3player.isPlaying() == 0){
		//Then we're not playing back anthing, so can't stop it.
        I2Creply[0] = 0x33;
        replyLength = 1;
      } else {
		//Then we're currently recording, so can't stop playback.
        I2Creply[0] = 0x99;
        replyLength = 1;
      }
    }
  }
  
  
	uint16_t result = 0;

	// If recorder is active, read data blocks from VS1053b chipset and
	// write them to track file.
	if (MP3player.isRecording()){
		result = MP3player.doRecordOgg();
	}
}

// Initialises the VS1053b chip and loads in the necessary codec plugin.
void StartMP3(){  
	uint8_t result; //result code,
	result = MP3player.begin();
}

// Interrupt service routine for when a read request is sent on the I2C bus.
void requestEvent() {
	//Writes back to I2C whatever data has been set in I2Creply.
  int i = 0;
   Wire.write(I2Creply, replyLength);
 
}

// Interrupt service routine for when data is received on the I2C bus.
void receiveEvent(int howMany) {
  if (Wire.available() > 1) {
	// Then we have a command followed by some parameters, so we set the appropriate flags and read the first byte.
    newI2C = TRUE;
    moreI2C = TRUE;
    receivedI2C = Wire.read();
  } else if(Wire.available() == 1) {
	//Then we have a command with no parameters, so we set the appropriate flags and read the command.
    newI2C = TRUE;
    moreI2C = FALSE;
    receivedI2C = Wire.read();
  }
}  


