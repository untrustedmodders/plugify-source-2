#pragma once

#include <networkbasetypes.pb.h>
#include <networksystem/inetworkmessages.h>

namespace pb = google::protobuf;

#define GETCHECK_FIELD()                                                                          \
	const pb::FieldDescriptor* field = m_msg->GetDescriptor()->FindFieldByName(pszFieldName); \
	if (!field) {                                                                                 \
		return false;                                                                             \
	}

#define CHECK_FIELD_TYPE(type)                                            \
	if (field->cpp_type() != pb::FieldDescriptor::CPPTYPE_##type) { \
		return false;                                                     \
	}

#define CHECK_FIELD_TYPE2(type1, type2)                                                                                       \
	pb::FieldDescriptor::CppType fieldType = field->cpp_type();                                                         \
	if (fieldType != pb::FieldDescriptor::CPPTYPE_##type1 && fieldType != pb::FieldDescriptor::CPPTYPE_##type2) { \
		return false;                                                                                                         \
	}

#define CHECK_FIELD_TYPE3(type1, type2, type3)                                                                                \
	pb::FieldDescriptor::CppType fieldType = field->cpp_type();                                                         \
	if (fieldType != pb::FieldDescriptor::CPPTYPE_##type1 && fieldType != pb::FieldDescriptor::CPPTYPE_##type2 && \
		fieldType != pb::FieldDescriptor::CPPTYPE_##type3) {                                                            \
		return false;                                                                                                         \
	}

#define CHECK_FIELD_REPEATED()                                         \
	if (field->label() != pb::FieldDescriptor::LABEL_REPEATED) { \
		return false;                                                  \
	}

#define CHECK_FIELD_NOT_REPEATED()                                     \
	if (field->label() == pb::FieldDescriptor::LABEL_REPEATED) { \
		return false;                                                  \
	}

#define CHECK_REPEATED_ELEMENT(idx)                               \
	int elemCount = m_msg->GetReflection()->FieldSize(*m_msg, field); \
	if (elemCount == 0 || idx >= elemCount || idx < 0) {          \
		return false;                                             \
	};

class INetworkMessageInternal;
namespace google::protobuf {
	class Message;
}

class UserMessage {
public:
	UserMessage(INetworkMessageInternal* msgSerializable, const CNetMessage* message, int recipientCount, uint64_t* recipientMask)
		: m_msgSerializable(msgSerializable), m_msg(const_cast<CNetMessage*>(message)->ToPB<pb::Message>()), m_recipientCount(recipientCount),
		  m_recipientMask(recipientMask) {
	}

	explicit UserMessage(const char* messageName) {
		m_manuallyAllocated = true;
		m_msgSerializable = g_pNetworkMessages->FindNetworkMessagePartial(messageName);
		if (!m_msgSerializable) return;

		m_msg = m_msgSerializable->AllocateMessage()->ToPB<pb::Message>();
		m_recipientMask = new uint64_t(0);
	}

	explicit UserMessage(uint16_t messageId) {
		m_manuallyAllocated = true;
		m_msgSerializable = g_pNetworkMessages->FindNetworkMessageById(static_cast<int32_t>(messageId));
		if (!m_msgSerializable) return;

		m_msg = m_msgSerializable->AllocateMessage()->ToPB<pb::Message>();
		m_recipientMask = new uint64_t(0);
	}

	~UserMessage() {
		if (m_manuallyAllocated) delete m_recipientMask;
	}

	std::string GetMessageName();
	uint16_t GetMessageID();
	bool HasField(const std::string& fieldName);
	const CNetMessagePB<pb::Message>* GetProtobufMessage();
	INetworkMessageInternal* GetSerializableMessage() { return m_msgSerializable; }
	uint64_t* GetRecipientMask() { return m_recipientMask; }
	bool IsManuallyAllocated() const { return m_manuallyAllocated; }

private:
	INetworkMessageInternal* m_msgSerializable{};
	CNetMessagePB<pb::Message>* m_msg{};
	int m_recipientCount{};
	bool m_manuallyAllocated{};
	uint64_t* m_recipientMask{};

public:
	bool HasField(const char* pszFieldName) {
		GETCHECK_FIELD();
		CHECK_FIELD_NOT_REPEATED();
		return m_msg->GetReflection()->HasField(*m_msg, field);
	}

