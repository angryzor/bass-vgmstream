/**
 * Copyright (c) 2008, Ruben "angryzor" Tytgat
 * 
 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "bass_vgmstream.h"

#include <vgmstream.h>
#include <stdlib.h>

/**
 * Callback for BASS. Called when it needs more data.
 */
DWORD CALLBACK vgmStreamProc(
    HSTREAM handle,
    void *buffer,
    DWORD length,
    void *user
)
{
	VGMSTREAM* stream = (VGMSTREAM*)user;                         // We passed the VGMSTREAM as user data.
	BOOL ended = FALSE;                                           // Used to signal end of stream.
	int max_samples = length / sizeof(sample) / stream->channels; // Calculate the maximum amount of samples from max buffer size.
	int samples_to_do;                                            // Will hold the amount of samples to be copied.
	
	// If this is a looping VGM stream, we handle it as an infinite stream and read out the
	// next `max_samples` of data.
	//
	// Otherwise, we read out at most `max_samples`. If there is less data available,
	// we consider the stream ended and signal this to BASS
	if (!stream->loop_flag && stream->current_sample + max_samples > stream->num_samples) {
		samples_to_do = stream->num_samples - stream->current_sample;
		ended = TRUE;
	}
	else
		samples_to_do = max_samples;

	// Render the stream.
	render_vgmstream((sample*)buffer, samples_to_do, stream);
	// BASS expects you to return the amount of data read in bytes, so multiply by the sample size
	samples_to_do *= sizeof(sample) * stream->channels;

	// If we reached the end of a non-looping VGM stream, we'll check BASS' loop flag.
	// If it is set, we restart from the beginning. Otherwise, we signal the end of stream
	// to BASS.
	if(ended) {
		if(BASS_ChannelFlags(handle,0,0) & BASS_SAMPLE_LOOP)
			reset_vgmstream(stream);
		else
			samples_to_do |= BASS_STREAMPROC_END;
	}

	return samples_to_do;
}

/**
 * Called when the BASS handle is closed
 */
void CALLBACK vgmStreamOnFree(
    HSYNC handle,
    DWORD channel,
    DWORD data,
    void *user
)
{
	VGMSTREAM* stream = (VGMSTREAM*)user;
	close_vgmstream(stream);
}

BASS_VGMSTREAM_API HSTREAM BASS_VGMSTREAM_StreamCreate(const char* file, DWORD flags)
{
	HSTREAM h;
	VGMSTREAM* stream = init_vgmstream(file);
	if(!stream)
		return 0;

	h = BASS_StreamCreate(stream->sample_rate, stream->channels, flags, &vgmStreamProc, stream);
	if(!h)
		return 0;

	BASS_ChannelSetSync(h, BASS_SYNC_FREE|BASS_SYNC_MIXTIME, 0, &vgmStreamOnFree, stream);
	return h;
}
