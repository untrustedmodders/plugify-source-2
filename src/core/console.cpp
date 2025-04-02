#include <tier0/logging.h>
#include <convar.h>

plg::string g_ServerCommandBuffer;
bool g_ShouldCatchSpew = false;

class LoggingListener final : public ILoggingListener {
public:
	void Log(const LoggingContext_t* pContext, const tchar* pMessage) override {
		g_ServerCommandBuffer = pMessage;
	}
} g_LoggingListener;

CON_COMMAND_F(s2_conhook_start, "", FCVAR_PROTECTED) {
	if (!g_ShouldCatchSpew)
		return;

	LoggingSystem_PushLoggingState(false, false);
	LoggingSystem_RegisterLoggingListener(&g_LoggingListener);
}

CON_COMMAND_F(s2_conhook_stop, "", FCVAR_PROTECTED) {
	if (!g_ShouldCatchSpew)
		return;

	LoggingSystem_PopLoggingState(false);
	g_ShouldCatchSpew = false;
}
