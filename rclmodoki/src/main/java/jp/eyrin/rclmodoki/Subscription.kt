package jp.eyrin.rclmodoki

class Subscription<MessageType : Any> internal constructor(
    messageType: Class<MessageType>,
    nodePtr: Long,
    topic: String,
    qos: QoS,
    callback: (MessageType) -> Unit
) {
    private var subscriptionPtr = 0L

    init {
        subscriptionPtr = createSubscription(
            nodePtr,
            getJNIClassName(messageType),
            topic,
            qos,
            callback,
            getJNIInitSignature(messageType)
        )
    }

    val publisherCount: Int
        get() = getPublisherCount(subscriptionPtr)

    private external fun createSubscription(
        nodePtr: Long,
        messageTypeName: String,
        topic: String,
        qos: QoS,
        callback: (MessageType) -> Unit,
        messageSig: String
    ): Long

    private external fun getPublisherCount(subscriptionPtr: Long): Int
}
