#pragma once

#include <networkbasetypes.pb.h>
#include <networksystem/inetworkmessages.h>

namespace pb = google::protobuf;

#define GETCHECK_FIELD()                                                                          \
	const pb::FieldDescriptor* field = msg->GetDescriptor()->FindFieldByName(pszFieldName); \
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
	int elemCount = msg->GetReflection()->FieldSize(*msg, field); \
	if (elemCount == 0 || idx >= elemCount || idx < 0) {          \
		return false;                                             \
	};

class INetworkMessageInternal;
namespace google::protobuf {
	class Message;
}

class UserMessage {
public:
	UserMessage(INetworkMessageInternal* msgSerializable, const CNetMessage* message, int nRecipientCount, uint64* recipientMask)
		: msgSerializable(msgSerializable), msg(const_cast<CNetMessage*>(message)->ToPB<pb::Message>()), nRecipientCount(nRecipientCount),
		  recipientMask(recipientMask) {
	}

	explicit UserMessage(const char* messageName) {
		manuallyAllocated = true;
		msgSerializable = g_pNetworkMessages->FindNetworkMessagePartial(messageName);
		if (!msgSerializable) return;

		msg = msgSerializable->AllocateMessage()->ToPB<pb::Message>();
		recipientMask = new uint64(0);
	}

	explicit UserMessage(int messageId) {
		manuallyAllocated = true;
		msgSerializable = g_pNetworkMessages->FindNetworkMessageById(messageId);
		if (!msgSerializable) return;

		msg = msgSerializable->AllocateMessage()->ToPB<pb::Message>();
		recipientMask = new uint64(0);
	}

	~UserMessage() {
		if (manuallyAllocated) delete recipientMask;
	}

	std::string GetMessageName();
	int GetMessageID();
	bool HasField(const std::string& fieldName);
	const CNetMessagePB<pb::Message>* GetProtobufMessage();
	INetworkMessageInternal* GetSerializableMessage() { return msgSerializable; }
	uint64* GetRecipientMask() { return recipientMask; }
	bool IsManuallyAllocated() const { return manuallyAllocated; }

private:
	INetworkMessageInternal* msgSerializable{};
	CNetMessagePB<pb::Message>* msg{};
	int nRecipientCount{};
	bool manuallyAllocated{};
	uint64* recipientMask{};

public:
	bool HasField(const char* pszFieldName) {
		GETCHECK_FIELD();
		CHECK_FIELD_NOT_REPEATED();
		return msg->GetReflection()->HasField(*msg, field);
	}

	bool GetInt32(const char* pszFieldName, int32* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_NOT_REPEATED();

		*out = msg->GetReflection()->GetInt32(*msg, field);
		return true;
	}

	bool SetInt32(const char* pszFieldName, int32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_NOT_REPEATED();

		msg->GetReflection()->SetInt32(msg, field, value);
		return true;
	}

	bool GetRepeatedInt32(const char* pszFieldName, int index, int32* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = msg->GetReflection()->GetRepeatedInt32(*msg, field, index);
		return true;
	}

	bool SetRepeatedInt32(const char* pszFieldName, int index, int32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		msg->GetReflection()->SetRepeatedInt32(msg, field, index, value);
		return true;
	}

	bool AddInt32(const char* pszFieldName, int32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT32);
		CHECK_FIELD_REPEATED();

