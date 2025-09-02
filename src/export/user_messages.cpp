#include <core/user_message.hpp>
#include <core/user_message_manager.hpp>
#include <engine/igameeventsystem.h>
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
extern "C" PLUGIN_API bool HookUserMessage(int16_t messageId, UserMessageCallback callback, HookMode mode) {
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
extern "C" PLUGIN_API bool UnhookUserMessage(int16_t messageId, UserMessageCallback callback, HookMode mode) {
	return g_UserMessageManager.UnhookUserMessage(messageId, callback, mode);
}

/**
 * @brief Creates a UserMessage from a serializable message.
 *
 * @param msgSerializable The serializable message.
 * @param message The network message.
 * @param recipients The recipient mask.
 * @return A pointer to the newly created UserMessage.
 */
extern "C" PLUGIN_API UserMessage* UserMessageCreateFromSerializable(INetworkMessageInternal* msgSerializable, const CNetMessage* message, uint64_t recipients) {
	return new UserMessage(msgSerializable, message, recipients);
}

/**
 * @brief Creates a UserMessage from a message name.
 *
 * @param messageName The name of the message.
 * @return A pointer to the newly created UserMessage.
 */
extern "C" PLUGIN_API UserMessage* UserMessageCreateFromName(const plg::string& messageName) {
	return new UserMessage(messageName.c_str());
}

/**
 * @brief Creates a UserMessage from a message ID.
 *
 * @param messageId The ID of the message.
 * @return A pointer to the newly created UserMessage.
 */
extern "C" PLUGIN_API UserMessage* UserMessageCreateFromId(int16_t messageId) {
	return new UserMessage(messageId);
}

/**
 * @brief Destroys a UserMessage and frees its memory.
 *
 * @param userMessage The UserMessage to destroy.
 */
extern "C" PLUGIN_API void UserMessageDestroy(UserMessage* userMessage) {
	delete userMessage;
}

/**
 * @brief Sends a UserMessage to the specified recipients.
 *
 * @param userMessage The UserMessage to send.
 */
extern "C" PLUGIN_API void UserMessageSend(UserMessage* userMessage) {
	g_pGameEventSystem->PostEventAbstract(-1, false, &userMessage->GetRecipientFilter(), userMessage->GetSerializableMessage(), userMessage->GetNetMessage(), 0);
}

/**
 * @brief Gets the name of the message.
 *
 * @param userMessage The UserMessage instance.
 * @return The name of the message as a string.
 */
extern "C" PLUGIN_API const plg::string UserMessageGetMessageName(UserMessage* userMessage) {
	return userMessage->GetMessageName();
}

/**
 * @brief Gets the ID of the message.
 *
 * @param userMessage The UserMessage instance.
 * @return The ID of the message.
 */
extern "C" PLUGIN_API int16_t UserMessageGetMessageID(UserMessage* userMessage) {
	return userMessage->GetMessageID();
}

/**
 * @brief Checks if the message has a specific field.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field to check.
 * @return True if the field exists, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessageHasField(UserMessage* userMessage, const plg::string& fieldName) {
	return userMessage->HasField(fieldName.c_str());
}

/**
 * @brief Gets the protobuf message associated with the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @return A pointer to the protobuf message.
 */
extern "C" PLUGIN_API const void* UserMessageGetProtobufMessage(UserMessage* userMessage) {
	return userMessage->GetProtobufMessage();
}

/**
 * @brief Gets the serializable message associated with the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @return A pointer to the serializable message.
 */
extern "C" PLUGIN_API void* UserMessageGetSerializableMessage(UserMessage* userMessage) {
	return userMessage->GetSerializableMessage();
}

/**
 * @brief Finds a message ID by its name.
 *
 * @param messageName The name of the message.
 * @return The ID of the message, or 0 if the message was not found.
 */
extern "C" PLUGIN_API int16_t UserMessageFindMessageIdByName(const plg::string& messageName) {
	if (auto message = g_pNetworkMessages->FindNetworkMessagePartial(messageName.c_str())) {
		return message->GetNetMessageInfo()->m_MessageId;
	}
	S2_LOGF(LS_WARNING, "Could not find user message: {}", messageName);
	return 0;
}

/**
 * @brief Gets the recipient mask for the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @return The recipient mask.
 */
extern "C" PLUGIN_API uint64_t UserMessageGetRecipientMask(UserMessage* userMessage) {
	return userMessage->GetRecipientFilter().GetRecipientCount() ? *reinterpret_cast<const uint64_t *>(userMessage->GetRecipientFilter().GetRecipients().Base()) : 0;
}

/**
 * @brief Adds a single recipient (player) to the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param playerSlot The slot index of the player to add as a recipient.
 */
extern "C" PLUGIN_API void UserMessageAddRecipient(UserMessage* userMessage, int32_t playerSlot) {
	userMessage->GetRecipientFilter().AddRecipient(playerSlot);
}

/**
 * @brief Adds all connected players as recipients to the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 */
extern "C" PLUGIN_API void UserMessageAddAllPlayers(UserMessage* userMessage) {
	userMessage->GetRecipientFilter().AddAllPlayers();
}

/**
 * @brief Sets the recipient mask for the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param mask The recipient mask to set.
 */
extern "C" PLUGIN_API void UserMessageSetRecipientMask(UserMessage* userMessage, uint64_t recipients) {
	userMessage->GetRecipientFilter().SetRecipients(recipients);
}

/**
 * @brief Gets a nested message from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param message A pointer to store the retrieved message.
 * @return True if the message was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool UserMessageGetMessage(UserMessage* userMessage, const plg::string& fieldName, void** message) {
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
extern "C" PLUGIN_API bool UserMessageGetRepeatedMessage(UserMessage* userMessage, const plg::string& fieldName, int index, const void** message) {
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
extern "C" PLUGIN_API bool UserMessageAddMessage(UserMessage* userMessage, const plg::string& fieldName, void** message) {
	return userMessage->AddMessage(fieldName.c_str(), reinterpret_cast<pb::Message**>(message));
}

/**
 * @brief Gets the count of repeated fields in a field of the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @return The count of repeated fields, or -1 if the field is not repeated or does not exist.
 */
extern "C" PLUGIN_API int UserMessageGetRepeatedFieldCount(UserMessage* userMessage, const plg::string& fieldName) {
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
extern "C" PLUGIN_API bool UserMessageRemoveRepeatedFieldValue(UserMessage* userMessage, const plg::string& fieldName, int index) {
	return userMessage->RemoveRepeatedFieldValue(fieldName.c_str(), index);
}

/**
 * @brief Gets the debug string representation of the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @return The debug string as a string.
 */
extern "C" PLUGIN_API plg::string UserMessageGetDebugString(UserMessage* userMessage) {
	return userMessage->GetDebugString();
}

/**
 * @brief Reads an enum value from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The integer representation of the enum value, or 0 if invalid.
 */
extern "C" PLUGIN_API int PbReadEnum(UserMessage* userMessage, const plg::string& fieldName, int index) {
	int32_t returnValue;
	if (index < 0) {
		if (!userMessage->GetEnum(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	} else {
		if (!userMessage->GetRepeatedEnum(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	}
	return returnValue;
}

/**
 * @brief Reads a 32-bit integer from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The int32_t value read, or 0 if invalid.
 */
extern "C" PLUGIN_API int32_t PbReadInt32(UserMessage* userMessage, const plg::string& fieldName, int index) {
	int32_t returnValue;
	if (index < 0) {
		if (!userMessage->GetInt32(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	} else {
		if (!userMessage->GetRepeatedInt32(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	}
	return returnValue;
}

/**
 * @brief Reads a 64-bit integer from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The int64_t value read, or 0 if invalid.
 */
extern "C" PLUGIN_API int64_t PbReadInt64(UserMessage* userMessage, const plg::string& fieldName, int index) {
	int64_t returnValue;
	if (index < 0) {
		if (!userMessage->GetInt64(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	} else {
		if (!userMessage->GetRepeatedInt64(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	}
	return returnValue;
}

/**
 * @brief Reads an unsigned 32-bit integer from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The uint32_t value read, or 0 if invalid.
 */
extern "C" PLUGIN_API uint32_t PbReadUInt32(UserMessage* userMessage, const plg::string& fieldName, int index) {
	uint32_t returnValue;
	if (index < 0) {
		if (!userMessage->GetUInt32(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	} else {
		if (!userMessage->GetRepeatedUInt32(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	}
	return returnValue;
}

/**
 * @brief Reads an unsigned 64-bit integer from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The uint64_t value read, or 0 if invalid.
 */
extern "C" PLUGIN_API uint64_t PbReadUInt64(UserMessage* userMessage, const plg::string& fieldName, int index) {
	uint64_t returnValue;
	if (index < 0) {
		if (!userMessage->GetUInt64(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	} else {
		if (!userMessage->GetRepeatedUInt64(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	}
	return returnValue;
}

/**
 * @brief Reads a floating-point value from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The float value read, or 0.0 if invalid.
 */
extern "C" PLUGIN_API float PbReadFloat(UserMessage* userMessage, const plg::string& fieldName, int index) {
	float returnValue;
	if (index < 0) {
		if (!userMessage->GetFloat(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	} else {
		if (!userMessage->GetRepeatedFloat(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	}
	return returnValue;
}

/**
 * @brief Reads a double-precision floating-point value from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The double value read, or 0.0 if invalid.
 */
extern "C" PLUGIN_API double PbReadDouble(UserMessage* userMessage, const plg::string& fieldName, int index) {
	double returnValue;
	if (index < 0) {
		if (!userMessage->GetDouble(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	} else {
		if (!userMessage->GetRepeatedDouble(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return 0;
		}
	}
	return returnValue;
}

/**
 * @brief Reads a boolean value from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The boolean value read, or false if invalid.
 */
extern "C" PLUGIN_API bool PbReadBool(UserMessage* userMessage, const plg::string& fieldName, int index) {
	bool returnValue;
	if (index < 0) {
		if (!userMessage->GetBool(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return false;
		}
	} else {
		if (!userMessage->GetRepeatedBool(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return false;
		}
	}
	return returnValue;
}

/**
 * @brief Reads a string from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The string value read, or an empty string if invalid.
 */
extern "C" PLUGIN_API plg::string PbReadString(UserMessage* userMessage, const plg::string& fieldName, int index) {
	plg::string returnValue;
	if (index < 0) {
		if (!userMessage->GetString(fieldName.c_str(), returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	} else {
		if (!userMessage->GetRepeatedString(fieldName.c_str(), index, returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	}
	return returnValue;
}

/**
 * @brief Reads a color value from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The color value read, or an empty value if invalid.
 */
extern "C" PLUGIN_API int PbReadColor(UserMessage* userMessage, const plg::string& fieldName, int index) {
	Color returnValue;
	if (index < 0) {
		if (!userMessage->GetColor(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	} else {
		if (!userMessage->GetRepeatedColor(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	}
	return returnValue.GetRawColor();
}

/**
 * @brief Reads a 2D vector from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The 2D vector value read, or an empty value if invalid.
 */
extern "C" PLUGIN_API plg::vec2 PbReadVector2(UserMessage* userMessage, const plg::string& fieldName, int index) {
	Vector2D returnValue;
	if (index < 0) {
		if (!userMessage->GetVector2D(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	} else {
		if (!userMessage->GetRepeatedVector2D(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	}
	return *reinterpret_cast<plg::vec2*>(&returnValue);
}

/**
 * @brief Reads a 3D vector from a UserMessage.
 *
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The 3D vector value read, or an empty value if invalid.
 */
extern "C" PLUGIN_API plg::vec3 PbReadVector3(UserMessage* userMessage, const plg::string& fieldName, int index) {
	Vector returnValue;
	if (index < 0) {
		if (!userMessage->GetVector(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	} else {
		if (!userMessage->GetRepeatedVector(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	}
	return *reinterpret_cast<plg::vec3*>(&returnValue);
}

/**
 * @brief Reads a QAngle (rotation vector) from a UserMessage.
 * 
 * @param userMessage Pointer to the UserMessage object.
 * @param fieldName Name of the field to read.
 * @param index Index of the repeated field (use -1 for non-repeated fields).
 * @return The QAngle value read, or an empty value if invalid.
 */
extern "C" PLUGIN_API plg::vec3 PbReadQAngle(UserMessage* userMessage, const plg::string& fieldName, int index) {
	QAngle returnValue;
	if (index < 0) {
		if (!userMessage->GetQAngle(fieldName.c_str(), &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\" for message \"{}\"", fieldName, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	} else {
		if (!userMessage->GetRepeatedQAngle(fieldName.c_str(), index, &returnValue)) {
			S2_LOGF(LS_WARNING, "Invalid field \"{}\"[{}] for message \"{}\"", fieldName, index, userMessage->GetProtobufMessage()->GetTypeName());
			return {};
		}
	}
	return *reinterpret_cast<plg::vec3*>(&returnValue);
}

/**
 * @brief Gets a enum value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetEnum(UserMessage* userMessage, const plg::string& fieldName, int* out) {
	return userMessage->GetEnum(fieldName.c_str(), out);
}

/**
 * @brief Sets a enum value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetEnum(UserMessage* userMessage, const plg::string& fieldName, int value) {
	return userMessage->SetEnum(fieldName.c_str(), value);
}

/**
 * @brief Gets a 32-bit integer value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetInt32(UserMessage* userMessage, const plg::string& fieldName, int32_t* out) {
	return userMessage->GetInt32(fieldName.c_str(), out);
}

/**
 * @brief Sets a 32-bit integer value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetInt32(UserMessage* userMessage, const plg::string& fieldName, int32_t value) {
	return userMessage->SetInt32(fieldName.c_str(), value);
}

/**
 * @brief Gets a 64-bit integer value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetInt64(UserMessage* userMessage, const plg::string& fieldName, int64_t* out) {
	return userMessage->GetInt64(fieldName.c_str(), out);
}

/**
 * @brief Sets a 64-bit integer value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetInt64(UserMessage* userMessage, const plg::string& fieldName, int64_t value) {
	return userMessage->SetInt64(fieldName.c_str(), value);
}

/**
 * @brief Gets an unsigned 32-bit integer value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetUInt32(UserMessage* userMessage, const plg::string& fieldName, uint32_t* out) {
	return userMessage->GetUInt32(fieldName.c_str(), out);
}

/**
 * @brief Sets an unsigned 32-bit integer value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetUInt32(UserMessage* userMessage, const plg::string& fieldName, uint32_t value) {
	return userMessage->SetUInt32(fieldName.c_str(), value);
}

/**
 * @brief Gets an unsigned 64-bit integer value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetUInt64(UserMessage* userMessage, const plg::string& fieldName, uint64_t* out) {
	return userMessage->GetUInt64(fieldName.c_str(), out);
}

/**
 * @brief Sets an unsigned 64-bit integer value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetUInt64(UserMessage* userMessage, const plg::string& fieldName, uint64_t value) {
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
extern "C" PLUGIN_API bool PbGetBool(UserMessage* userMessage, const plg::string& fieldName, bool* out) {
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
extern "C" PLUGIN_API bool PbSetBool(UserMessage* userMessage, const plg::string& fieldName, bool value) {
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
extern "C" PLUGIN_API bool PbGetFloat(UserMessage* userMessage, const plg::string& fieldName, float* out) {
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
extern "C" PLUGIN_API bool PbSetFloat(UserMessage* userMessage, const plg::string& fieldName, float value) {
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
extern "C" PLUGIN_API bool PbGetDouble(UserMessage* userMessage, const plg::string& fieldName, double* out) {
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
extern "C" PLUGIN_API bool PbSetDouble(UserMessage* userMessage, const plg::string& fieldName, double value) {
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
extern "C" PLUGIN_API bool PbGetString(UserMessage* userMessage, const plg::string& fieldName, plg::string& out) {
	return userMessage->GetString(fieldName.c_str(), out);
}

/**
 * @brief Sets a string value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetString(UserMessage* userMessage, const plg::string& fieldName, const plg::string& value) {
	return userMessage->SetString(fieldName.c_str(), std::string(value));
}

/**
 * @brief Gets a color value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output string.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetColor(UserMessage* userMessage, const plg::string& fieldName, int* out) {
	return userMessage->GetColor(fieldName.c_str(), reinterpret_cast<Color*>(out));
}

/**
 * @brief Sets a color value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetColor(UserMessage* userMessage, const plg::string& fieldName, int value) {
	return userMessage->SetColor(fieldName.c_str(), *reinterpret_cast<Color*>(&value));
}

/**
 * @brief Gets a Vector2 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output string.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetVector2(UserMessage* userMessage, const plg::string& fieldName, plg::vec2* out) {
	return userMessage->GetVector2D(fieldName.c_str(), reinterpret_cast<Vector2D*>(out));
}

/**
 * @brief Sets a Vector2 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetVector2(UserMessage* userMessage, const plg::string& fieldName, const plg::vec2& value) {
	return userMessage->SetVector2D(fieldName.c_str(), *reinterpret_cast<const Vector2D*>(&value));
}

/**
 * @brief Gets a Vector3 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output string.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetVector3(UserMessage* userMessage, const plg::string& fieldName, plg::vec3* out) {
	return userMessage->GetVector(fieldName.c_str(), reinterpret_cast<Vector*>(out));
}

/**
 * @brief Sets a Vector3 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetVector3(UserMessage* userMessage, const plg::string& fieldName, const plg::vec3& value) {
	return userMessage->SetVector(fieldName.c_str(), *reinterpret_cast<const Vector*>(&value));
}

/**
 * @brief Gets a QAngle value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param out The output string.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetQAngle(UserMessage* userMessage, const plg::string& fieldName, plg::vec3* out) {
	return userMessage->GetQAngle(fieldName.c_str(), reinterpret_cast<QAngle*>(out));
}

/**
 * @brief Sets a QAngle value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetQAngle(UserMessage* userMessage, const plg::string& fieldName, const plg::vec3& value) {
	return userMessage->SetQAngle(fieldName.c_str(), *reinterpret_cast<const QAngle*>(&value));
}

/**
 * @brief Gets a repeated enum value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedEnum(UserMessage* userMessage, const plg::string& fieldName, int index, int* out) {
	return userMessage->GetRepeatedEnum(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated enum value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedEnum(UserMessage* userMessage, const plg::string& fieldName, int index, int value) {
	return userMessage->SetRepeatedEnum(fieldName.c_str(), index, value);
}

/**
 * @brief Adds a enum value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddEnum(UserMessage* userMessage, const plg::string& fieldName, int value) {
	return userMessage->AddEnum(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated int32_t value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedInt32(UserMessage* userMessage, const plg::string& fieldName, int index, int32_t* out) {
	return userMessage->GetRepeatedInt32(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated int32_t value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedInt32(UserMessage* userMessage, const plg::string& fieldName, int index, int32_t value) {
	return userMessage->SetRepeatedInt32(fieldName.c_str(), index, value);
}

/**
 * @brief Adds a 32-bit integer value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddInt32(UserMessage* userMessage, const plg::string& fieldName, int32_t value) {
	return userMessage->AddInt32(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated int64_t value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedInt64(UserMessage* userMessage, const plg::string& fieldName, int index, int64_t* out) {
	return userMessage->GetRepeatedInt64(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated int64_t value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedInt64(UserMessage* userMessage, const plg::string& fieldName, int index, int64_t value) {
	return userMessage->SetRepeatedInt64(fieldName.c_str(), index, value);
}

/**
 * @brief Adds a 64-bit integer value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddInt64(UserMessage* userMessage, const plg::string& fieldName, int64_t value) {
	return userMessage->AddInt64(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated uint32_t value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedUInt32(UserMessage* userMessage, const plg::string& fieldName, int index, uint32_t* out) {
	return userMessage->GetRepeatedUInt32(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated uint32_t value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedUInt32(UserMessage* userMessage, const plg::string& fieldName, int index, uint32_t value) {
	return userMessage->SetRepeatedUInt32(fieldName.c_str(), index, value);
}

/**
 * @brief Adds an unsigned 32-bit integer value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddUInt32(UserMessage* userMessage, const plg::string& fieldName, uint32_t value) {
	return userMessage->AddUInt32(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated uint64_t value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output value.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedUInt64(UserMessage* userMessage, const plg::string& fieldName, int index, uint64_t* out) {
	return userMessage->GetRepeatedUInt64(fieldName.c_str(), index, out);
}

/**
 * @brief Sets a repeated uint64_t value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedUInt64(UserMessage* userMessage, const plg::string& fieldName, int index, uint64_t value) {
	return userMessage->SetRepeatedUInt64(fieldName.c_str(), index, value);
}

/**
 * @brief Adds an unsigned 64-bit integer value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddUInt64(UserMessage* userMessage, const plg::string& fieldName, uint64_t value) {
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
extern "C" PLUGIN_API bool PbGetRepeatedBool(UserMessage* userMessage, const plg::string& fieldName, int index, bool* out) {
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
extern "C" PLUGIN_API bool PbSetRepeatedBool(UserMessage* userMessage, const plg::string& fieldName, int index, bool value) {
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
extern "C" PLUGIN_API bool PbAddBool(UserMessage* userMessage, const plg::string& fieldName, bool value) {
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
extern "C" PLUGIN_API bool PbGetRepeatedFloat(UserMessage* userMessage, const plg::string& fieldName, int index, float* out) {
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
extern "C" PLUGIN_API bool PbSetRepeatedFloat(UserMessage* userMessage, const plg::string& fieldName, int index, float value) {
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
extern "C" PLUGIN_API bool PbAddFloat(UserMessage* userMessage, const plg::string& fieldName, float value) {
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
extern "C" PLUGIN_API bool PbGetRepeatedDouble(UserMessage* userMessage, const plg::string& fieldName, int index, double* out) {
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
extern "C" PLUGIN_API bool PbSetRepeatedDouble(UserMessage* userMessage, const plg::string& fieldName, int index, double value) {
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
extern "C" PLUGIN_API bool PbAddDouble(UserMessage* userMessage, const plg::string& fieldName, double value) {
	return userMessage->AddDouble(fieldName.c_str(), value);
}

/**
 * @brief Gets a repeated string value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output string.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedString(UserMessage* userMessage, const plg::string& fieldName, int index, plg::string& out) {
	return userMessage->GetRepeatedString(fieldName.c_str(), index, out);
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
extern "C" PLUGIN_API bool PbSetRepeatedString(UserMessage* userMessage, const plg::string& fieldName, int index, const plg::string& value) {
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
extern "C" PLUGIN_API bool PbAddString(UserMessage* userMessage, const plg::string& fieldName, const plg::string& value) {
	return userMessage->AddString(fieldName.c_str(), value.c_str());
}

/**
 * @brief Gets a repeated color value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output color.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedColor(UserMessage* userMessage, const plg::string& fieldName, int index, int* out) {
	return userMessage->GetRepeatedColor(fieldName.c_str(), index, reinterpret_cast<Color*>(out));
}

/**
 * @brief Sets a repeated color value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedColor(UserMessage* userMessage, const plg::string& fieldName, int index, int value) {
	return userMessage->SetRepeatedColor(fieldName.c_str(), index, *reinterpret_cast<Color*>(&value));
}

/**
 * @brief Adds a color value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddColor(UserMessage* userMessage, const plg::string& fieldName, int value) {
	return userMessage->AddColor(fieldName.c_str(), *reinterpret_cast<Color*>(&value));
}

/**
 * @brief Gets a repeated Vector2 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output vector2.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedVector2(UserMessage* userMessage, const plg::string& fieldName, int index, plg::vec2* out) {
	return userMessage->GetRepeatedVector2D(fieldName.c_str(), index, reinterpret_cast<Vector2D*>(out));
}

/**
 * @brief Sets a repeated Vector2 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedVector2(UserMessage* userMessage, const plg::string& fieldName, int index, const plg::vec2& value) {
	return userMessage->SetRepeatedVector2D(fieldName.c_str(), index, *reinterpret_cast<const Vector2D*>(&value));
}

/**
 * @brief Adds a Vector2 value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddVector2(UserMessage* userMessage, const plg::string& fieldName, const plg::vec2& value) {
	return userMessage->AddVector2D(fieldName.c_str(), *reinterpret_cast<const Vector2D*>(&value));
}

/**
 * @brief Gets a repeated Vector3 value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output vector2.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedVector3(UserMessage* userMessage, const plg::string& fieldName, int index, plg::vec3* out) {
	return userMessage->GetRepeatedVector(fieldName.c_str(), index, reinterpret_cast<Vector*>(out));
}

/**
 * @brief Sets a repeated Vector3 value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedVector3(UserMessage* userMessage, const plg::string& fieldName, int index, const plg::vec3& value) {
	return userMessage->SetRepeatedVector(fieldName.c_str(), index, *reinterpret_cast<const Vector*>(&value));
}

/**
 * @brief Adds a Vector3 value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddVector3(UserMessage* userMessage, const plg::string& fieldName, const plg::vec3& value) {
	return userMessage->AddVector(fieldName.c_str(), *reinterpret_cast<const Vector*>(&value));
}

/**
 * @brief Gets a repeated QAngle value from a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param out The output vector2.
 * @return True if the field was successfully retrieved, false otherwise.
 */
extern "C" PLUGIN_API bool PbGetRepeatedQAngle(UserMessage* userMessage, const plg::string& fieldName, int index, plg::vec3* out) {
	return userMessage->GetRepeatedQAngle(fieldName.c_str(), index, reinterpret_cast<QAngle*>(out));
}

/**
 * @brief Sets a repeated QAngle value for a field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param index The index of the repeated field.
 * @param value The value to set.
 * @return True if the field was successfully set, false otherwise.
 */
extern "C" PLUGIN_API bool PbSetRepeatedQAngle(UserMessage* userMessage, const plg::string& fieldName, int index, const plg::vec3& value) {
	return userMessage->SetRepeatedQAngle(fieldName.c_str(), index, *reinterpret_cast<const QAngle*>(&value));
}

/**
 * @brief Adds a QAngle value to a repeated field in the UserMessage.
 *
 * @param userMessage The UserMessage instance.
 * @param fieldName The name of the field.
 * @param value The value to add.
 * @return True if the value was successfully added, false otherwise.
 */
extern "C" PLUGIN_API bool PbAddQAngle(UserMessage* userMessage, const plg::string& fieldName, const plg::vec3& value) {
	return userMessage->AddQAngle(fieldName.c_str(), *reinterpret_cast<const QAngle*>(&value));
}

PLUGIFY_WARN_POP()