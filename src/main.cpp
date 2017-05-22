#include <utki/math.hpp>
#include <nitki/Thread.hpp>

#include <audout/Player.hpp>

#include "libayemu/ayemu.h"




struct SinePlayer : public audout::Listener{
//	double time = 0;

	audout::AudioFormat format;
	
	ayemu_vtx_t *vtx;
	
	std::vector<std::uint8_t> audioBuf;
	
	ayemu_ay_t ay;
	
	size_t pos = 0;

	decltype(audioBuf)::iterator srcIter;
	
	void fillPlayBuf(utki::Buf<std::int16_t> buf)noexcept override{
//		TRACE_ALWAYS(<< "filling smp buf" << std::endl)


		for (
				auto i = buf.begin(), e = buf.end();
				i != e && this->pos < vtx->frames;
				++this->pos
			)
		{
			ayemu_ay_reg_frame_t regs;
			ayemu_vtx_getframe (this->vtx, this->pos, regs);
			ayemu_set_regs (&this->ay, regs);
			ayemu_gen_sound (&this->ay, &*this->audioBuf.begin(), this->audioBuf.size());
			
			ASSERT_INFO(this->audioBuf.size() % 2 == 0, "this->audioBuf.size() = " << this->audioBuf.size())
			
			for(; i != e; ++i){
				if(this->srcIter == this->audioBuf.end()){
					this->srcIter = this->audioBuf.begin();
					break;
				}
				ASSERT(this->srcIter != this->audioBuf.end())
				ASSERT(this->srcIter + 1 != this->audioBuf.end())
				*i = std::int16_t(utki::deserialize16LE(&*this->srcIter));
				++this->srcIter;
				ASSERT(this->srcIter != this->audioBuf.end())
				++this->srcIter;
			}
		}
		
//		TRACE_ALWAYS(<< "time = " << this->time << std::endl)
//		TRACE(<< "this->smpBuf = " << buf << std::endl)
	}
	
	SinePlayer(audout::AudioFormat format) :
			format(format)
	{
		if((this->vtx = ayemu_vtx_load_from_file("dizzy5.vtx"))){}else{
			throw utki::Exc("could not open VTX file");
		}
		
		TRACE(<< "this->format.frequency() = " << this->format.frequency()
				<< ", this->format.numChannels() = " << this->format.numChannels()
				<< ", this->vtx->playerFreq = " << this->vtx->playerFreq
				<< std::endl)
		
		size_t audio_bufsize = this->format.frequency() * this->format.numChannels() * 2 / this->vtx->playerFreq;
		audio_bufsize += audio_bufsize % 2;
		this->audioBuf.resize(audio_bufsize);
		this->srcIter = this->audioBuf.begin();
		
		ayemu_init(&this->ay);
		if(!ayemu_set_sound_format(&this->ay, this->format.frequency(), this->format.numChannels(), 16)){
			throw utki::Exc("could not set AY sound format");
		}
		
		ayemu_reset(&this->ay);
		if(!ayemu_set_chip_type(&this->ay, this->vtx->chiptype, NULL)){
			throw utki::Exc("could not set AY chip type");
		}
		ayemu_set_chip_freq(&this->ay, this->vtx->chipFreq);
		if(!ayemu_set_stereo(&this->ay, ayemu_stereo_t(this->vtx->stereo), NULL)){
			throw utki::Exc("could not set AY stereo");
		}
	}
	
	~SinePlayer()noexcept{
		ayemu_vtx_free(this->vtx);
	}
};

void play(audout::AudioFormat format){
	SinePlayer pl(format);
	audout::Player p(format, 1000, &pl);
	p.setPaused(false);

	nitki::Thread::sleep(200000);
}


int main(int argc, char *argv[]){
	{
		TRACE_ALWAYS(<< "Opening audio playback device: Mono 11025" << std::endl)
		play(audout::AudioFormat(audout::Frame_e::MONO, audout::SamplingRate_e::HZ_11025));
		TRACE_ALWAYS(<< "finished playing" << std::endl)
	}
	
//	{
//		TRACE_ALWAYS(<< "Opening audio playback device: Stereo 11025" << std::endl)
//		play(audout::AudioFormat(audout::Frame_e::STEREO, audout::SamplingRate_e::HZ_11025));
//	}
//	
//	{
//		TRACE_ALWAYS(<< "Opening audio playback device: Mono 22050" << std::endl)
//		play(audout::AudioFormat(audout::Frame_e::MONO, audout::SamplingRate_e::HZ_22050));
//	}
//	
//	{
//		TRACE_ALWAYS(<< "Opening audio playback device: Stereo 22050" << std::endl)
//		play(audout::AudioFormat(audout::Frame_e::STEREO, audout::SamplingRate_e::HZ_22050));
//	}
//	
//	{
//		TRACE_ALWAYS(<< "Opening audio playback device: Mono 44100" << std::endl)
//		play(audout::AudioFormat(audout::Frame_e::MONO, audout::SamplingRate_e::HZ_44100));
//	}
//	
//	{
//		TRACE_ALWAYS(<< "Opening audio playback device: Stereo 44100" << std::endl)
//		play(audout::AudioFormat(audout::Frame_e::STEREO, audout::SamplingRate_e::HZ_44100));
//	}
//	
//	{
//		TRACE_ALWAYS(<< "Opening audio playback device: Mono 48000" << std::endl)
//		play(audout::AudioFormat(audout::Frame_e::MONO, audout::SamplingRate_e::HZ_48000));
//	}
//	
	{
		TRACE_ALWAYS(<< "Opening audio playback device: Stereo 48000" << std::endl)
		play(audout::AudioFormat(audout::Frame_e::STEREO, audout::SamplingRate_e::HZ_48000));
	}
	
	return 0;
}
