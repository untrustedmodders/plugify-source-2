#include <core/user_message.hpp>
#include <core/user_message_manager.hpp>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Hooks a user message with a callback.
 *
 * @param messageId The ID of the message to hook.
 * @param callback The callback function to invoke when the message is received.
 * @param mode Whether to hook the message in the post mode (after processing) or pre mode (before processing).
 * @return True if the hook was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool HookUserMessage(int messageId, UserMessageCallback callback, HookMode mode) {
	return g_UserMessageManager.HookUserMessage(messageId, callback, mode);
}

/**
 * @brief Unhooks a previously hooked user message.
 *
 * @param messageId The ID of the message to unhook.
 * @param callback The callback function to remove.
 * @param mode Whether the hook was in post mode (after processing) or pre mode (before processing).
 * @return True if the hook was successfully removed, false otherwise.
 */
extern "C" PLUGIN_API bool UnhookUserMessage(int messageId, UserMessageCallback callback, HookMode mode) {
	return g_UserMessageManager.UnhookUserMessage(messageId, callback, mode);
}

/**
 * @brief Creates a UserMessage from a serializable message.
 *
 * @param msgSerializable The serializable message.
 * @param message The network message.
 * @param nRecipientCount The number of recipients.
 * @param recipientMask The recipient mask.
 * @return A pointer to the newly created UserMessage.
 */
extern "C" PLUGIN_API UserMessage* UserMessage_CreateFromSerializable(INetworkMessageInternal* msgSerializable, const CNetMessage* message, int nRecipientCount, uint64* recipientMask) {
	return new UserMessage(msgSerializable, message, nRecipientCount, recipientMask);
}

/**
 * @brief Creates a UserMessage from a message name.
 *
 * @param messageName The name of the message.
 * @return A pointer to the newly created UserMessage.
 */
extern "C" PLUGIN_API UserMessage* UserMessage_CreateFromName(const plg::string& messageName) {
	return new UserMessage(messageName.c_str());
}

/**
 * @brief Creates a UserMessage from a message ID.
 *
 * @param messageId The ID of the message.
 * @return A pointer to the newly created UserMessage.
 */
extern "C" PLUGIN_API UserMessage* UserMessage_CreateFromId(int messageId) {
	return new UserMessage(messageId);
}

/**
 * @brief Destroys a UserMessage and frees its memory.
 *
 * @param userMessage The UserMessage to destroy.
 */
extern "C" PLUGIN_API void UserMessage_Destroy(UserMessage* userMessage) {
	delete userMessage;
}

/**
 * @brief Gets the name of the message.
 *
 * @param userMessage The UserMessage instance.
 * @return The name of the message as a plg::string.
 */
extern "C" PLUGIN_API const plg::string UserMessage_GetMessageName(UserMessage* userMessage) {
	return userMessage->GetMessageName();
}

/**
 * @brief Gets the ID of the message.
 *
 * @param userMessage The UserMessage instance.
 * @return The ID of the message.
 */
extern "C" PLUGIN_API int UserMessage_GetMessageID(UserMessage* userMessage) {
	return userMessage->GetMessageID();
}

/**
 * @brief Checks if the message has a specific field.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field to check.
 * @return True if the field exists, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_HasField(UserMessage* userMessage, const plg::string& fieldName) {
	return userMessage->HasField(fieldName.c_str());
}

/**
 * @brief Gets the protobuf message associated with the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @return A pointer to the protobuf message.
 */
extern "C" PLUGIN_API const void* UserMessage_GetProtobufMessage(UserMessage* userMessage) {
	return userMessage->GetProtobufMessage();
}

/**
 * @brief Gets the serializable message associated with the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @return A pointer to the serializable message.
 */
extern "C" PLUGIN_API void* UserMessage_GetSerializableMessage(UserMessage* userMessage) {
	return userMessage->GetSerializableMessage();
}

/**
 * @brief Gets the recipient mask for the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @return A pointer to the recipient mask.
 */
extern "C" PLUGIN_API uint64* UserMessage_GetRecipientMask(UserMessage* userMessage) {
	return userMessage->GetRecipientMask();
}

