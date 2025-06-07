#include <jni.h>

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include <terra/aescrypt/engine/encryptor.h>
// Write C++ code here.
//
// Do not forget to dynamically load the C++ library into your application.
//
// For instance,
//
// In MainActivity.java:
//    static {
//       System.loadLibrary("androidcrypt");
//    }
//
// Or, in MainActivity.kt:
//    companion object {
//      init {
//         System.loadLibrary("androidcrypt")
//      }
//    }

class JNIOutputBuffer : public std::streambuf {
public:
    JNIOutputBuffer(JNIEnv * javaEnv, const jobject &javaOutputStream, int bufferSize)
            : javaEnv(javaEnv), javaOutputStream(javaOutputStream), bufferSize(bufferSize), buffer(bufferSize) {
        jclass outputStreamClass = javaEnv->GetObjectClass(javaOutputStream);
        writeMethod = javaEnv->GetMethodID(outputStreamClass, "write", "([B)V");
        flushMethod = javaEnv->GetMethodID(outputStreamClass, "flush", "()V");
        setp(&buffer.front(), &buffer.back());
    }
protected:
    int overflow(int c) override {
        sync();//write out buffer and reset pointers to "empty" it
        sputc(c);//put char that caused the overflow into now empty buffer
        return c;
    }
    int sync() override {
        jsize length = static_cast<jsize>(pptr()-pbase());
        jbyteArray javaByteArray = javaEnv->NewByteArray(length);
        javaEnv->SetByteArrayRegion(javaByteArray, 0, length, reinterpret_cast<const jbyte*>(&buffer.front()));
        javaEnv->CallVoidMethod(javaOutputStream, writeMethod, javaByteArray);
        javaEnv->CallVoidMethod(javaOutputStream, flushMethod);

        setp(&buffer.front(), &buffer.back());
        return std::streambuf::sync();
    };
private:
    JNIEnv * javaEnv;
    jmethodID writeMethod;
    jmethodID flushMethod;
    const jobject & javaOutputStream;
    const int bufferSize;
    std::vector<char> buffer;
};

class JNIOstream : public std::ostream {
private:
    JNIOutputBuffer buffer;
public:
    JNIOstream(JNIEnv * javaEnv, const jobject & javaOutputStream, int bufferSize) : std::ostream(&buffer), buffer(javaEnv, javaOutputStream, bufferSize) {}
};

class JNIInputBuffer : public std::streambuf {
public:
    JNIInputBuffer(JNIEnv *javaEnv, const jobject &javaInputStream, int bufferSize): javaEnv(javaEnv), javaInputStream(javaInputStream), bufferSize(bufferSize), buffer(bufferSize){
        jclass inputStreamClass = javaEnv->GetObjectClass(javaInputStream);
        readMethod = javaEnv->GetMethodID(inputStreamClass, "read", "([B)I");
        setg(&buffer.front(), &buffer.front(), &buffer.front());
    }
protected:
    int underflow() override {
        if (gptr() < egptr()) {
            return traits_type::to_int_type(*gptr());
        }
        jsize length = static_cast<jsize>(bufferSize);
        jbyteArray javaByteArray = javaEnv->NewByteArray(length);
        // Read from Java InputStream
        jint bytesRead = javaEnv->CallIntMethod(javaInputStream, readMethod, javaByteArray);
        if (bytesRead <= 0) {
            return traits_type::eof();
        }
        // Copy data from Java buffer to C++ buffer
        javaEnv->GetByteArrayRegion(javaByteArray, 0, bytesRead, reinterpret_cast<jbyte*>(&buffer.front()));
        // Set buffer pointers
        setg(&buffer.front(), &buffer.front(), &buffer.front() + bytesRead);
        return traits_type::to_int_type(*gptr());


        /*int bytesRead = javaEnv->CallIntMethod(javaInputStream, readMethod, javaByteArray);
        if (bytesRead == -1) {
            return Traits::eof();
        }*/
    }
private:
    JNIEnv * javaEnv;
    jmethodID readMethod;
    const jobject & javaInputStream;
    const int bufferSize;
    std::vector<char> buffer;
};

class JNIIstream : public std::istream {
private:
    JNIInputBuffer buffer;
public:
    JNIIstream(JNIEnv * javaEnv, const jobject & javaInputStream, const int bufferSize): std::istream(&buffer), buffer(javaEnv, javaInputStream, bufferSize) {}
};


extern "C"
JNIEXPORT void JNICALL
Java_com_dewdrop623_androidcrypt_JNIInterface_encrypt(JNIEnv *env, jclass jclass1, jobject inputStream, jobject outputStream, jobject progressCallback, jobject logStream) {
    jclass progressCallbackClass = env->GetObjectClass(progressCallback);
    jmethodID progressCallbackMethodId = env->GetMethodID(progressCallbackClass, "progressCallback", "(I)V");

    //TODO make password a parameter
    std::u8string password = u8"test";

    //TODO define these somewhere else?
    int bufferSize = 1049000;
    int iterations = 300000;

    JNIIstream jniIstream{env, inputStream, bufferSize};
    JNIOstream jniOstream{env, outputStream, bufferSize};

    Terra::AESCrypt::Engine::EncryptResult encrypt_result{};

    //TODO pass logger to constructor
    Terra::AESCrypt::Engine::Encryptor encryptor{};

    std::vector<std::pair<std::string, std::string>> extensions = {};
    encrypt_result = encryptor.Encrypt(static_cast<std::u8string>(password), iterations, jniIstream, jniOstream, extensions, NULL, 0);

    // *** IMPORTANT NOTE: FINAL FLUSH IS REQUIRED TO WRITE LAST DATAi ***
    jniOstream.flush();
}