#pragma once

#include <jni.h>

#include <fastdds/dds/topic/qos/TopicQos.hpp>

#include "eprosima.hpp"

namespace rclmodoki {

inline dds::TopicQos get_fastdds_qos(JNIEnv *env, jobject obj) {
  dds::TopicQos qos = dds::TOPIC_QOS_DEFAULT;

  jclass clazz = env->FindClass("jp/eyrin/rclmodoki/QoS");

  {
    jobject history_depth = env->GetObjectField(obj, env->GetFieldID(clazz, "historyDepth", "Ljava/lang/Integer;"));
    if (history_depth != nullptr) {
      jmethodID int_value = env->GetMethodID(env->GetObjectClass(history_depth), "intValue", "()I");
      int value = env->CallIntMethod(history_depth, int_value);
      qos.history().depth = value;
    }
  }

  {
    jclass history_policy_class = env->FindClass("jp/eyrin/rclmodoki/QoS$HistoryPolicy");
    jobject keep_last = env->GetStaticObjectField(
        history_policy_class,
        env->GetStaticFieldID(history_policy_class, "KEEP_LAST", "Ljp/eyrin/rclmodoki/QoS$HistoryPolicy;"));
    jobject keep_all = env->GetStaticObjectField(
        history_policy_class,
        env->GetStaticFieldID(history_policy_class, "KEEP_ALL", "Ljp/eyrin/rclmodoki/QoS$HistoryPolicy;"));
    jobject history_policy =
        env->GetObjectField(obj, env->GetFieldID(clazz, "historyPolicy", "Ljp/eyrin/rclmodoki/QoS$HistoryPolicy;"));
    if (env->IsSameObject(keep_last, history_policy)) {
      qos.history().kind = dds::KEEP_LAST_HISTORY_QOS;
    } else if (env->IsSameObject(keep_all, history_policy)) {
      qos.history().kind = dds::KEEP_ALL_HISTORY_QOS;
    }
  }

  {
    jclass reliability_policy_class = env->FindClass("jp/eyrin/rclmodoki/QoS$ReliabilityPolicy");
    jobject best_effort = env->GetStaticObjectField(
        reliability_policy_class,
        env->GetStaticFieldID(reliability_policy_class, "BEST_EFFORT", "Ljp/eyrin/rclmodoki/QoS$ReliabilityPolicy;"));
    jobject reliable = env->GetStaticObjectField(
        reliability_policy_class,
        env->GetStaticFieldID(reliability_policy_class, "RELIABLE", "Ljp/eyrin/rclmodoki/QoS$ReliabilityPolicy;"));
    jobject reliability_policy = env->GetObjectField(
        obj, env->GetFieldID(clazz, "reliabilityPolicy", "Ljp/eyrin/rclmodoki/QoS$ReliabilityPolicy;"));
    if (env->IsSameObject(best_effort, reliability_policy)) {
      qos.reliability().kind = dds::BEST_EFFORT_RELIABILITY_QOS;
    } else if (env->IsSameObject(reliable, reliability_policy)) {
      qos.reliability().kind = dds::RELIABLE_RELIABILITY_QOS;
    }
  }

  {
    jclass durability_policy_class = env->FindClass("jp/eyrin/rclmodoki/QoS$DurabilityPolicy");
    jobject volatile_ = env->GetStaticObjectField(
        durability_policy_class,
        env->GetStaticFieldID(durability_policy_class, "VOLATILE", "Ljp/eyrin/rclmodoki/QoS$DurabilityPolicy;"));
    jobject transient_local = env->GetStaticObjectField(
        durability_policy_class,
        env->GetStaticFieldID(durability_policy_class, "TRANSIENT_LOCAL", "Ljp/eyrin/rclmodoki/QoS$DurabilityPolicy;"));
    jobject durability_policy = env->GetObjectField(
        obj, env->GetFieldID(clazz, "durabilityPolicy", "Ljp/eyrin/rclmodoki/QoS$DurabilityPolicy;"));
    if (env->IsSameObject(volatile_, durability_policy)) {
      qos.durability().kind = dds::VOLATILE_DURABILITY_QOS;
    } else if (env->IsSameObject(transient_local, durability_policy)) {
      qos.durability().kind = dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    }
  }

  {
    jclass liveliness_policy_class = env->FindClass("jp/eyrin/rclmodoki/QoS$LivelinessPolicy");
    jobject automatic = env->GetStaticObjectField(
        liveliness_policy_class,
        env->GetStaticFieldID(liveliness_policy_class, "AUTOMATIC", "Ljp/eyrin/rclmodoki/QoS$LivelinessPolicy;"));
    jobject manual_by_topic = env->GetStaticObjectField(
        liveliness_policy_class,
        env->GetStaticFieldID(liveliness_policy_class, "MANUAL_BY_TOPIC", "Ljp/eyrin/rclmodoki/QoS$LivelinessPolicy;"));
    jobject liveliness_policy = env->GetObjectField(
        obj, env->GetFieldID(clazz, "livelinessPolicy", "Ljp/eyrin/rclmodoki/QoS$LivelinessPolicy;"));
    if (env->IsSameObject(automatic, liveliness_policy)) {
      qos.liveliness().kind = dds::AUTOMATIC_LIVELINESS_QOS;
    } else if (env->IsSameObject(manual_by_topic, liveliness_policy)) {
      qos.liveliness().kind = dds::MANUAL_BY_TOPIC_LIVELINESS_QOS;
    }
  }

  {
    jobject deadline_seconds = env->GetObjectField(obj, env->GetFieldID(clazz, "deadlineSeconds", "Ljava/lang/Long;"));
    if (deadline_seconds != nullptr) {
      int deadline_nanoseconds = env->GetIntField(obj, env->GetFieldID(clazz, "deadlineNanoseconds", "I"));
      jmethodID long_value = env->GetMethodID(env->GetObjectClass(deadline_seconds), "longValue", "()J");
      long value = env->CallLongMethod(deadline_seconds, long_value);
      if (value == -1) {
        qos.deadline().period = {eprosima::fastrtps::Duration_t::INFINITE_SECONDS,
                                 eprosima::fastrtps::Duration_t::INFINITE_NANOSECONDS};
      } else {
        qos.deadline().period = {
            static_cast<int32_t>(value),
            static_cast<uint32_t>(deadline_nanoseconds),
        };
      }
    }
  }

  {
    jobject lifespan_seconds = env->GetObjectField(obj, env->GetFieldID(clazz, "lifespanSeconds", "Ljava/lang/Long;"));
    if (lifespan_seconds != nullptr) {
      int lifespan_nanoseconds = env->GetIntField(obj, env->GetFieldID(clazz, "lifespanNanoseconds", "I"));
      jmethodID long_value = env->GetMethodID(env->GetObjectClass(lifespan_seconds), "longValue", "()J");
      long value = env->CallLongMethod(lifespan_seconds, long_value);
      if (value == -1) {
        qos.lifespan().duration = {eprosima::fastrtps::Duration_t::INFINITE_SECONDS,
                                   eprosima::fastrtps::Duration_t::INFINITE_NANOSECONDS};
      } else {
        qos.lifespan().duration = {
            static_cast<int32_t>(value),
            static_cast<uint32_t>(lifespan_nanoseconds),
        };
      }
    }
  }

  return qos;
}

} // namespace rclmodoki
