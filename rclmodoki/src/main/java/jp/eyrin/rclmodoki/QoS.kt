package jp.eyrin.rclmodoki

import kotlin.time.Duration

class QoS() {
    enum class HistoryPolicy {
        KEEP_LAST, KEEP_ALL
    }

    enum class ReliabilityPolicy {
        BEST_EFFORT, RELIABLE
    }

    enum class DurabilityPolicy {
        VOLATILE, TRANSIENT_LOCAL
    }

    enum class LivelinessPolicy {
        AUTOMATIC, MANUAL_BY_TOPIC
    }

    constructor(depth: Int) : this() {
        historyDepth = depth
    }

    fun history(policy: HistoryPolicy) = apply {
        historyPolicy = policy
    }

    fun reliability(policy: ReliabilityPolicy) = apply {
        reliabilityPolicy = policy
    }

    fun durability(policy: DurabilityPolicy) = apply {
        durabilityPolicy = policy
    }

    fun liveliness(policy: LivelinessPolicy) = apply {
        livelinessPolicy = policy
    }

    fun deadline(duration: Duration) = apply {
        if (duration == Duration.INFINITE) {
            deadlineSeconds = -1
        } else {
            duration.toComponents { seconds, nanoseconds ->
                deadlineSeconds = seconds
                deadlineNanoseconds = nanoseconds
            }
        }
    }

    fun lifespan(duration: Duration) = apply {
        if (duration == Duration.INFINITE) {
            lifespanSeconds = -1
        } else {
            duration.toComponents { seconds, nanoseconds ->
                lifespanSeconds = seconds
                lifespanNanoseconds = nanoseconds
            }
        }
    }

    private var historyDepth: Int? = null
    private var historyPolicy: HistoryPolicy? = null
    private var reliabilityPolicy: ReliabilityPolicy? = null
    private var durabilityPolicy: DurabilityPolicy? = null
    private var livelinessPolicy: LivelinessPolicy? = null
    private var deadlineSeconds: Long? = null
    private var deadlineNanoseconds: Int = 0
    private var lifespanSeconds: Long? = null
    private var lifespanNanoseconds: Int = 0
}