		msg->GetReflection()->AddInt32(msg, field, value);
		return true;
	}

	bool GetInt64(const char* pszFieldName, int64* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_NOT_REPEATED();

		*out = msg->GetReflection()->GetInt64(*msg, field);
		return true;
	}

	bool SetInt64(const char* pszFieldName, int64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_NOT_REPEATED();

		msg->GetReflection()->SetInt64(msg, field, value);
		return true;
	}

	bool GetRepeatedInt64(const char* pszFieldName, int index, int64* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = msg->GetReflection()->GetRepeatedInt64(*msg, field, index);
		return true;
	}

	bool SetRepeatedInt64(const char* pszFieldName, int index, int64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		msg->GetReflection()->SetRepeatedInt64(msg, field, index, value);
		return true;
	}

	bool AddInt64(const char* pszFieldName, int64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(INT64);
		CHECK_FIELD_REPEATED();

		msg->GetReflection()->AddInt64(msg, field, value);
		return true;
	}

	bool GetUInt32(const char* pszFieldName, uint32* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_NOT_REPEATED();

		*out = msg->GetReflection()->GetUInt32(*msg, field);
		return true;
	}

	bool SetUInt32(const char* pszFieldName, uint32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_NOT_REPEATED();

		msg->GetReflection()->SetUInt32(msg, field, value);
		return true;
	}

	bool GetRepeatedUInt32(const char* pszFieldName, int index, uint32* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = msg->GetReflection()->GetRepeatedUInt32(*msg, field, index);
		return true;
	}

	bool SetRepeatedUInt32(const char* pszFieldName, int index, uint32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		msg->GetReflection()->SetRepeatedUInt32(msg, field, index, value);
		return true;
	}

	bool AddUInt32(const char* pszFieldName, uint32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT32);
		CHECK_FIELD_REPEATED();

		msg->GetReflection()->AddUInt32(msg, field, value);
		return true;
	}

	bool GetUInt64(const char* pszFieldName, uint64* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_NOT_REPEATED();

		*out = msg->GetReflection()->GetUInt64(*msg, field);
		return true;
	}

	bool SetUInt64(const char* pszFieldName, uint64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_NOT_REPEATED();

		msg->GetReflection()->SetUInt64(msg, field, value);
		return true;
	}

	bool GetRepeatedUInt64(const char* pszFieldName, int index, uint64* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = msg->GetReflection()->GetRepeatedUInt64(*msg, field, index);
		return true;
	}

	bool SetRepeatedUInt64(const char* pszFieldName, int index, uint64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		msg->GetReflection()->SetRepeatedUInt64(msg, field, index, value);
		return true;
	}

	bool AddUInt64(const char* pszFieldName, uint64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(UINT64);
		CHECK_FIELD_REPEATED();

		msg->GetReflection()->AddUInt32(msg, field, value);
		return true;
	}

	bool GetInt32OrUnsignedOrEnum(const char* pszFieldName, int32* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE3(INT32, UINT32, ENUM);
		CHECK_FIELD_NOT_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT32) *out = (int32) msg->GetReflection()->GetUInt32(*msg, field);
		else if (fieldType == pb::FieldDescriptor::CPPTYPE_INT32)
			*out = msg->GetReflection()->GetInt32(*msg, field);
		else// CPPTYPE_ENUM
			*out = msg->GetReflection()->GetEnum(*msg, field)->number();

		return true;
	}

	bool GetInt64OrUnsigned(const char* pszFieldName, int64* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(INT64, UINT64);
		CHECK_FIELD_NOT_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT64) *out = (int64) msg->GetReflection()->GetUInt64(*msg, field);
		else
			*out = msg->GetReflection()->GetInt64(*msg, field);

		return true;
	}

	bool SetInt32OrUnsignedOrEnum(const char* pszFieldName, int32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE3(INT32, UINT32, ENUM);
		CHECK_FIELD_NOT_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT32) {
			msg->GetReflection()->SetUInt32(msg, field, (uint32) value);
		} else if (fieldType == pb::FieldDescriptor::CPPTYPE_INT32) {
			msg->GetReflection()->SetInt32(msg, field, value);
		} else// CPPTYPE_ENUM
		{
			const pb::EnumValueDescriptor* pEnumValue = field->enum_type()->FindValueByNumber(value);
			if (!pEnumValue) return false;

			msg->GetReflection()->SetEnum(msg, field, pEnumValue);
		}

		return true;
	}

	bool SetInt64OrUnsigned(const char* pszFieldName, int64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(INT64, UINT64);
		CHECK_FIELD_NOT_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT64) {
			msg->GetReflection()->SetUInt64(msg, field, (uint64) value);
		} else {
			msg->GetReflection()->SetInt64(msg, field, value);
		}

		return true;
	}

	bool GetRepeatedInt32OrUnsignedOrEnum(const char* pszFieldName, int index, int32* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE3(INT32, UINT32, ENUM);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT32)
			*out = (int32) msg->GetReflection()->GetRepeatedUInt32(*msg, field, index);
		else if (fieldType == pb::FieldDescriptor::CPPTYPE_INT32)
			*out = msg->GetReflection()->GetRepeatedInt32(*msg, field, index);
		else// CPPTYPE_ENUM
			*out = msg->GetReflection()->GetRepeatedEnum(*msg, field, index)->number();

		return true;
	}

	bool GetRepeatedInt64OrUnsigned(const char* pszFieldName, int index, int64* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(INT64, UINT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT64)
			*out = (int64) msg->GetReflection()->GetRepeatedUInt64(*msg, field, index);
		else
			*out = msg->GetReflection()->GetRepeatedInt64(*msg, field, index);

		return true;
	}

	bool SetRepeatedInt32OrUnsignedOrEnum(const char* pszFieldName, int index, int32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE3(INT32, UINT32, ENUM);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT32) {
			msg->GetReflection()->SetRepeatedUInt32(msg, field, index, (uint32) value);
		} else if (fieldType == pb::FieldDescriptor::CPPTYPE_INT32) {
			msg->GetReflection()->SetRepeatedInt32(msg, field, index, value);
		} else// CPPTYPE_ENUM
		{
			const pb::EnumValueDescriptor* pEnumValue = field->enum_type()->FindValueByNumber(value);
			if (!pEnumValue) return false;

			msg->GetReflection()->SetRepeatedEnum(msg, field, index, pEnumValue);
		}

		return true;
	}

	bool SetRepeatedInt64OrUnsigned(const char* pszFieldName, int index, int64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(INT64, UINT64);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT64) {
			msg->GetReflection()->SetRepeatedUInt64(msg, field, index, (uint64) value);
		} else {
			msg->GetReflection()->SetRepeatedInt64(msg, field, index, value);
		}

		return true;
	}

	bool AddInt32OrUnsignedOrEnum(const char* pszFieldName, int32 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE3(INT32, UINT32, ENUM);
		CHECK_FIELD_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT32) {
			msg->GetReflection()->AddUInt32(msg, field, (uint32) value);
		} else if (fieldType == pb::FieldDescriptor::CPPTYPE_INT32) {
			msg->GetReflection()->AddInt32(msg, field, value);
		} else// CPPTYPE_ENUM
		{
			const pb::EnumValueDescriptor* pEnumValue = field->enum_type()->FindValueByNumber(value);
			if (!pEnumValue) return false;

			msg->GetReflection()->AddEnum(msg, field, pEnumValue);
		}

		return true;
	}

	bool AddInt64OrUnsigned(const char* pszFieldName, int64 value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(INT64, UINT64);
		CHECK_FIELD_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_UINT64) {
			msg->GetReflection()->AddUInt64(msg, field, (uint64) value);
		} else {
			msg->GetReflection()->AddInt64(msg, field, value);
		}

		return true;
	}

	bool GetBool(const char* pszFieldName, bool* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_NOT_REPEATED();

		*out = msg->GetReflection()->GetBool(*msg, field);
		return true;
	}

	bool SetBool(const char* pszFieldName, bool value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_NOT_REPEATED();

		msg->GetReflection()->SetBool(msg, field, value);
		return true;
	}

	bool GetRepeatedBool(const char* pszFieldName, int index, bool* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = msg->GetReflection()->GetRepeatedBool(*msg, field, index);
		return true;
	}

	bool SetRepeatedBool(const char* pszFieldName, int index, bool value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		msg->GetReflection()->SetRepeatedBool(msg, field, index, value);
		return true;
	}

	bool AddBool(const char* pszFieldName, bool value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(BOOL);
		CHECK_FIELD_REPEATED();

		msg->GetReflection()->AddBool(msg, field, value);
		return true;
	}

	bool GetFloat(const char* pszFieldName, float* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_NOT_REPEATED();

		*out = msg->GetReflection()->GetFloat(*msg, field);
		return true;
	}

	bool SetFloat(const char* pszFieldName, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_NOT_REPEATED();

		msg->GetReflection()->SetFloat(msg, field, value);
		return true;
	}

	bool GetRepeatedFloat(const char* pszFieldName, int index, float* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = msg->GetReflection()->GetRepeatedFloat(*msg, field, index);
		return true;
	}

	bool SetRepeatedFloat(const char* pszFieldName, int index, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		msg->GetReflection()->SetRepeatedFloat(msg, field, index, value);
		return true;
	}

	bool AddFloat(const char* pszFieldName, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(FLOAT);
		CHECK_FIELD_REPEATED();

		msg->GetReflection()->AddFloat(msg, field, value);
		return true;
	}

	bool GetDouble(const char* pszFieldName, double* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_NOT_REPEATED();

		*out = msg->GetReflection()->GetDouble(*msg, field);
		return true;
	}

	bool SetDouble(const char* pszFieldName, double value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_NOT_REPEATED();

		msg->GetReflection()->SetDouble(msg, field, value);
		return true;
	}

	bool GetRepeatedDouble(const char* pszFieldName, int index, double* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		*out = msg->GetReflection()->GetRepeatedDouble(*msg, field, index);
		return true;
	}

	bool SetRepeatedDouble(const char* pszFieldName, int index, double value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		msg->GetReflection()->SetRepeatedDouble(msg, field, index, value);
		return true;
	}

	bool AddDouble(const char* pszFieldName, double value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(DOUBLE);
		CHECK_FIELD_REPEATED();

		msg->GetReflection()->AddDouble(msg, field, value);
		return true;
	}

	bool GetFloatOrDouble(const char* pszFieldName, float* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(FLOAT, DOUBLE);
		CHECK_FIELD_NOT_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_DOUBLE) *out = (float) msg->GetReflection()->GetDouble(*msg, field);
		else
			*out = msg->GetReflection()->GetFloat(*msg, field);

		return true;
	}

	bool SetFloatOrDouble(const char* pszFieldName, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(FLOAT, DOUBLE);
		CHECK_FIELD_NOT_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_DOUBLE) msg->GetReflection()->SetDouble(msg, field, (double) value);
		else
			msg->GetReflection()->SetFloat(msg, field, value);

		return true;
	}

	bool GetRepeatedFloatOrDouble(const char* pszFieldName, int index, float* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(FLOAT, DOUBLE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		if (fieldType == pb::FieldDescriptor::CPPTYPE_DOUBLE)
			*out = (float) msg->GetReflection()->GetRepeatedDouble(*msg, field, index);
		else
			*out = msg->GetReflection()->GetRepeatedFloat(*msg, field, index);

		return true;
	}

	bool SetRepeatedFloatOrDouble(const char* pszFieldName, int index, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(FLOAT, DOUBLE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		if (fieldType == pb::FieldDescriptor::CPPTYPE_DOUBLE)
			msg->GetReflection()->SetRepeatedDouble(msg, field, index, (double) value);
		else
			msg->GetReflection()->SetRepeatedFloat(msg, field, index, value);

		return true;
	}

	bool AddFloatOrDouble(const char* pszFieldName, float value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE2(FLOAT, DOUBLE);
		CHECK_FIELD_REPEATED();

		if (fieldType == pb::FieldDescriptor::CPPTYPE_DOUBLE) msg->GetReflection()->AddDouble(msg, field, (double) value);
		else
			msg->GetReflection()->AddFloat(msg, field, value);

		return true;
	}

	bool GetString(const char* pszFieldName, std::string& out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_NOT_REPEATED();

		out = msg->GetReflection()->GetString(*msg, field);

		return true;
	}

	bool SetString(const char* pszFieldName, std::string value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_NOT_REPEATED();

		msg->GetReflection()->SetString(msg, field, std::move(value));
		return true;
	}

	bool GetRepeatedString(const char* pszFieldName, int index, std::string& out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		out = msg->GetReflection()->GetRepeatedString(*msg, field, index);

		return true;
	}

	bool SetRepeatedString(const char* pszFieldName, int index, std::string value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		msg->GetReflection()->SetRepeatedString(msg, field, index, std::move(value));

		return true;
	}

	bool AddString(const char* pszFieldName, const char* value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(STRING);
		CHECK_FIELD_REPEATED();

		msg->GetReflection()->AddString(msg, field, value);
		return true;
	}

	bool GetColor(const char* pszFieldName, Color* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		auto msgRGBA = *(CMsgRGBA*) msg->GetReflection()->MutableMessage(msg, field);
		out->SetColor(msgRGBA.r(), msgRGBA.g(), msgRGBA.b(), msgRGBA.a());

		return true;
	}

	bool SetColor(const char* pszFieldName, const Color& value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		CMsgRGBA* msgRGBA = (CMsgRGBA*) msg->GetReflection()->MutableMessage(msg, field);
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

		const CMsgRGBA& msgRGBA = (const CMsgRGBA&) msg->GetReflection()->GetRepeatedMessage(*msg, field, index);
		out->SetColor(msgRGBA.r(), msgRGBA.g(), msgRGBA.b(), msgRGBA.a());

		return true;
	}

	bool SetRepeatedColor(const char* pszFieldName, int index, const Color& value) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		CMsgRGBA* msgRGBA = (CMsgRGBA*) msg->GetReflection()->MutableRepeatedMessage(msg, field, index);
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

		CMsgRGBA* msgRGBA = (CMsgRGBA*) msg->GetReflection()->AddMessage(msg, field);
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

		auto msgVec2d = *(CMsgVector2D*) msg->GetReflection()->MutableMessage(msg, field);
		out->Init(msgVec2d.x(), msgVec2d.y());

		return true;
	}

	bool SetVector2D(const char* pszFieldName, Vector2D& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		CMsgVector2D* msgVec2d = (CMsgVector2D*) msg->GetReflection()->MutableMessage(msg, field);
		msgVec2d->set_x(vec.x);
		msgVec2d->set_y(vec.y);

		return true;
	}

	bool GetRepeatedVector2D(const char* pszFieldName, int index, Vector2D* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		const CMsgVector2D& msgVec2d = (const CMsgVector2D&) msg->GetReflection()->GetRepeatedMessage(*msg, field, index);
		out->Init(msgVec2d.x(), msgVec2d.y());

		return true;
	}

	bool SetRepeatedVector2D(const char* pszFieldName, int index, Vector2D& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		CMsgVector2D* msgVec2d = (CMsgVector2D*) msg->GetReflection()->MutableRepeatedMessage(msg, field, index);
		msgVec2d->set_x(vec.x);
		msgVec2d->set_y(vec.y);

		return true;
	}

	bool AddVector2D(const char* pszFieldName, Vector2D& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		CMsgVector2D* msgVec2d = (CMsgVector2D*) msg->GetReflection()->AddMessage(msg, field);
		msgVec2d->set_x(vec.x);
		msgVec2d->set_y(vec.y);

		return true;
	}

	bool GetVector(const char* pszFieldName, Vector* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		auto msgVec = *(CMsgVector*) msg->GetReflection()->MutableMessage(msg, field);
		out->Init(msgVec.x(), msgVec.y(), msgVec.z());

		return true;
	}

	bool SetVector(const char* pszFieldName, Vector& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		CMsgVector* msgVec = (CMsgVector*) msg->GetReflection()->MutableMessage(msg, field);
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

		const CMsgVector& msgVec = (const CMsgVector&) msg->GetReflection()->GetRepeatedMessage(*msg, field, index);
		out->Init(msgVec.x(), msgVec.y(), msgVec.z());

		return true;
	}

	bool SetRepeatedVector(const char* pszFieldName, int index, Vector& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		CMsgVector* msgVec = (CMsgVector*) msg->GetReflection()->MutableRepeatedMessage(msg, field, index);
		msgVec->set_x(vec.x);
		msgVec->set_y(vec.y);
		msgVec->set_z(vec.z);

		return true;
	}

	bool AddVector(const char* pszFieldName, Vector& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		CMsgVector* msgVec = (CMsgVector*) msg->GetReflection()->AddMessage(msg, field);
		msgVec->set_x(vec.x);
		msgVec->set_y(vec.y);
		msgVec->set_z(vec.z);

		return true;
	}

	bool GetQAngle(const char* pszFieldName, QAngle* out) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		auto msgAng = *(CMsgQAngle*) msg->GetReflection()->MutableMessage(msg, field);
		out->Init(msgAng.x(), msgAng.y(), msgAng.z());

		return true;
	}

	bool SetQAngle(const char* pszFieldName, QAngle& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		CMsgQAngle* msgAng = (CMsgQAngle*) msg->GetReflection()->MutableMessage(msg, field);
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

		const CMsgQAngle& msgAng = (const CMsgQAngle&) msg->GetReflection()->GetRepeatedMessage(*msg, field, index);
		out->Init(msgAng.x(), msgAng.y(), msgAng.z());

		return true;
	}

	bool SetRepeatedQAngle(const char* pszFieldName, int index, QAngle& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		CMsgQAngle* msgAng = (CMsgQAngle*) msg->GetReflection()->MutableRepeatedMessage(msg, field, index);
		msgAng->set_x(vec.x);
		msgAng->set_y(vec.y);
		msgAng->set_z(vec.z);

		return true;
	}

	bool AddQAngle(const char* pszFieldName, QAngle& vec) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		CMsgQAngle* msgAng = (CMsgQAngle*) msg->GetReflection()->AddMessage(msg, field);
		msgAng->set_x(vec.x);
		msgAng->set_y(vec.y);
		msgAng->set_z(vec.z);

		return true;
	}

	bool GetMessage(const char* pszFieldName, pb::Message** message) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_NOT_REPEATED();

		*message = msg->GetReflection()->MutableMessage(msg, field);

		return true;
	}

	bool GetRepeatedMessage(const char* pszFieldName, int index, const pb::Message** message) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		const pb::Message* m = &msg->GetReflection()->GetRepeatedMessage(*msg, field, index);
		*message = m;

		return true;
	}

	bool AddMessage(const char* pszFieldName, pb::Message** message) {
		GETCHECK_FIELD();
		CHECK_FIELD_TYPE(MESSAGE);
		CHECK_FIELD_REPEATED();

		*message = msg->GetReflection()->AddMessage(msg, field);

		return true;
	}

	int GetRepeatedFieldCount(const char* pszFieldName) {
		const pb::FieldDescriptor* field = msg->GetDescriptor()->FindFieldByName(pszFieldName);
		if (!field) return -1;

		if (field->label() != pb::FieldDescriptor::LABEL_REPEATED) return -1;

		return msg->GetReflection()->FieldSize(*msg, field);
	}

	bool RemoveRepeatedFieldValue(const char* pszFieldName, int index) {
		GETCHECK_FIELD();
		CHECK_FIELD_REPEATED();
		CHECK_REPEATED_ELEMENT(index);

		// Protobuf guarantees that repeated field values will stay in order and so must we.
		const pb::Reflection* pReflection = msg->GetReflection();
		for (int i = index; i < elemCount - 1; ++i) {
			pReflection->SwapElements(msg, field, i, i + 1);
		}

		pReflection->RemoveLast(msg, field);

		return true;
	}

	std::string GetDebugString() { return msg->DebugString(); }
};