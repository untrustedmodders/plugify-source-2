#pragma once

#include <plugify/polyhook.h>
#include <eiface.h>

class CVoiceManager
{
public:
	CVoiceManager() = delete;
	~CVoiceManager() = delete;

	static poly::ReturnAction Hook_SetClientListening(poly::Params& params, int count, poly::Return& ret);
};