/**
 * @brief Checks if the UserMessage was manually allocated.
 *
 * @param userMessage The UserMessage instance.
 * @return True if the message was manually allocated, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_IsManuallyAllocated(UserMessage* userMessage) {
	return userMessage->IsManuallyAllocated();
}

/**
 * @brief Gets an int32 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetInt32(UserMessage* userMessage, const plg::string& fieldName, int32_t* out) {
	return userMessage->GetInt32(fieldName.c_str(), out);
}

/**
 * @brief Sets an int32 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetInt32(UserMessage* userMessage, const plg::string& fieldName, int32_t value) {
	return userMessage->SetInt32(fieldName.c_str(), value);
}

/**
 * @brief Gets an int64 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetInt64(UserMessage* userMessage, const plg::string& fieldName, int64* out) {
	return userMessage->GetInt64(fieldName.c_str(), out);
}

/**
 * @brief Sets an int64 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetInt64(UserMessage* userMessage, const plg::string& fieldName, int64 value) {
	return userMessage->SetInt64(fieldName.c_str(), value);
}

/**
 * @brief Gets a uint32 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetUInt32(UserMessage* userMessage, const plg::string& fieldName, uint32_t* out) {
	return userMessage->GetUInt32(fieldName.c_str(), out);
}

/**
 * @brief Sets a uint32 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetUInt32(UserMessage* userMessage, const plg::string& fieldName, uint32_t value) {
	return userMessage->SetUInt32(fieldName.c_str(), value);
}

/**
 * @brief Gets a uint64 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetUInt64(UserMessage* userMessage, const plg::string& fieldName, uint64* out) {
	return userMessage->GetUInt64(fieldName.c_str(), out);
}

/**
 * @brief Sets a uint64 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetUInt64(UserMessage* userMessage, const plg::string& fieldName, uint64 value) {
	return userMessage->SetUInt64(fieldName.c_str(), value);
}

/**
 * @brief Gets a bool value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetBool(UserMessage* userMessage, const plg::string& fieldName, bool* out) {
	return userMessage->GetBool(fieldName.c_str(), out);
}

/**
 * @brief Sets a bool value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetBool(UserMessage* userMessage, const plg::string& fieldName, bool value) {
	return userMessage->SetBool(fieldName.c_str(), value);
}

/**
 * @brief Gets a float value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetFloat(UserMessage* userMessage, const plg::string& fieldName, float* out) {
	return userMessage->GetFloat(fieldName.c_str(), out);
}

/**
 * @brief Sets a float value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetFloat(UserMessage* userMessage, const plg::string& fieldName, float value) {
	return userMessage->SetFloat(fieldName.c_str(), value);
}

/**
 * @brief Gets a double value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetDouble(UserMessage* userMessage, const plg::string& fieldName, double* out) {
	return userMessage->GetDouble(fieldName.c_str(), out);
}

/**
 * @brief Sets a double value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetDouble(UserMessage* userMessage, const plg::string& fieldName, double value) {
	return userMessage->SetDouble(fieldName.c_str(), value);
}

/**
 * @brief Gets a string value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output string.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetString(UserMessage* userMessage, const plg::string& fieldName, plg::string& out) {
	std::string result;
	bool success = userMessage->GetString(fieldName.c_str(), result);
	if (success) {
		out = result;
	}
	return success;
}

/**
 * @brief Sets a string value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetString(UserMessage* userMessage, const plg::string& fieldName, const plg::string& value) {
	return userMessage->SetString(fieldName.c_str(), std::string(value));
}

/**
 * @brief Gets a repeated int32 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedInt32(UserMessage* userMessage, const plg::string& fieldName, int index, int32_t* out) {
	return userMessage->GetRepeatedInt32(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated int32 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetRepeatedInt32(UserMessage* userMessage, const plg::string& fieldName, int index, int32_t value) {
	return userMessage->SetRepeatedInt32(fieldName.c_str(), index, value);
}

/**
 * @brief Adds an int32 value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddInt32(UserMessage* userMessage, const plg::string& fieldName, int32_t value) {
	return userMessage->AddInt32(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated int64 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedInt64(UserMessage* userMessage, const plg::string& fieldName, int index, int64* out) {
	return userMessage->GetRepeatedInt64(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated int64 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetRepeatedInt64(UserMessage* userMessage, const plg::string& fieldName, int index, int64 value) {
	return userMessage->SetRepeatedInt64(fieldName.c_str(), index, value);
}

/**
 * @brief Adds an int64 value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddInt64(UserMessage* userMessage, const plg::string& fieldName, int64 value) {
	return userMessage->AddInt64(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated uint32 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedUInt32(UserMessage* userMessage, const plg::string& fieldName, int index, uint32_t* out) {
	return userMessage->GetRepeatedUInt32(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated uint32 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetRepeatedUInt32(UserMessage* userMessage, const plg::string& fieldName, int index, uint32_t value) {
	return userMessage->SetRepeatedUInt32(fieldName.c_str(), index, value);
}

/**
 * @brief Adds a uint32 value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddUInt32(UserMessage* userMessage, const plg::string& fieldName, uint32_t value) {
	return userMessage->AddUInt32(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated uint64 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedUInt64(UserMessage* userMessage, const plg::string& fieldName, int index, uint64* out) {
	return userMessage->GetRepeatedUInt64(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated uint64 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetRepeatedUInt64(UserMessage* userMessage, const plg::string& fieldName, int index, uint64 value) {
	return userMessage->SetRepeatedUInt64(fieldName.c_str(), index, value);
}

/**
 * @brief Adds a uint64 value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddUInt64(UserMessage* userMessage, const plg::string& fieldName, uint64 value) {
	return userMessage->AddUInt64(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated bool value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedBool(UserMessage* userMessage, const plg::string& fieldName, int index, bool* out) {
	return userMessage->GetRepeatedBool(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated bool value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetRepeatedBool(UserMessage* userMessage, const plg::string& fieldName, int index, bool value) {
	return userMessage->SetRepeatedBool(fieldName.c_str(), index, value);
}

/**
 * @brief Adds a bool value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddBool(UserMessage* userMessage, const plg::string& fieldName, bool value) {
	return userMessage->AddBool(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated float value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedFloat(UserMessage* userMessage, const plg::string& fieldName, int index, float* out) {
	return userMessage->GetRepeatedFloat(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated float value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetRepeatedFloat(UserMessage* userMessage, const plg::string& fieldName, int index, float value) {
	return userMessage->SetRepeatedFloat(fieldName.c_str(), index, value);
}

/**
 * @brief Adds a float value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddFloat(UserMessage* userMessage, const plg::string& fieldName, float value) {
	return userMessage->AddFloat(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated double value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedDouble(UserMessage* userMessage, const plg::string& fieldName, int index, double* out) {
	return userMessage->GetRepeatedDouble(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated double value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetRepeatedDouble(UserMessage* userMessage, const plg::string& fieldName, int index, double value) {
	return userMessage->SetRepeatedDouble(fieldName.c_str(), index, value);
}

/**
 * @brief Adds a double value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddDouble(UserMessage* userMessage, const plg::string& fieldName, double value) {
	return userMessage->AddDouble(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated string value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output string.
 * @param outSize The size of the output buffer.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedString(UserMessage* userMessage, const plg::string& fieldName, int index, plg::string& out, size_t outSize) {
	std::string result;
	bool success = userMessage->GetRepeatedString(fieldName.c_str(), index, result);
	if (success) {
		out = result;
	}
	return success;
}

/**
 * @brief Sets a repeated string value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_SetRepeatedString(UserMessage* userMessage, const plg::string& fieldName, int index, const plg::string& value) {
	return userMessage->SetRepeatedString(fieldName.c_str(), index, std::string(value));
}

/**
 * @brief Adds a string value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddString(UserMessage* userMessage, const plg::string& fieldName, const plg::string& value) {
	return userMessage->AddString(fieldName.c_str(), value.c_str());
}

/**
 * @brief Gets a nested message from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param message A pointer to store the retrieved message.
 * @return True if the message was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetMessage(UserMessage* userMessage, const plg::string& fieldName, void** message) {
	return userMessage->GetMessage(fieldName.c_str(), reinterpret_cast<pb::Message**>(message));
}

/**
 * @brief Gets a repeated nested message from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param message A pointer to store the retrieved message.
 * @return True if the message was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_GetRepeatedMessage(UserMessage* userMessage, const plg::string& fieldName, int index, const void** message) {
	return userMessage->GetRepeatedMessage(fieldName.c_str(), index, reinterpret_cast<const pb::Message**>(message));
}

/**
 * @brief Adds a nested message to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param message A pointer to the message to add.
 * @return True if the message was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_AddMessage(UserMessage* userMessage, const plg::string& fieldName, void** message) {
	return userMessage->AddMessage(fieldName.c_str(), reinterpret_cast<pb::Message**>(message));
}

/**
 * @brief Gets the count of repeated fields in a field of the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @return The count of repeated fields, or -1 if the field is not repeated or does not exist.
 */
extern "C" PLUGIN_API int UserMessage_GetRepeatedFieldCount(UserMessage* userMessage, const plg::string& fieldName) {
	return userMessage->GetRepeatedFieldCount(fieldName.c_str());
}

/**
 * @brief Removes a value from a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the value to remove.
 * @return True if the value was successfully removed, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessage_RemoveRepeatedFieldValue(UserMessage* userMessage, const plg::string& fieldName, int index) {
	return userMessage->RemoveRepeatedFieldValue(fieldName.c_str(), index);
}

/**
 * @brief Gets the debug string representation of the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @return The debug string as a plg::string.
 */
extern "C" PLUGIN_API plg::string UserMessage_GetDebugString(UserMessage* userMessage) {
	return userMessage->GetDebugString();
}

PLUGIFY_WARN_POP()