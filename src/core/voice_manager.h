#pragma once

#include <dynohook/dynohook.h>
#include <public/eiface.h>

class CVoiceManager
{
public:
	CVoiceManager() = delete;
	~CVoiceManager() = delete;

	static dyno::ReturnAction Hook_SetClientListening(dyno::IHook& hook);
};