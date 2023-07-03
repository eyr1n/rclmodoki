#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <string>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include "channel.hpp"
#include "eprosima.hpp"
#include "jni_class.hpp"
#include "rclmodoki.hpp"

namespace rclmodoki {

class SubscriptionCallback {
public:
  SubscriptionCallback(MessageType &message_type, jobject callback_gref, jmethodID callback_mid, jmethodID message_mid,
                       void *message)
      : message_type_(message_type), callback_gref_(callback_gref), callback_mid_(callback_mid),
        message_mid_(message_mid), message_(message) {}

  ~SubscriptionCallback() {
    message_type_.type_support.delete_data(message_);
  }

  void invoke(JNIEnv *env) {
    env->CallVoidMethod(callback_gref_, callback_mid_,
                        message_type_.cpp_to_kt(env, message_type_.jni_class, message_mid_, message_));
  }

private:
  MessageType &message_type_;
  jobject callback_gref_;
  jmethodID callback_mid_;
  jmethodID message_mid_;
  void *message_;
};

class SubscriptionListener : public dds::DataReaderListener {
public:
  SubscriptionListener(MessageType &message_type, jobject callback_gref, jmethodID callback_mid, jmethodID message_mid,
                       Channel<SubscriptionCallback *> &channel)
      : message_type_(message_type), callback_gref_(callback_gref), callback_mid_(callback_mid),
        message_mid_(message_mid), channel_(channel) {}

  void on_subscription_matched(dds::DataReader *, const dds::SubscriptionMatchedStatus &status) override {
    count = status.current_count;
  }

  void on_data_available(dds::DataReader *reader) override {
    void *message = message_type_.type_support.create_data();
    if (reader->take_next_sample(message, &sample_info_) == ReturnCode_t::RETCODE_OK && sample_info_.valid_data) {
      channel_.produce(new SubscriptionCallback(message_type_, callback_gref_, callback_mid_, message_mid_, message));
    }
  }

  std::atomic<int32_t> count{0};

private:
  dds::SampleInfo sample_info_;
  MessageType &message_type_;
  jobject callback_gref_;
  jmethodID callback_mid_;
  jmethodID message_mid_;
  Channel<SubscriptionCallback *> &channel_;
};

class Subscription {
public:
  Subscription(dds::DomainParticipant *participant, MessageType &message_type, const std::string &topic,
               const dds::TopicQos &qos, jobject callback_gref, JNIClass *callback_class, jmethodID callback_mid,
               jmethodID message_mid, Channel<SubscriptionCallback *> &channel)
      : participant_(participant), callback_gref_(callback_gref), callback_class_(callback_class),
        listener_(message_type, callback_gref, callback_mid, message_mid, channel) {
    message_type.type_support.register_type(participant_);
    topic_ = participant_->create_topic(topic, message_type.type_support.get_type_name(), qos);
    subscriber_ = participant_->create_subscriber(dds::SUBSCRIBER_QOS_DEFAULT);

    dds::DataReaderQos reader_qos = dds::DATAREADER_QOS_DEFAULT;
    reader_qos.endpoint().history_memory_policy = rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
    reader_qos.data_sharing().off();
    reader_ = subscriber_->create_datareader(topic_, reader_qos, &listener_);
  }

  void destroy(JNIEnv *env) {
    callback_class_->destroy(env);
    env->DeleteGlobalRef(callback_gref_);
    delete this;
  }

  int32_t get_publisher_count() {
    return listener_.count;
  }

private:
  ~Subscription() {
    subscriber_->delete_datareader(reader_);
    participant_->delete_subscriber(subscriber_);
    participant_->delete_topic(topic_);
  }

  dds::DomainParticipant *participant_;
  dds::Topic *topic_;
  dds::Subscriber *subscriber_;
  dds::DataReader *reader_;
  SubscriptionListener listener_;
  jobject callback_gref_;
  JNIClass *callback_class_;
};

} // namespace rclmodoki