	bool GetEnum(const char* pszFieldName, int* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(ENUM);
		CHECK_FIELD_NOT_REPEATED();

		*out = m_msg->GetReflection()->GetEnum(*m_msg, field)->number();
		return true;
	}

	bool SetEnum(const char* pszFieldName, int value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(ENUM);
		CHECK_FIELD_NOT_REPEATED();

		const pb::EnumValueDescriptor* pEnumValue = field->enum_type()->FindValueByNumber(value);
		if (!pEnumValue) return false;
		m_msg->GetReflection()->SetEnum(m_msg, field, pEnumValue);
		return true;
	}

	bool GetRepeatedEnum(const char* pszFieldName, int index, int* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(ENUM);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = m_msg->GetReflection()->GetRepeatedEnum(*m_msg, field, index)->number();
		return true;
	}

	bool SetRepeatedEnum(const char* pszFieldName, int index, int value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(ENUM);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		const pb::EnumValueDescriptor* pEnumValue = field->enum_type()->FindValueByNumber(value);
		if (!pEnumValue) return false;
		m_msg->GetReflection()->SetRepeatedEnum(m_msg, field, index, pEnumValue);
		return true;
	}

	bool AddEnum(const char* pszFieldName, int value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(ENUM);
		CHECK_FIELD_REPEATED();

		const pb::EnumValueDescriptor* pEnumValue = field->enum_type()->FindValueByNumber(value);
		if (!pEnumValue) return false;
		m_msg->GetReflection()->AddEnum(m_msg, field, pEnumValue);
		return true;
	}

	bool GetInt32(const char* pszFieldName, int32_t* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_NOT_REPEATED();

		*out = m_msg->GetReflection()->GetInt32(*m_msg, field);
		return true;
	}

	bool SetInt32(const char* pszFieldName, int32_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_NOT_REPEATED();

		m_msg->GetReflection()->SetInt32(m_msg, field, value);
		return true;
	}

	bool GetRepeatedInt32(const char* pszFieldName, int index, int32_t* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = m_msg->GetReflection()->GetRepeatedInt32(*m_msg, field, index);
		return true;
	}

	bool SetRepeatedInt32(const char* pszFieldName, int index, int32_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		m_msg->GetReflection()->SetRepeatedInt32(m_msg, field, index, value);
		return true;
	}

	bool AddInt32(const char* pszFieldName, int32_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_REPEATED();

		m_msg->GetReflection()->AddInt32(m_msg, field, value);
		return true;
	}

	bool GetInt64(const char* pszFieldName, int64_t* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_NOT_REPEATED();

		*out = m_msg->GetReflection()->GetInt64(*m_msg, field);
		return true;
	}

	bool SetInt64(const char* pszFieldName, int64_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_NOT_REPEATED();

		m_msg->GetReflection()->SetInt64(m_msg, field, value);
		return true;
	}

	bool GetRepeatedInt64(const char* pszFieldName, int index, int64_t* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = m_msg->GetReflection()->GetRepeatedInt64(*m_msg, field, index);
		return true;
	}

	bool SetRepeatedInt64(const char* pszFieldName, int index, int64_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		m_msg->GetReflection()->SetRepeatedInt64(m_msg, field, index, value);
		return true;
	}

	bool AddInt64(const char* pszFieldName, int64_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_REPEATED();

		m_msg->GetReflection()->AddInt64(m_msg, field, value);
		return true;
	}

	bool GetUInt32(const char* pszFieldName, uint32_t* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_NOT_REPEATED();

		*out = m_msg->GetReflection()->GetUInt32(*m_msg, field);
		return true;
	}

	bool SetUInt32(const char* pszFieldName, uint32_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_NOT_REPEATED();

		m_msg->GetReflection()->SetUInt32(m_msg, field, value);
		return true;
	}

	bool GetRepeatedUInt32(const char* pszFieldName, int index, uint32_t* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = m_msg->GetReflection()->GetRepeatedUInt32(*m_msg, field, index);
		return true;
	}

	bool SetRepeatedUInt32(const char* pszFieldName, int index, uint32_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		m_msg->GetReflection()->SetRepeatedUInt32(m_msg, field, index, value);
		return true;
	}

	bool AddUInt32(const char* pszFieldName, uint32_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_REPEATED();

		m_msg->GetReflection()->AddUInt32(m_msg, field, value);
		return true;
	}

