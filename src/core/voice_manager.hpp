#pragma once

#include <eiface.h>
#include <plugify/polyhook.hpp>

class CVoiceManager {
public:
	CVoiceManager() = delete;
	~CVoiceManager() = delete;

	static poly::ReturnAction Hook_SetClientListening(poly::Params& params, int count, poly::Return& ret);
};