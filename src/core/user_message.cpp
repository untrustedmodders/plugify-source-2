#include "user_message.hpp"

int UserMessage::GetMessageID() { return m_msgSerializable->GetNetMessageInfo()->m_MessageId; }

std::string UserMessage::GetMessageName() { return m_msgSerializable->GetUnscopedName(); }

bool UserMessage::HasField(const std::string& fieldName) {
	const pb::Descriptor* descriptor = m_msg->GetDescriptor();
	const pb::FieldDescriptor* field = descriptor->FindFieldByName(fieldName);

	if (field == nullptr || (field->label() == pb::FieldDescriptor::LABEL_REPEATED)) {
		return false;
	}

	return m_msg->GetReflection()->HasField(*m_msg, field);
}

const CNetMessagePB<pb::Message>* UserMessage::GetProtobufMessage() { return m_msg; }