	bool GetUInt64(const char* pszFieldName, uint64_t* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_NOT_REPEATED();

		*out = m_msg->GetReflection()->GetUInt64(*m_msg, field);
		return true;
	}

	bool SetUInt64(const char* pszFieldName, uint64_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_NOT_REPEATED();

		m_msg->GetReflection()->SetUInt64(m_msg, field, value);
		return true;
	}

	bool GetRepeatedUInt64(const char* pszFieldName, int index, uint64_t* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = m_msg->GetReflection()->GetRepeatedUInt64(*m_msg, field, index);
		return true;
	}

	bool SetRepeatedUInt64(const char* pszFieldName, int index, uint64_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		m_msg->GetReflection()->SetRepeatedUInt64(m_msg, field, index, value);
		return true;
	}

	bool AddUInt64(const char* pszFieldName, uint64_t value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_REPEATED();

		m_msg->GetReflection()->AddUInt32(m_msg, field, value);
		return true;
	}

	bool GetBool(const char* pszFieldName, bool* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_NOT_REPEATED();

		*out = m_msg->GetReflection()->GetBool(*m_msg, field);
		return true;
	}

	bool SetBool(const char* pszFieldName, bool value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_NOT_REPEATED();

		m_msg->GetReflection()->SetBool(m_msg, field, value);
		return true;
	}

	bool GetRepeatedBool(const char* pszFieldName, int index, bool* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = m_msg->GetReflection()->GetRepeatedBool(*m_msg, field, index);
		return true;
	}

	bool SetRepeatedBool(const char* pszFieldName, int index, bool value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		m_msg->GetReflection()->SetRepeatedBool(m_msg, field, index, value);
		return true;
	}

	bool AddBool(const char* pszFieldName, bool value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_REPEATED();

		m_msg->GetReflection()->AddBool(m_msg, field, value);
		return true;
	}

	bool GetFloat(const char* pszFieldName, float* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_NOT_REPEATED();

		*out = m_msg->GetReflection()->GetFloat(*m_msg, field);
		return true;
	}

	bool SetFloat(const char* pszFieldName, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_NOT_REPEATED();

		m_msg->GetReflection()->SetFloat(m_msg, field, value);
		return true;
	}

	bool GetRepeatedFloat(const char* pszFieldName, int index, float* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = m_msg->GetReflection()->GetRepeatedFloat(*m_msg, field, index);
		return true;
	}

	bool SetRepeatedFloat(const char* pszFieldName, int index, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		m_msg->GetReflection()->SetRepeatedFloat(m_msg, field, index, value);
		return true;
	}

	bool AddFloat(const char* pszFieldName, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_REPEATED();

		m_msg->GetReflection()->AddFloat(m_msg, field, value);
		return true;
	}

	bool GetDouble(const char* pszFieldName, double* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_NOT_REPEATED();

		*out = m_msg->GetReflection()->GetDouble(*m_msg, field);
		return true;
	}

	bool SetDouble(const char* pszFieldName, double value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_NOT_REPEATED();

		m_msg->GetReflection()->SetDouble(m_msg, field, value);
		return true;
	}

	bool GetRepeatedDouble(const char* pszFieldName, int index, double* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = m_msg->GetReflection()->GetRepeatedDouble(*m_msg, field, index);
		return true;
	}

	bool SetRepeatedDouble(const char* pszFieldName, int index, double value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		m_msg->GetReflection()->SetRepeatedDouble(m_msg, field, index, value);
		return true;
	}

	bool AddDouble(const char* pszFieldName, double value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_REPEATED();

		m_msg->GetReflection()->AddDouble(m_msg, field, value);
		return true;
	}

	bool GetString(const char* pszFieldName, plg::string& out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_NOT_REPEATED();

		out = m_msg->GetReflection()->GetString(*m_msg, field);

		return true;
	}

	bool SetString(const char* pszFieldName, std::string value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_NOT_REPEATED();

		m_msg->GetReflection()->SetString(m_msg, field, std::move(value));
		return true;
	}

	bool GetRepeatedString(const char* pszFieldName, int index, plg::string& out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		out = m_msg->GetReflection()->GetRepeatedString(*m_msg, field, index);

		return true;
	}

