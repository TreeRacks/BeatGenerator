// Incomplete implementation of an audio mixer. Search for "REVISIT" to find things
// which are left as incomplete.
// Note: Generates low latency audio on BeagleBone Black; higher latency found on host.
#include "audioMixer.h"
#include "intervalTimer.h"
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <alloca.h> // needed for mixer


static snd_pcm_t *handle;
// static wavedata_t drum, snare, highHat;
// #define drumSound "sounds/100051__menegass__gui-drum-bd-hard.wav"
// #define snareSound "sounds/100058__menegass__gui-drum-snare-hard.wav"
// #define highHatSound "sounds/100062__menegass__gui-drum-tom-hi-hard.wav"

#define DEFAULT_VOLUME 80

#define SAMPLE_RATE 44100 //every 0.1 seconds send out a playbackbuffer
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (sizeof(short)) 			// bytes per sample
// Sample size note: This works for mono files because each sample ("frame') is 1 value.
// If using stereo files then a frame would be two samples.

static unsigned long playbackBufferSize = 0;
static short *playbackBuffer = NULL;
static Interval_statistics_t *mixerStatistics;

// Currently active (waiting to be played) sound bites
#define MAX_SOUND_BITES 30
typedef struct {
	// A pointer to a previously allocated sound bite (wavedata_t struct).
	// Note that many different sound-bite slots could share the same pointer
	// (overlapping cymbal crashes, for example)
	wavedata_t *pSound;

	// The offset into the pData of pSound. Indicates how much of the
	// sound has already been played (and hence where to start playing next).
	int location;
} playbackSound_t;
static playbackSound_t soundBites[MAX_SOUND_BITES];

// Playback threading
void* playbackThread(void* arg);
static _Bool stopping = false;
static pthread_t playbackThreadId;
static pthread_mutex_t audioMutex = PTHREAD_MUTEX_INITIALIZER;

static int volume = 0;

void AudioMixer_init(void)
{
	mixerStatistics = malloc(sizeof(*mixerStatistics));
	AudioMixer_setVolume(DEFAULT_VOLUME);

	// Initialize the currently active sound-bites being played
	// REVISIT:- Implement this. Hint: set the pSound pointer to NULL for each
	//     sound bite.

	// AudioMixer_readWaveFileIntoMemory(drumSound, &drum);
	// AudioMixer_readWaveFileIntoMemory(snareSound, &snare);
	// AudioMixer_readWaveFileIntoMemory(highHatSound, &highHat);

	//set everything back to NULL and zero
	for(int i = 0; i < MAX_SOUND_BITES; i++){
		soundBites[i].pSound = NULL;
		soundBites[i].location = 0;
	}

	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
 	unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

	// Launch playback thread:
	pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
}


