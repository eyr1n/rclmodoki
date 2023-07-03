#include <jni.h>

#include "jni_class.hpp"
#include "node.hpp"
#include "qos.hpp"
#include "rclmodoki.hpp"

namespace rclmodoki {

// Node
jlong node_create_node(JNIEnv *env, jobject thiz, jint domain_id) {
  return reinterpret_cast<jlong>(new Node(domain_id));
}

void node_destroy_node(JNIEnv *env, jobject thiz, jlong node_ptr) {
  auto node = reinterpret_cast<Node *>(node_ptr);
  node->destroy(env);
}

void node_spin(JNIEnv *env, jobject thiz, jlong node_ptr) {
  auto node = reinterpret_cast<Node *>(node_ptr);
  node->spin(env);
}

void node_stop_spin(JNIEnv *env, jobject thiz, jlong node_ptr) {
  auto node = reinterpret_cast<Node *>(node_ptr);
  node->stop_spin();
}

// Publisher
jlong publisher_create_publisher(JNIEnv *env, jobject thiz, jlong node_ptr, jstring message_type_name, jstring topic,
                                 jobject qos) {
  auto node = reinterpret_cast<Node *>(node_ptr);
  return reinterpret_cast<jlong>(node->create_publisher(message_types.at(from_jstring(env, message_type_name)),
                                                        from_jstring(env, topic), get_fastdds_qos(env, qos)));
}

void publisher_publish_impl(JNIEnv *env, jobject thiz, jlong publisher_ptr, jobject message) {
  auto publisher = reinterpret_cast<Publisher *>(publisher_ptr);
  publisher->publish(env, message);
}

jint publisher_get_subscription_count(JNIEnv *env, jobject thiz, jlong publisher_ptr) {
  auto publisher = reinterpret_cast<Publisher *>(publisher_ptr);
  return publisher->get_subscription_count();
}

// Subscription
jlong subscription_create_subscription(JNIEnv *env, jobject thiz, jlong node_ptr, jstring message_type_name,
                                       jstring topic, jobject qos, jobject callback, jstring message_sig) {
  auto node = reinterpret_cast<Node *>(node_ptr);
  std::string message_type_name_ = from_jstring(env, message_type_name);
  auto *callback_class = new JNIClass(env, env->GetObjectClass(callback));
  jobject callback_gref = env->NewGlobalRef(callback);
  jmethodID callback_mid = callback_class->get_method_id(env, "invoke", "(L" + message_type_name_ + ";)V");
  jmethodID message_mid = message_types.at(from_jstring(env, message_type_name))
                              .jni_class->get_method_id(env, "<init>", from_jstring(env, message_sig));
  return reinterpret_cast<jlong>(node->create_subscription(message_types.at(message_type_name_),
                                                           from_jstring(env, topic), get_fastdds_qos(env, qos),
                                                           callback_gref, callback_class, callback_mid, message_mid));
}

jint subscription_get_publisher_count(JNIEnv *env, jobject thiz, jlong subscription_ptr) {
  auto subscription = reinterpret_cast<Subscription *>(subscription_ptr);
  return subscription->get_publisher_count();
}

void rclmodoki_init(JNIEnv *env) {
  // Init MessageTypes
  for (auto &message_type : message_types) {
    message_type.second.jni_class = new JNIClass(env, env->FindClass(message_type.first.c_str()));
  }

  // Node
  jclass node_class = env->FindClass("jp/eyrin/rclmodoki/Node");
  static const JNINativeMethod node_methods[] = {
      {"createNode", "(I)J", reinterpret_cast<void *>(node_create_node)},
      {"destroyNode", "(J)V", reinterpret_cast<void *>(node_destroy_node)},
      {"spin", "(J)V", reinterpret_cast<void *>(node_spin)},
      {"stopSpin", "(J)V", reinterpret_cast<void *>(node_stop_spin)},
  };
  env->RegisterNatives(node_class, node_methods, sizeof(node_methods) / sizeof(JNINativeMethod));

  // Publisher
  jclass publisher_class = env->FindClass("jp/eyrin/rclmodoki/Publisher");
  static const JNINativeMethod publisher_methods[] = {
      {"createPublisher", "(JLjava/lang/String;Ljava/lang/String;Ljp/eyrin/rclmodoki/QoS;)J",
       reinterpret_cast<void *>(publisher_create_publisher)},
      {"publishImpl", "(JLjava/lang/Object;)V", reinterpret_cast<void *>(publisher_publish_impl)},
      {"getSubscriptionCount", "(J)I", reinterpret_cast<void *>(publisher_get_subscription_count)},
  };
  env->RegisterNatives(publisher_class, publisher_methods, sizeof(publisher_methods) / sizeof(JNINativeMethod));

  // Subscription
  jclass subscription_class = env->FindClass("jp/eyrin/rclmodoki/Subscription");
  static const JNINativeMethod subscription_methods[] = {
      {"createSubscription",
       "(JLjava/lang/String;Ljava/lang/String;Ljp/eyrin/rclmodoki/QoS;Lkotlin/jvm/functions/"
       "Function1;Ljava/lang/String;)J",
       reinterpret_cast<void *>(subscription_create_subscription)},
      {"getPublisherCount", "(J)I", reinterpret_cast<void *>(subscription_get_publisher_count)},
  };
  env->RegisterNatives(subscription_class, subscription_methods,
                       sizeof(subscription_methods) / sizeof(JNINativeMethod));
}

} // namespace rclmodoki
