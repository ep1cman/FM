/*
 * File:   RECORDER.h
 * Author: Phil
 *
 * Created on 27 April 2015, 02:26
 */

#ifndef RECORDER_H
#define	RECORDER_H

#define RECORDER_ADDR 0x6e

// Starts recording, saving it at the given file number (0x01-0x0A). 
// 'rtn' set to:
// 0x77 on recording successfully started.
// 0x33 on recording failed.
// 0x99 on recording already in progress.

void startRecording(unsigned char fileNumber);


// Stops a recording which is currently in progress.
// 'rtn' set to:
// 0x77 on recording successfully stopped.
// 0x33 on recording not stopped.
// 0x99 on no recording in progress to stop.

void stopRecording();


// Plays back a recording with the given file number (0x01-0x0A).
// 'rtn' set to:
// 0x77 on file successfully playing.
// 0x33 on file failed playing.
// 0x99 on recording currently in progress.

void playRecording(unsigned char fileNumber);


// Toggles the pause on a file currently being played. Call to pause, call again to unpause.
// 'rtn' set to:
// 0x77 on file successfully paused/unpaused.
// 0x33 on nothing playing to be paused.
// 0x99 on recording currently in progress.

void pausePlayback();


// Stops current playback entirely (not resumable).
// 'rtn' set to:
// 0x77 on playback successfully stopped.
// 0x33 on nothing playing to be stopped.
// 0x99 on recording currently in progress.

void stopPlayback();


#endif	/* RECORDER_H */

