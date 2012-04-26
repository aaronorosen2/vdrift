#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#include "soundbuffer.h"
#include "soundfilter.h"
#include "mathvector.h"
#include "quaternion.h"
#include "memory.h"
#include <cassert>

class SOUNDSOURCE
{
public:
	SOUNDSOURCE();

	void SetBuffer(std::tr1::shared_ptr<SOUNDBUFFER> newbuf)
	{
		buffer = newbuf;
		samples_per_channel = buffer->GetInfo().samples / buffer->GetInfo().channels;
		samples = samples_per_channel * buffer->GetInfo().channels;
		Loop(false);
		Stop();
	}

	void SampleAndAdvanceWithPitch16bit(int * chan1, int * chan2, int len);

	void AdvanceWithPitch(int len);

	void SeekToSample(int n)
	{
		assert(buffer);
		assert(n * buffer->GetInfo().channels < buffer->GetInfo().samples);
		sample_pos = n;
		sample_pos_remainder = 0;
	}

	void SetAutoDelete(bool value)
	{
		autodelete = value;
	}

	bool GetAutoDelete() const
	{
		return autodelete;
	}

	void SetGain(float value)
	{
		gain = value;
	}

	void SetPitch(float value)
	{
		pitch = value * denom;
	}

	void SetPosition(float x, float y, float z)
	{
		position[0] = x;
		position[1] = y;
		position[2] = z;
	}

	const MATHVECTOR <float, 3> & GetPosition() const
	{
		return position;
	}

	void SetVelocity(float x, float y, float z)
	{
		velocity[0] = x;
		velocity[1] = y;
		velocity[2] = z;
	}

	const MATHVECTOR <float, 3> & GetVelocity() const
	{
		return velocity;
	}

	void Enable3D(bool value)
	{
		effects3d = value;
	}

	bool Get3DEffects() const
	{
		return effects3d;
	}

	void SetComputedGain(float cpg1, float cpg2)
	{
		computed_gain1 = cpg1 * denom;
		computed_gain2 = cpg2 * denom;
	}

	float GetGain() const
	{
		return gain;
	}

	void Loop(bool value)
	{
		loop = value;
	}

	void Reset()
	{
		sample_pos = 0;
		sample_pos_remainder = 0;
		last_computed_gain1 = 0;
		last_computed_gain2 = 0;
	}

	void Stop()
	{
		Pause();
		Reset();
	}

	void Pause()
	{
		playing = false;
	}

	void Play()
	{
		playing = true;
	}

	bool Audible() const
	{
		return playing && (gain > 0);
	}

	SOUNDFILTER & AddFilter()
	{
		filters.push_back(SOUNDFILTER());
		return filters.back();
	}

	SOUNDFILTER & GetFilter(unsigned n)
	{
		assert(n < filters.size());
		return filters[n];
	}

	int NumFilters() const
	{
		return filters.size();
	}

	void ClearFilters()
	{
		filters.clear();
	}

	bool operator<(const SOUNDSOURCE & other) const
	{
		return std::max(computed_gain1, computed_gain2) <
			std::max(other.computed_gain1, other.computed_gain2);
	}

private:
	static const int denom = 1 << 15;
	static const int max_gain_delta = (denom * 100) / 44100;

	int samples_per_channel;
	int samples;
	int sample_pos;
	int sample_pos_remainder;
	int pitch;
	int computed_gain1;
	int computed_gain2;
	int last_computed_gain1;
	int last_computed_gain2;

	bool playing;
	bool loop;
	bool autodelete;
	bool effects3d;

	MATHVECTOR <float, 3> position;
	MATHVECTOR <float, 3> velocity;
	float gain;

	std::tr1::shared_ptr<SOUNDBUFFER> buffer;
	std::vector<SOUNDFILTER> filters;
};

#endif // SOUNDSOURCE_H
