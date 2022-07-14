#include "audio.h"

#include "AL/al.h"
#include "AL/alext.h"
#include "AL/alc.h"

namespace FlatEngine{

	static ALCdevice* s_AudioDevice = nullptr;

}