#include <core/player_manager.hpp>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Sets the listening state of a client with respect to another client.
 *
 * This function allows you to specify whether a receiver client should
 * listen to a sender client. The listening state can be controlled
 * through the `listen` parameter which can enable or disable the
 * listening of the sender by the receiver.
 *
 * @param receiverIndex The index of the client who will receive the audio.
 * @param senderIndex The index of the client who will be sending audio.
 * @param listen The listening state override for the receiver with respect to the sender.
 */
extern "C" PLUGIN_API void SetClientListening(int receiverIndex, int senderIndex, int8_t listen) {
	if (senderIndex < 0 || senderIndex >= CPlayerManager::MaxClients()) {
		g_Logger.Log(LS_WARNING, "Invalid sender\n");
		return;
	}

	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(receiverIndex));
	if (pPlayer == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid receiver\n");
		return;
	}

	pPlayer->SetListen(CPlayerSlot(senderIndex), static_cast<ListenOverride>(listen));
}

/**
 * @brief Retrieves the listening state of a client with respect to another client.
 *
 * This function returns the current listening state of a receiver client
 * for a specific sender client. It checks if the sender is valid and returns
 * the listening state if valid, or a default value if invalid.
 *
 * @param receiverIndex The index of the client who receives the audio.
 * @param senderIndex The index of the client who sends the audio.
 * @return The listening state override for the receiver with respect to the sender.
 */
extern "C" PLUGIN_API int8_t GetClientListening(int receiverIndex, int senderIndex) {
	if (senderIndex < 0 || senderIndex >= CPlayerManager::MaxClients()) {
		g_Logger.Log(LS_WARNING, "Invalid sender\n");
		return Listen_Default;
	}

	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(receiverIndex));
	if (pPlayer == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid receiver\n");
		return Listen_Default;
	}

	return pPlayer->GetListen(CPlayerSlot(senderIndex));
}

/**
 * @brief Sets the voice flags for a specified client.
 *
 * This function allows you to set the voice-related flags for a client,
 * which can determine aspects of voice communication such as muting or enabling
 * the client’s voice transmission.
 *
 * @param clientIndex The index of the client for whom the voice flags are being set.
 * @param flags The voice flags to be set for the client.
 */
extern "C" PLUGIN_API void SetClientVoiceFlags(int clientIndex, uint8_t flags) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(clientIndex));
	if (pPlayer == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid receiver\n");
		return;
	}

	pPlayer->SetVoiceFlags(flags);
}

/**
 * @brief Retrieves the voice flags for a specified client.
 *
 * This function returns the current voice flags for a specified client.
 * If the client is invalid, it returns zero as the default.
 *
 * @param clientIndex The index of the client for whom the voice flags are being retrieved.
 * @return The current voice flags for the specified client.
 */
extern "C" PLUGIN_API uint8_t GetClientVoiceFlags(int clientIndex) {
	auto pPlayer = g_PlayerManager.ToPlayer(CPlayerSlot(clientIndex));
	if (pPlayer == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid receiver\n");
		return 0;
	}

	return pPlayer->GetVoiceFlags();
}

PLUGIFY_WARN_POP()
