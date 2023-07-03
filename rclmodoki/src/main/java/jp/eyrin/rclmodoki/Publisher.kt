package jp.eyrin.rclmodoki

class Publisher<MessageType : Any> internal constructor(
    messageType: Class<MessageType>, nodePtr: Long, topic: String, qos: QoS
) {
    private var publisherPtr = 0L

    init {
        publisherPtr = createPublisher(
            nodePtr, getJNIClassName(messageType), topic, qos
        )
    }

    fun publish(message: MessageType) {
        publishImpl(publisherPtr, message)
    }

    val subscriptionCount: Int
        get() = getSubscriptionCount(publisherPtr)

    private external fun createPublisher(
        nodePtr: Long, messageTypeName: String, topic: String, qos: QoS
    ): Long

    private external fun publishImpl(publisherPtr: Long, message: Any)
    private external fun getSubscriptionCount(publisherPtr: Long): Int
}
