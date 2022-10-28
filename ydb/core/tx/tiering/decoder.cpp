#include "decoder.h"
#include <library/cpp/protobuf/json/proto2json.h>
#include <contrib/libs/protobuf/src/google/protobuf/text_format.h>
#include <library/cpp/actors/core/log.h>

namespace NKikimr::NInternal {

i32 TDecoderBase::GetFieldIndex(const Ydb::ResultSet& rawData, const TString& columnId, const bool verify /*= true*/) const {
    i32 idx = 0;
    for (auto&& i : rawData.columns()) {
        if (i.name() == columnId) {
            return idx;
        }
        ++idx;
    }
    Y_VERIFY(!verify, "incorrect columnId %s", columnId.data());
    return -1;
}

bool TDecoderBase::Read(const ui32 columnIdx, TString& result, const Ydb::Value& r) const {
    result = r.items()[columnIdx].bytes_value();
    return true;
}

bool TDecoderBase::Read(const ui32 columnIdx, TDuration& result, const Ydb::Value& r) const {
    const TString& s = r.items()[columnIdx].bytes_value();
    if (!TDuration::TryParse(s, result)) {
        ALS_WARN(0) << "cannot parse duration for tiering: " << s;
        return false;
    }
    return true;
}

bool TDecoderBase::ReadDebugProto(const ui32 columnIdx, ::google::protobuf::Message& result, const Ydb::Value& r) const {
    const TString& s = r.items()[columnIdx].bytes_value();
    if (!::google::protobuf::TextFormat::ParseFromString(s, &result)) {
        ALS_ERROR(0) << "cannot parse proto string: " << s;
        return false;
    }
    return true;
}

}
