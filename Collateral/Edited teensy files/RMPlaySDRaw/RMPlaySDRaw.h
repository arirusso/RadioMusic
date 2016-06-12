/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef RMPlaySDraw_h_
#define RMPlaySDraw_h_

#include "AudioStream.h"
#include "SD.h"
#define AUDIOBUFSIZE (AUDIO_BLOCK_SAMPLES * 8)

class RMPlaySDRaw : public AudioStream
{
public:
	RMPlaySDRaw(void) : AudioStream(0, NULL) { begin(); }
	void begin(void);
  void fillBuffer(void);
  void fillBlockFromBuffer(audio_block_t *block);
	bool play(const char *filename);
	bool playFrom(const char *filename, unsigned long start);
	bool preparePlayFrom(const char *filename);
	void stop(void);
	void pause(void);
	bool isPlaying(void) { return playing; }
	uint32_t positionMillis(void);
	uint32_t lengthMillis(void);
	uint32_t fileOffset(void);
	virtual void update(void);

    	void (*hotswap_cb)();

  // speed functions
  void addFrame(int16_t *table, uint16_t at_index, int16_t frame, uint16_t length);
  void set_speed(int8_t speed) { speed_ = speed; }
private:
	File rawfile;
	uint32_t file_size;
	volatile uint32_t file_offset;
	volatile bool playing;
	unsigned char audioBuffer[AUDIOBUFSIZE]; //
	volatile int bufAvail = 0;
	volatile int bufPos = 0;
	char _filePath[16];
  int8_t speed_ = 0;
};

#endif