	bool SetRepeatedString(const char* pszFieldName, int index, std::string value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		m_msg->GetReflection()->SetRepeatedString(m_msg, field, index, std::move(value));

		return true;
	}

	bool AddString(const char* pszFieldName, const char* value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_REPEATED();

		m_msg->GetReflection()->AddString(m_msg, field, value);
		return true;
	}

	bool GetColor(const char* pszFieldName, Color* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		auto msgRGBA = *(CMsgRGBA*) m_msg->GetReflection()->MutableMessage(m_msg, field);
		out->SetColor(msgRGBA.r(), msgRGBA.g(), msgRGBA.b(), msgRGBA.a());

		return true;
	}

	bool SetColor(const char* pszFieldName, const Color& value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		CMsgRGBA* msgRGBA = (CMsgRGBA*) m_msg->GetReflection()->MutableMessage(m_msg, field);
		msgRGBA->set_r(value.r());
		msgRGBA->set_g(value.g());
		msgRGBA->set_b(value.b());
		msgRGBA->set_a(value.a());

		return true;
	}

	bool GetRepeatedColor(const char* pszFieldName, int index, Color* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		const CMsgRGBA& msgRGBA = (const CMsgRGBA&) m_msg->GetReflection()->GetRepeatedMessage(*m_msg, field, index);
		out->SetColor(msgRGBA.r(), msgRGBA.g(), msgRGBA.b(), msgRGBA.a());

		return true;
	}

	bool SetRepeatedColor(const char* pszFieldName, int index, const Color& value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		CMsgRGBA* msgRGBA = (CMsgRGBA*) m_msg->GetReflection()->MutableRepeatedMessage(m_msg, field, index);
		msgRGBA->set_r(value.r());
		msgRGBA->set_g(value.g());
		msgRGBA->set_b(value.b());
		msgRGBA->set_a(value.a());

		return true;
	}

	bool AddColor(const char* pszFieldName, const Color& value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		CMsgRGBA* msgRGBA = (CMsgRGBA*) m_msg->GetReflection()->AddMessage(m_msg, field);
		msgRGBA->set_r(value.r());
		msgRGBA->set_g(value.g());
		msgRGBA->set_b(value.b());
		msgRGBA->set_a(value.a());

		return true;
	}

	bool GetVector2D(const char* pszFieldName, Vector2D* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		auto msgVec2d = *(CMsgVector2D*) m_msg->GetReflection()->MutableMessage(m_msg, field);
		out->Init(msgVec2d.x(), msgVec2d.y());

		return true;
	}

	bool SetVector2D(const char* pszFieldName, const Vector2D& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		CMsgVector2D* msgVec2d = (CMsgVector2D*) m_msg->GetReflection()->MutableMessage(m_msg, field);
		msgVec2d->set_x(vec.x);
		msgVec2d->set_y(vec.y);

		return true;
	}

	bool GetRepeatedVector2D(const char* pszFieldName, int index, Vector2D* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		const CMsgVector2D& msgVec2d = (const CMsgVector2D&) m_msg->GetReflection()->GetRepeatedMessage(*m_msg, field, index);
		out->Init(msgVec2d.x(), msgVec2d.y());

		return true;
	}

	bool SetRepeatedVector2D(const char* pszFieldName, int index, const Vector2D& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		CMsgVector2D* msgVec2d = (CMsgVector2D*) m_msg->GetReflection()->MutableRepeatedMessage(m_msg, field, index);
		msgVec2d->set_x(vec.x);
		msgVec2d->set_y(vec.y);

		return true;
	}

	bool AddVector2D(const char* pszFieldName, const Vector2D& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		CMsgVector2D* msgVec2d = (CMsgVector2D*) m_msg->GetReflection()->AddMessage(m_msg, field);
		msgVec2d->set_x(vec.x);
		msgVec2d->set_y(vec.y);

		return true;
	}

	bool GetVector(const char* pszFieldName, Vector* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		auto msgVec = *(CMsgVector*) m_msg->GetReflection()->MutableMessage(m_msg, field);
		out->Init(msgVec.x(), msgVec.y(), msgVec.z());

		return true;
	}

	bool SetVector(const char* pszFieldName, const Vector& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		CMsgVector* msgVec = (CMsgVector*) m_msg->GetReflection()->MutableMessage(m_msg, field);
		msgVec->set_x(vec.x);
		msgVec->set_y(vec.y);
		msgVec->set_z(vec.z);

		return true;
	}

