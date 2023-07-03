package jp.eyrin.rclmodoki

internal fun getJNIClassName(clazz: Class<*>) = clazz.name.replace(".", "/")

internal fun getJNITypeSignature(clazz: Class<*>): String = if (clazz.isArray) {
    "[${getJNITypeSignature(clazz.componentType!!)}"
} else {
    when (clazz) {
        Boolean::class.java -> "Z"
        Byte::class.java -> "B"
        Char::class.java -> "C"
        Short::class.java -> "S"
        Int::class.java -> "I"
        Long::class.java -> "J"
        Float::class.java -> "F"
        Double::class.java -> "D"
        else -> "L${getJNIClassName(clazz)};"
    }
}

internal fun getJNIInitSignature(clazz: Class<*>) = clazz.constructors.first().parameterTypes.let { params ->
    "(${params.joinToString("") { getJNITypeSignature(it) }})V"
}