// Client code must call AudioMixer_freeWaveFileData to free dynamically allocated data.
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound)
{
	assert(pSound);

	// The PCM data in a wave file starts after the header:
	const int PCM_DATA_OFFSET = 44;

	// Open the wave file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
	pSound->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Search to the start of the data in the file
	fseek(file, PCM_DATA_OFFSET, SEEK_SET);

	// Allocate space to hold all PCM data
	pSound->pData = malloc(sizeInBytes);
	if (pSound->pData == 0) {
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
				sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read PCM data from wave file into memory
	int samplesRead = fread(pSound->pData, SAMPLE_SIZE, pSound->numSamples, file);
	if (samplesRead != pSound->numSamples) {
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				pSound->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}
}

void AudioMixer_freeWaveFileData(wavedata_t *pSound)
{
	pSound->numSamples = 0;
	free(pSound->pData);
	pSound->pData = NULL;
}

void AudioMixer_queueSound(wavedata_t *pSound)
{
	// Ensure we are only being asked to play "good" sounds:
	assert(pSound->numSamples > 0);
	assert(pSound->pData);

	// Insert the sound by searching for an empty sound bite spot
	/*
	 * REVISIT: Implement this:
	 * 1. Since this may be called by other threads, and there is a thread
	 *    processing the soundBites[] array, we must ensure access is threadsafe.
	 * 2. Search through the soundBites[] array looking for a free slot.
	 * 3. If a free slot is found, place the new sound file into that slot.
	 *    Note: You are only copying a pointer, not the entire data of the wave file!
	 * 4. After searching through all slots, if no free slot is found then print
	 *    an error message to the console (and likely just return vs asserting/exiting
	 *    because the application most likely doesn't want to crash just for
	 *    not being able to play another wave file.
	 */

		int numberOfFreeSpaces = MAX_SOUND_BITES;
		for(int i =0; i < MAX_SOUND_BITES; i++){
			if(soundBites[i].pSound == NULL){
				pthread_mutex_lock(&audioMutex);
				{
				soundBites[i].pSound = pSound;
				soundBites[i].pSound->pData = pSound->pData;
				// for(int j=0;j<500; j++){
				// 	printf("pdata: %d\n",soundBites[i].pSound->pData[j]);
				// }
				soundBites[i].pSound->numSamples = pSound->numSamples;
				}
				pthread_mutex_unlock(&audioMutex);
				numberOfFreeSpaces--;
				return;
			} 
		}
		if(numberOfFreeSpaces == 0){
			printf("No space found in queue!\n");
			return;
		}
}

void AudioMixer_cleanup(void)
{
	printf("Stopping audio...\n");

	// Stop the PCM generation thread
	stopping = true;
	pthread_join(playbackThreadId, NULL);

	// Shutdown the PCM output, allowing any pending sound to play out (drain)
	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	// Free playback buffer
	// (note that any wave files read into wavedata_t records must be freed
	//  in addition to this by calling AudioMixer_freeWaveFileData() on that struct.)
	free(playbackBuffer);
	playbackBuffer = NULL;

	printf("Done stopping audio...\n");
	fflush(stdout);
}


int AudioMixer_getVolume()
{
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
void AudioMixer_setVolume(int newVolume)
{
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
		printf("ERROR: Volume must be between 0 and 100.\n");
		return;
	}
	volume = newVolume;

    long min, max;
    snd_mixer_t *mixerHandle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    // const char *selem_name = "PCM";	// For ZEN cape
    const char *selem_name = "Speaker";	// For USB Audio

    snd_mixer_open(&mixerHandle, 0);
    snd_mixer_attach(mixerHandle, card);
    snd_mixer_selem_register(mixerHandle, NULL, NULL);
    snd_mixer_load(mixerHandle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(mixerHandle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(mixerHandle);
}


// Fill the buff array with new PCM values to output.
//    buff: buffer to fill with new PCM data from sound bites.
//    size: the number of *values* to store into buff
static void fillPlaybackBuffer(short *buff, int size)
{
	memset(buff, 0, sizeof(*buff));
	/*
	 * REVISIT: Implement this
	 * 1. Wipe the buff to all 0's to clear any previous PCM data.
	 *    Hint: use memset(); read the docs about its use of size.
	 * 2. Since this is called from a background thread, and soundBites[] array
	 *    may be used by any other thread, must synchronize this.
	 * 3. Loop through each slot in soundBites[], which are sounds that are either
	 *    waiting to be played, or partially already played:
	 *    - If the sound bite slot is unused, do nothing for this slot.
	 *    - Otherwise "add" this sound bite's data to the play-back buffer
	 *      (other sound bites needing to be played back will also add to the same data).
	 *      * Record that this portion of the sound bite has been played back by incrementing
	 *        the location inside the data where play-back currently is.
	 *      * If you have now played back the entire sample, free the slot in the
	 *        soundBites[] array.
	 *
	 * Notes on "adding" PCM samples:
	 * - PCM is stored as signed shorts (between SHRT_MIN and SHRT_MAX).
	 * - When adding values, ensure there is not an overflow. Any values which would
	 *   greater than SHRT_MAX should be clipped to SHRT_MAX; likewise for underflow.
	 * - Don't overflow any arrays!
	 * - Efficiency matters here! The compiler may do quite a bit for you, but it doesn't
	 *   hurt to keep it in mind. Here are some tips for efficiency and readability:
	 *   * If, for each pass of the loop which "adds" you need to change a value inside
	 *     a struct inside an array, it may be faster to first load the value into a local
	 *      variable, increment this variable as needed throughout the loop, and then write it
	 *     back into the struct inside the array after. For example:
	 *           int offset = myArray[someIdx].value;
	 *           for (int i =...; i < ...; i++) {
	 *               offset ++;
	 *           }
	 *           myArray[someIdx].value = offset;
	 *   * If you need a value in a number of places, try loading it into a local variable
	 *          int someNum = myArray[someIdx].value;
	 *          if (someNum < X || someNum > Y || someNum != Z) {
	 *              someNum = 42;
	 *          }
	 *          ... use someNum vs myArray[someIdx].value;
	 *
	 */
	pthread_mutex_lock(&audioMutex);
	{
		int *temp;
		temp = malloc(size * sizeof(*temp)); //make some space for temp
		memset(temp, 0, sizeof(*temp)); //clean it up before using
		for(int j = 0; j < size; j++){
			
			for(int i =0; i < MAX_SOUND_BITES; i++){ //soundBites
				if(soundBites[i].pSound == NULL){
					break;
				}
				if(soundBites[i].location < soundBites[i].pSound->numSamples){ // if the location of our current sound is less than its total samples...
					//printf("location is: %d\n", soundBites[i].location);
					if(soundBites[i].pSound->pData[soundBites[i].location] != 0){ // ...and if the data at that location exists (is not NULL)...
						temp[j] += soundBites[i].pSound->pData[soundBites[i].location]; //...add them up in an integer array. 
					}
					if(temp[j] > SHRT_MAX){ // do the clipping here
						temp[j] = SHRT_MAX;
					}
					if(temp[j] < SHRT_MIN){
						temp[j] = SHRT_MIN;
					}
					soundBites[i].location +=1; // increment the location 
				} else if(soundBites[i].location == soundBites[i].pSound->numSamples){ // if the location of our sound has reached the number of total samples, it has ended
					soundBites[i].location = 0; // set it back to 0
					soundBites[i].pSound = NULL;
					//AudioMixer_freeWaveFileData(soundBites[i].pSound); //don't wanna free this here
				}
			}
			buff[j] = (short)temp[j];
		}
	}
	pthread_mutex_unlock(&audioMutex);


}

double AudioMixer_getMinInterval(){
	return mixerStatistics->minIntervalInMs;
}
double AudioMixer_getMaxInterval(){
	return mixerStatistics->maxIntervalInMs;
}
double AudioMixer_getAvgInterval(){
	return mixerStatistics->avgIntervalInMs;
}
int AudioMixer_getNumSamplesInterval(){
	return mixerStatistics->numSamples;
}

void* playbackThread(void* _arg)
{

	
	while (!stopping) {
		// Generate next block of audio
		fillPlaybackBuffer(playbackBuffer, playbackBufferSize);

		Interval_markInterval(INTERVAL_LOW_LEVEL_AUDIO);
		Interval_getStatisticsAndClear(INTERVAL_LOW_LEVEL_AUDIO, mixerStatistics);
		
		// Output the audio
		snd_pcm_sframes_t frames = snd_pcm_writei(handle,
				playbackBuffer, playbackBufferSize);

		// Check for (and handle) possible error conditions on output
		if (frames < 0) {
			fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
			frames = snd_pcm_recover(handle, frames, 1);
		}
		if (frames < 0) {
			fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
					frames);
			exit(EXIT_FAILURE);
		}
		if (frames > 0 && frames < playbackBufferSize) {
			printf("Short write (expected %li, wrote %li)\n",
					playbackBufferSize, frames);
		}
	}

	return NULL;
}
