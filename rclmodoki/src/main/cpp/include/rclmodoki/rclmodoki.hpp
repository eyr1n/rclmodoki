#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <jni.h>

#include <fastdds/dds/topic/TypeSupport.hpp>

#include "jni_class.hpp"

namespace rclmodoki {

class MessageType {
public:
  MessageType(eprosima::fastdds::dds::TopicDataType *message_type,
              std::function<void *(JNIEnv *, JNIClass *, jobject)> &&kt_to_cpp,
              std::function<jobject(JNIEnv *, JNIClass *, jmethodID, void *)> &&cpp_to_kt)
      : type_support(message_type), kt_to_cpp(std::move(kt_to_cpp)), cpp_to_kt(std::move(cpp_to_kt)) {}

  eprosima::fastdds::dds::TypeSupport type_support;
  std::function<void *(JNIEnv *, JNIClass *, jobject)> kt_to_cpp;
  std::function<jobject(JNIEnv *, JNIClass *, jmethodID, void *)> cpp_to_kt;
  JNIClass *jni_class;
};

using MessageTypes = std::unordered_map<std::string, MessageType>;

extern MessageTypes message_types;

inline std::string from_jstring(JNIEnv *env, jstring s) {
  const char *s_ = env->GetStringUTFChars(s, nullptr);
  std::string res{s_};
  env->ReleaseStringUTFChars(s, s_);
  return res;
}

inline jstring to_jstring(JNIEnv *env, const std::string &s) {
  return env->NewStringUTF(s.data());
}

} // namespace rclmodoki
