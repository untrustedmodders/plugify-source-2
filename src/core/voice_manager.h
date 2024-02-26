#pragma once

#include <dynohook/dynohook.h>
#include <public/eiface.h>

class VoiceManager
{
public:
	VoiceManager() = delete;
	~VoiceManager() = delete;

	static dyno::ReturnAction Hook_SetClientListening(dyno::IHook& hook);
};