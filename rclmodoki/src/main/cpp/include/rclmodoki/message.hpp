#pragma once

#include "jni_class.hpp"

namespace rclmodoki {

class MessageKt {
public:
  MessageKt(JNIEnv *env, JNIClass *clazz, jobject msg) : env_(env), clazz_(clazz), msg_(msg) {}

  JNIEnv *get_env() {
    return env_;
  }

  JNIClass *get_class() {
    return clazz_;
  }

  jobject get_message() {
    return msg_;
  }

  jobject get_object_field(const std::string &name, const std::string &sig) {
    return env_->GetObjectField(msg_, clazz_->get_field_id(env_, name, sig));
  }

  template <class T> T get_field(const std::string &name);
  template <> jboolean get_field<jboolean>(const std::string &name) {
    return env_->GetBooleanField(msg_, clazz_->get_field_id(env_, name, "Z"));
  }
  template <> jbyte get_field<jbyte>(const std::string &name) {
    return env_->GetByteField(msg_, clazz_->get_field_id(env_, name, "B"));
  }
  template <> jchar get_field<jchar>(const std::string &name) {
    return env_->GetCharField(msg_, clazz_->get_field_id(env_, name, "C"));
  }
  template <> jshort get_field<jshort>(const std::string &name) {
    return env_->GetShortField(msg_, clazz_->get_field_id(env_, name, "S"));
  }
  template <> jint get_field<jint>(const std::string &name) {
    return env_->GetIntField(msg_, clazz_->get_field_id(env_, name, "I"));
  }
  template <> jlong get_field<jlong>(const std::string &name) {
    return env_->GetLongField(msg_, clazz_->get_field_id(env_, name, "J"));
  }
  template <> jfloat get_field<jfloat>(const std::string &name) {
    return env_->GetFloatField(msg_, clazz_->get_field_id(env_, name, "F"));
  }
  template <> jdouble get_field<jdouble>(const std::string &name) {
    return env_->GetDoubleField(msg_, clazz_->get_field_id(env_, name, "D"));
  }

private:
  JNIEnv *env_;
  JNIClass *clazz_;
  jobject msg_;
};

class MessageCpp {
public:
  MessageCpp(JNIEnv *env, JNIClass *clazz, jmethodID init, void *msg)
      : env_(env), clazz_(clazz), init_(init), msg_(msg) {}

  JNIEnv *get_env() {
    return env_;
  }

  JNIClass *get_class() {
    return clazz_;
  }

  template <class T> const T &get_message() {
    return *reinterpret_cast<T *>(msg_);
  }

  template <class... Args> jobject new_object(Args &&...args) {
    return env_->NewObject(clazz_->get_class(), init_, std::forward<Args>(args)...);
  }

private:
  JNIEnv *env_;
  JNIClass *clazz_;
  jmethodID init_;
  void *msg_;
};

} // namespace rclmodoki
