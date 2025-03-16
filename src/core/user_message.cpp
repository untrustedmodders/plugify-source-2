#include "user_message.hpp"

int UserMessage::GetMessageID() { return msgSerializable->GetNetMessageInfo()->m_MessageId; }

std::string UserMessage::GetMessageName() { return msgSerializable->GetUnscopedName(); }

bool UserMessage::HasField(const std::string& fieldName) {
	const pb::Descriptor* descriptor = msg->GetDescriptor();
	const pb::FieldDescriptor* field = descriptor->FindFieldByName(fieldName);

	if (field == nullptr || (field->label() == pb::FieldDescriptor::LABEL_REPEATED)) {
		return false;
	}

	return msg->GetReflection()->HasField(*msg, field);
}

const CNetMessagePB<pb::Message>* UserMessage::GetProtobufMessage() { return msg; }