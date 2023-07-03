#pragma once

#include <string>
#include <unordered_map>

#include <jni.h>

namespace rclmodoki {

class JNIClass {
public:
  JNIClass(JNIEnv *env, jclass clazz) {
    global_ref_ = static_cast<jclass>(env->NewGlobalRef(clazz));
  }

  void destroy(JNIEnv *env) {
    env->DeleteGlobalRef(global_ref_);
    delete this;
  }

  jmethodID get_method_id(JNIEnv *env, const std::string &name, const std::string &sig) {
    if (method_id_caches_.find(name) == method_id_caches_.end()) {
      method_id_caches_[name] = env->GetMethodID(global_ref_, name.c_str(), sig.c_str());
    }
    return method_id_caches_[name];
  }

  jfieldID get_field_id(JNIEnv *env, const std::string &name, const std::string &sig) {
    if (field_id_caches_.find(name) == field_id_caches_.end()) {
      field_id_caches_[name] = env->GetFieldID(global_ref_, name.c_str(), sig.c_str());
    }
    return field_id_caches_[name];
  }

  jclass get_class() {
    return global_ref_;
  }

private:
  ~JNIClass() = default;

  jclass global_ref_;
  std::unordered_map<std::string, jmethodID> method_id_caches_;
  std::unordered_map<std::string, jfieldID> field_id_caches_;
};

} // namespace rclmodoki
