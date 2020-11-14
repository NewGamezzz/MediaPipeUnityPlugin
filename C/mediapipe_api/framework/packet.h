#ifndef C_MEDIAPIPE_API_FRAMEWORK_PACKET_H_
#define C_MEDIAPIPE_API_FRAMEWORK_PACKET_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "mediapipe/framework/packet.h"
#include "mediapipe_api/common.h"
#include "mediapipe_api/external/protobuf.h"

template <typename T>
class UnsafePacketHolder : public mediapipe::packet_internal::Holder<T> {
  using mediapipe::packet_internal::Holder<T>::ptr_;

 public:
  T* Get() const {
    return const_cast<T*>(ptr_);
  }
};

extern "C" {

typedef struct MpPacket {
  std::unique_ptr<mediapipe::Packet> impl;

  MpPacket() : impl { std::make_unique<mediapipe::Packet>() } {}
  MpPacket(mediapipe::Packet packet) : impl { std::make_unique<mediapipe::Packet>(std::move(packet)) } {}
} MpPacket;

typedef std::map<std::string, mediapipe::Packet> SidePacket;

/** mediapipe::Packet API */
MP_CAPI(MpReturnCode) mp_Packet__(mediapipe::Packet** packet_out);
MP_CAPI(void) mp_Packet__delete(mediapipe::Packet* packet);
MP_CAPI(MpReturnCode) mp_Packet__At__Rtimestamp(mediapipe::Packet* packet, mediapipe::Timestamp* timestamp, mediapipe::Packet** packet_out);
MP_CAPI(MpReturnCode) mp_Packet__ValidateAsProtoMessageLite(mediapipe::Packet* packet, mediapipe::Status** status_out);
MP_CAPI(MpReturnCode) mp_Packet__Timestamp(mediapipe::Packet* packet, mediapipe::Timestamp** timestamp_out);
MP_CAPI(MpReturnCode) mp_Packet__DebugString(mediapipe::Packet* packet, const char** str_out);
MP_CAPI(MpReturnCode) mp_Packet__RegisteredTypeName(mediapipe::Packet* packet, const char** str_out);
MP_CAPI(MpReturnCode) mp_Packet__DebugTypeName(mediapipe::Packet* packet, const char** str_out);

// Boolean
MP_CAPI(MpReturnCode) mp__MakeBoolPacket__b(bool value, mediapipe::Packet** packet_out);
MP_CAPI(MpReturnCode) mp__MakeBoolPacket_At__b_Rtimestamp(bool value, mediapipe::Timestamp* timestamp, mediapipe::Packet** packet_out);
MP_CAPI(MpReturnCode) mp_Packet__GetBool(mediapipe::Packet* packet, bool* value_out);
MP_CAPI(MpReturnCode) mp_Packet__ValidateAsBool(mediapipe::Packet* packet, mediapipe::Status** status_out);

// Float
MP_CAPI(MpReturnCode) mp__MakeFloatPacket__f(float value, mediapipe::Packet** packet_out);
MP_CAPI(MpReturnCode) mp__MakeFloatPacket_At__f_Rtimestamp(float value, mediapipe::Timestamp* timestamp, mediapipe::Packet** packet_out);
MP_CAPI(MpReturnCode) mp_Packet__GetFloat(mediapipe::Packet* packet, float* value_out);
MP_CAPI(MpReturnCode) mp_Packet__ValidateAsFloat(mediapipe::Packet* packet, mediapipe::Status** status_out);

// String
MP_CAPI(MpReturnCode) mp__MakeStringPacket__PKc(const char* str, mediapipe::Packet** packet_out);
MP_CAPI(MpReturnCode) mp__MakeStringPacket_At__PKc_Rtimestamp(const char* str,
                                                              mediapipe::Timestamp* timestamp,
                                                              mediapipe::Packet** packet_out);
MP_CAPI(MpReturnCode) mp_Packet__GetString(mediapipe::Packet* packet, const char** value_out);
MP_CAPI(MpReturnCode) mp_Packet__ValidateAsString(mediapipe::Packet* packet, mediapipe::Status** status_out);

/** SidePacket API */
MP_CAPI(MpReturnCode) mp_SidePacket__(SidePacket** side_packet_out);
MP_CAPI(void) mp_SidePacket__delete(SidePacket* side_packet);
MP_CAPI(MpReturnCode) mp_SidePacket__emplace__PKc_Rpacket(SidePacket* side_packet, const char* key, mediapipe::Packet* packet);
MP_CAPI(MpReturnCode) mp_SidePacket__at__PKc(SidePacket* side_packet, const char* key, mediapipe::Packet** packet_out);
MP_CAPI(MpReturnCode) mp_SidePacket__erase__PKc(SidePacket* side_packet, const char* key, int* count_out);
MP_CAPI(void) mp_SidePacket__clear(SidePacket* side_packet);
MP_CAPI(int) mp_SidePacket__size(SidePacket* side_packet);

}  // extern "C"

template <class T>
inline MpReturnCode mp_Packet__Consume(mediapipe::Packet* packet, mediapipe::StatusOr<T>** status_or_value_out) {
  TRY_ALL {
    auto status_or_value = packet->Consume<T>();

    if (status_or_value.ok()) {
      *status_or_value_out = new mediapipe::StatusOr<T> { std::move(*status_or_value.ConsumeValueOrDie().release()) };
    } else {
      *status_or_value_out = new mediapipe::StatusOr<T> { status_or_value.status() };
    }
    RETURN_CODE(MpReturnCode::Success);
  } CATCH_ALL
}

template <class T>
inline MpSerializedProto* MpPacketGetProto(MpPacket* packet) {
  auto proto = packet->impl->Get<T>();

  return MpSerializedProtoInitialize(proto);
}

template <class T>
inline MpSerializedProtoVector* MpPacketGetProtoVector(MpPacket* packet) {
  auto proto_vec = packet->impl->Get<std::vector<T>>();

  return MpSerializedProtoVectorInitialize(proto_vec);
}

#endif  // C_MEDIAPIPE_API_FRAMEWORK_PACKET_H_
