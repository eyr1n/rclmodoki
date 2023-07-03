package jp.eyrin.rclmodoki

import java.util.concurrent.locks.ReentrantReadWriteLock
import kotlin.concurrent.withLock

class Node(domainId: Int, private val ns: String? = null) {
    private var nodePtr = 0L
    private val nodePtrLock = ReentrantReadWriteLock()
    private var spinThread: Thread

    init {
        nodePtrLock.writeLock().withLock {
            nodePtr = createNode(domainId)
        }
        spinThread = Thread {
            nodePtrLock.readLock().withLock {
                spin(nodePtr)
            }
        }
        spinThread.start()
    }

    fun shutdown() {
        nodePtrLock.readLock().withLock {
            stopSpin(nodePtr)
        }
        nodePtrLock.writeLock().withLock {
            destroyNode(nodePtr)
        }
    }

    inline fun <reified MessageType : Any> createPublisher(
        topic: String, qos: QoS
    ) = createPublisherImpl(MessageType::class.java, topic, qos)

    inline fun <reified MessageType : Any> createSubscription(
        topic: String, qos: QoS, noinline callback: (MessageType) -> Unit
    ) = createSubscriptionImpl(MessageType::class.java, topic, qos, callback)

    @PublishedApi
    internal fun <MessageType : Any> createPublisherImpl(
        messageType: Class<MessageType>, topic: String, qos: QoS
    ) = Publisher(messageType, nodePtr, getROS2Topic(topic), qos)

    @PublishedApi
    internal fun <MessageType : Any> createSubscriptionImpl(
        messageType: Class<MessageType>, topic: String, qos: QoS, callback: (MessageType) -> Unit
    ) = Subscription(messageType, nodePtr, getROS2Topic(topic), qos, callback)

    private fun getROS2Topic(topic: String) = if (topic[0] == '/') {
        "rt$topic"
    } else if (ns == null) {
        "rt/$topic"
    } else {
        "rt/$ns/$topic"
    }

    private external fun createNode(domainId: Int): Long
    private external fun destroyNode(nodePtr: Long)
    private external fun spin(nodePtr: Long)
    private external fun stopSpin(nodePtr: Long)
}
