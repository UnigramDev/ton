//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2019
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/utils/port/CxCli.h"

#include "auto/tl/tonlib_api.h"
#include "auto/tl/tonlib_api.hpp"

#include "td/utils/common.h"
#include "td/utils/logging.h"
#include "td/utils/misc.h"
#include "td/utils/Slice.h"

#include <memory>

namespace Tonlib {
namespace Api {

using namespace CxCli;

public ref class NativeObject sealed {
public:
  virtual ~NativeObject() {
    if (data != nullptr) {
      get_object_ptr();
    }
  }
internal:
  explicit NativeObject(ton::tonlib_api::BaseObject *fromData) {
    data = static_cast<void *>(fromData);
  }
  ton::tonlib_api::object_ptr<ton::tonlib_api::BaseObject> get_object_ptr() {
    auto res = static_cast<ton::tonlib_api::BaseObject *>(data);
    data = nullptr;
    return ton::tonlib_api::object_ptr<ton::tonlib_api::BaseObject>(res);
  }
  void *data;
};

public interface class BaseObject {
public:
  virtual NativeObject^ ToUnmanaged();
};

// from unmanaged
inline bool FromUnmanaged(bool val) {
  return val;
}

inline int32 FromUnmanaged(int32 val) {
  return val;
}

inline int64 FromUnmanaged(int64 val) {
  return val;
}

inline float64 FromUnmanaged(float64 val) {
  return val;
}

inline String^ FromUnmanaged(const std::string &from) {
  return string_from_unmanaged(from);
}

inline String^ FromUnmanaged(td::SecureString &from) {
  td::SecureString cstr(from.size() + 1);
  cstr.as_mutable_slice().copy_from(from);
  cstr.as_mutable_slice().back() = 0;
  td::CSlice s(cstr.data(), cstr.data() + from.size());

  return string_from_unmanaged(s.c_str());
}

inline auto CLRCALL BytesFromUnmanaged(td::Slice from) {
  //const byte* data = reinterpret_cast<const byte*>(from.data());
  //return REF_NEW Vector<byte>(data, from.size());
  Array<byte> ^ res = REF_NEW Vector<byte>(static_cast<ArrayIndexType>(from.size()));
  ArrayIndexType i = 0;
  for (auto b : from) {
    ArraySet(res, i++, b);
  }
  return res;
}

inline auto CLRCALL BytesFromUnmanaged(const std::string &from) {
  Array<byte> ^ res = REF_NEW Vector<byte>(static_cast<ArrayIndexType>(from.size()));
  ArrayIndexType i = 0;
  for (auto b : from) {
    ArraySet(res, i++, b);
  }
  return res;
}

template <class FromT>
auto CLRCALL FromUnmanaged(std::vector<FromT> &vec) {
  using ToT = decltype(FromUnmanaged(vec[0]));
  Array<ToT>^ res = REF_NEW Vector<ToT>(static_cast<ArrayIndexType>(vec.size()));
  ArrayIndexType i = 0;
  for (auto &from : vec) {
    ArraySet(res, i++, FromUnmanaged(from));
  }
  return res;
}

inline auto CLRCALL BytesFromUnmanaged(const std::vector<std::string> &vec) {
  using ToT = decltype(BytesFromUnmanaged(vec[0]));
  Array<ToT>^ res = REF_NEW Vector<ToT>(static_cast<ArrayIndexType>(vec.size()));
  ArrayIndexType i = 0;
  for (auto &from : vec) {
    ArraySet(res, i++, BytesFromUnmanaged(from));
  }
  return res;
}

template <class T>
auto CLRCALL FromUnmanaged(ton::tonlib_api::object_ptr<T> &from) -> decltype(FromUnmanaged(*from.get())) {
  if (!from) {
    return nullptr;
  }
  return FromUnmanaged(*from.get());
}

#if TD_CLI
template <class ResT>
ref class CallFromUnmanagedRes {
public:
  [System::ThreadStaticAttribute]
  static property ResT res;
};

template <class ResT>
struct CallFromUnmanaged {
  template <class T>
  void operator()(T &val) const {
    CallFromUnmanagedRes<ResT>::res = FromUnmanaged(val);
  }
};
#endif

template <class ResT, class T>
ResT DoFromUnmanaged(T &from) {
#if TD_WINRT
  ResT res;
  downcast_call(from, [&](auto &from_downcasted) {
    res = FromUnmanaged(from_downcasted);
  });
  return res;
#elif TD_CLI
  CallFromUnmanaged<ResT> res;
  downcast_call(from, res);
  return CallFromUnmanagedRes<ResT>::res;
#endif
}

inline BaseObject^ FromUnmanaged(ton::tonlib_api::Function &from) {
  return DoFromUnmanaged<BaseObject^>(from);
}

inline BaseObject^ FromUnmanaged(ton::tonlib_api::Object &from) {
  return DoFromUnmanaged<BaseObject^>(from);
}

// to unmanaged
inline bool ToUnmanaged(bool val) {
  return val;
}
inline int32 ToUnmanaged(int32 val) {
  return val;
}

inline int64 ToUnmanaged(int64 val) {
  return val;
}

inline float64 ToUnmanaged(float64 val) {
  return val;
}

inline std::string ToUnmanaged(String ^from) {
  return string_to_unmanaged(from);
}

inline std::string ToUnmanaged(Array<byte>^ from) {
  if (!from) {
    return std::string();
  }

  ArrayIndexType size = ArraySize(from);
  std::string res(size, '\0');
  for (ArrayIndexType i = 0; i < size; i++) {
    res[i] = static_cast<char>(ArrayGet(from, i));
  }
  return res;
}

inline td::SecureString SecureToUnmanaged(Array<byte>^ from) {
  if (!from) {
    return td::SecureString();
  }

  ArrayIndexType size = ArraySize(from);
  std::string res(size, '\0');
  for (ArrayIndexType i = 0; i < size; i++) {
    res[i] = static_cast<char>(ArrayGet(from, i));
  }
  return td::SecureString(res);
}

inline std::vector<td::SecureString> SecureToUnmanaged(Array<String^>^ from) {
  if (!from) {
    //return td::SecureString();
  }

  //ArrayIndexType size = ArraySize(from);
  //td::SecureString res(size, '\0');
  //for (ArrayIndexType i = 0; i < size; i++) {
  //  res[i] = static_cast<char>(ArrayGet(from, i));
  //}
  std::vector<td::SecureString> res;
  return res;
}

inline td::SecureString SecureToUnmanaged(String^ from) {
  return td::SecureString(string_to_unmanaged(from));
}

template <class FromT>
auto ToUnmanaged(Array<FromT>^ from) {
  std::vector<decltype(ToUnmanaged(ArrayGet(from, 0)))> res;
  if (from && ArraySize(from)) {
    ArrayIndexType size = ArraySize(from);
    res.reserve(size);
    for (ArrayIndexType i = 0; i < size; i++) {
      res.push_back(ToUnmanaged(ArrayGet(from, i)));
    }
  }
  return res;
}

inline NativeObject^ ToUnmanaged(BaseObject^ from) {
  if (!from) {
    return REF_NEW NativeObject(nullptr);
  }

  return from->ToUnmanaged();
}

inline String^ ToString(BaseObject^ from) {
  return string_from_unmanaged(ton::tonlib_api::to_string(ToUnmanaged(from)->get_object_ptr()));
}

}  // namespace Api
}  // namespace Tonlib
