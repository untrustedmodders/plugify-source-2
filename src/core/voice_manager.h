#pragma once

#include <public/eiface.h>
#include <dynohook/dynohook.h>

class VoiceManager
{
public:
	VoiceManager() = delete;
	~VoiceManager() = delete;

	static dyno::ReturnAction Hook_SetClientListening(dyno::IHook& hook);
};