	bool GetRepeatedVector(const char* pszFieldName, int index, Vector* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		const CMsgVector& msgVec = (const CMsgVector&) m_msg->GetReflection()->GetRepeatedMessage(*m_msg, field, index);
		out->Init(msgVec.x(), msgVec.y(), msgVec.z());

		return true;
	}

	bool SetRepeatedVector(const char* pszFieldName, int index, const Vector& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		CMsgVector* msgVec = (CMsgVector*) m_msg->GetReflection()->MutableRepeatedMessage(m_msg, field, index);
		msgVec->set_x(vec.x);
		msgVec->set_y(vec.y);
		msgVec->set_z(vec.z);

		return true;
	}

	bool AddVector(const char* pszFieldName, const Vector& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		CMsgVector* msgVec = (CMsgVector*) m_msg->GetReflection()->AddMessage(m_msg, field);
		msgVec->set_x(vec.x);
		msgVec->set_y(vec.y);
		msgVec->set_z(vec.z);

		return true;
	}

	bool GetQAngle(const char* pszFieldName, QAngle* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		auto msgAng = *(CMsgQAngle*) m_msg->GetReflection()->MutableMessage(m_msg, field);
		out->Init(msgAng.x(), msgAng.y(), msgAng.z());

		return true;
	}

	bool SetQAngle(const char* pszFieldName, const QAngle& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		CMsgQAngle* msgAng = (CMsgQAngle*) m_msg->GetReflection()->MutableMessage(m_msg, field);
		msgAng->set_x(vec.x);
		msgAng->set_y(vec.y);
		msgAng->set_z(vec.z);

		return true;
	}

	bool GetRepeatedQAngle(const char* pszFieldName, int index, QAngle* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		const CMsgQAngle& msgAng = (const CMsgQAngle&) m_msg->GetReflection()->GetRepeatedMessage(*m_msg, field, index);
		out->Init(msgAng.x(), msgAng.y(), msgAng.z());

		return true;
	}

	bool SetRepeatedQAngle(const char* pszFieldName, int index, const QAngle& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		CMsgQAngle* msgAng = (CMsgQAngle*) m_msg->GetReflection()->MutableRepeatedMessage(m_msg, field, index);
		msgAng->set_x(vec.x);
		msgAng->set_y(vec.y);
		msgAng->set_z(vec.z);

		return true;
	}

	bool AddQAngle(const char* pszFieldName, const QAngle& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		CMsgQAngle* msgAng = (CMsgQAngle*) m_msg->GetReflection()->AddMessage(m_msg, field);
		msgAng->set_x(vec.x);
		msgAng->set_y(vec.y);
		msgAng->set_z(vec.z);

		return true;
	}

	bool GetMessage(const char* pszFieldName, pb::Message** message) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		*message = m_msg->GetReflection()->MutableMessage(m_msg, field);

		return true;
	}

	bool GetRepeatedMessage(const char* pszFieldName, int index, const pb::Message** message) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		const pb::Message* m = &m_msg->GetReflection()->GetRepeatedMessage(*m_msg, field, index);
		*message = m;

		return true;
	}

	bool AddMessage(const char* pszFieldName, pb::Message** message) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		*message = m_msg->GetReflection()->AddMessage(m_msg, field);

		return true;
	}

	int GetRepeatedFieldCount(const char* pszFieldName) {
		const pb::FieldDescriptor* field = m_msg->GetDescriptor()->FindFieldByName(pszFieldName);
		if (!field) return -1;

		if (field->label() != pb::FieldDescriptor::LABEL_REPEATED) return -1;

		return m_msg->GetReflection()->FieldSize(*m_msg, field);
	}

	bool RemoveRepeatedFieldValue(const char* pszFieldName, int index) {
		GETCHECK_FIELD();
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		// Protobuf guarantees that repeated field values will stay in order and so must we.
		const pb::Reflection* pReflection = m_msg->GetReflection();
		for (int i = index; i < elemCount - 1; ++i) {
			pReflection->SwapElements(m_msg, field, i, i + 1);
		}

		pReflection->RemoveLast(m_msg, field);

		return true;
	}

	std::string GetDebugString() { return m_msg->DebugString(); }
};