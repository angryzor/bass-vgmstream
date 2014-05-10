#include "bass_vgmstream.h"

#include <vgmstream.h>
#include <stdlib.h>

typedef struct VSTREAM {
	VGMSTREAM* vgmstream;
	DWORD decode_pos;
	DWORD stream_length_samples;
} VSTREAM;

DWORD CALLBACK vgmStreamProc(
    HSTREAM handle,
    void *buffer,
    DWORD length,
    void *user
)
{
	VSTREAM* vs = (VSTREAM*)user;
	BOOL ended = FALSE;
	int max_samples = length/sizeof(sample)/vs->vgmstream->channels;
	int samples_to_do;
	
	if (vs->decode_pos + max_samples > vs->stream_length_samples && !vs->vgmstream->loop_flag)
	{
		samples_to_do = vs->stream_length_samples - vs->decode_pos;
		ended = TRUE;
	}
	else
		samples_to_do = max_samples;
	render_vgmstream((sample*)buffer, samples_to_do, vs->vgmstream);

	if(!vs->vgmstream->loop_flag)
		vs->decode_pos += samples_to_do;

	samples_to_do *= sizeof(sample) * vs->vgmstream->channels;
	if(ended)
	{
		if(BASS_ChannelFlags(handle,0,0) & BASS_SAMPLE_LOOP)
		{
			reset_vgmstream(vs->vgmstream);
			vs->decode_pos = 0;
		}
		else
			samples_to_do |= BASS_STREAMPROC_END;
	}

	return samples_to_do;
}

void CALLBACK vgmStreamOnFree(
    HSYNC handle,
    DWORD channel,
    DWORD data,
    void *user
)
{
	VSTREAM* vs = (VSTREAM*)user;
	close_vgmstream(vs->vgmstream);
	free(vs);
}

BASS_VGMSTREAM_API HSTREAM BASS_VGMSTREAM_StreamCreate(const char* file, DWORD flags)
{
	VSTREAM* vs;
	HSTREAM h;
	VGMSTREAM* v = init_vgmstream(file);
	if(!v)
		return 0;

	vs = (VSTREAM*)malloc(sizeof(VSTREAM));
	vs->vgmstream = v;
	vs->decode_pos = 0;
	vs->stream_length_samples = get_vgmstream_play_samples(0,0,0,v);
	h = BASS_StreamCreate(v->sample_rate, v->channels, flags, &vgmStreamProc, vs);
	if(!h)
		return 0;

	BASS_ChannelSetSync(h, BASS_SYNC_FREE|BASS_SYNC_MIXTIME, 0, &vgmStreamOnFree, vs);
	return h;
}
