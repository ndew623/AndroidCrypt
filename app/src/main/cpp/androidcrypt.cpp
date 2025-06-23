#include <jni.h>

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <map>

#include <terra/aescrypt/engine/encryptor.h>
#include <terra/aescrypt/engine/decryptor.h>
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

const std::map<Terra::AESCrypt::Engine::EncryptResult, int> ENCRYPT_RESULT_CODES = {
        {Terra::AESCrypt::Engine::EncryptResult::Success, 1},
        {Terra::AESCrypt::Engine::EncryptResult::IOError, 2},
        {Terra::AESCrypt::Engine::EncryptResult::InvalidExtension, 3},
        {Terra::AESCrypt::Engine::EncryptResult::InvalidPassword, 4},
        {Terra::AESCrypt::Engine::EncryptResult::InvalidIterations, 5},
        {Terra::AESCrypt::Engine::EncryptResult::AlreadyEncrypting, 6},
        {Terra::AESCrypt::Engine::EncryptResult::EncryptionCancelled, 7},
        {Terra::AESCrypt::Engine::EncryptResult::InternalError, 8},
};
const std::map<Terra::AESCrypt::Engine::DecryptResult, int> DECRYPT_RESULT_CODES = {
        {Terra::AESCrypt::Engine::DecryptResult::Success, 101},
        {Terra::AESCrypt::Engine::DecryptResult::InvalidAESCryptStream, 102},
        {Terra::AESCrypt::Engine::DecryptResult::UnsupportedAESCryptVersion, 103},
        {Terra::AESCrypt::Engine::DecryptResult::IOError, 104},
        {Terra::AESCrypt::Engine::DecryptResult::InvalidPassword, 105},
        {Terra::AESCrypt::Engine::DecryptResult::InvalidIterations, 106},
        {Terra::AESCrypt::Engine::DecryptResult::AlteredMessage, 107},
        {Terra::AESCrypt::Engine::DecryptResult::AlreadyDecrypting, 108},
        {Terra::AESCrypt::Engine::DecryptResult::DecryptionCancelled, 109},
        {Terra::AESCrypt::Engine::DecryptResult::InternalError, 110},
};

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

//use destructor to run function when object leaves scope
struct Deferer {
private:
    std::function<void()> deferred;
public:
    Deferer(std::function<void()> deferred): deferred(deferred){};
    ~Deferer() {
        deferred();
    };
};
Terra::AESCrypt::Engine::Encryptor * encryptorPtr = NULL;
Terra::AESCrypt::Engine::Decryptor * decryptorPtr = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_dewdrop623_androidcrypt_JNIInterface_cancel(JNIEnv *env, jclass jclass1) {
    if (encryptorPtr != NULL) {
        encryptorPtr -> Cancel();
    }
    if (decryptorPtr != NULL) {
        decryptorPtr -> Cancel();
    }
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_dewdrop623_androidcrypt_JNIInterface_encrypt(JNIEnv *env, jclass jclass1, jstring jpassword, jobject inputStream, jobject outputStream, jobject callbackInterface, jobject logStream) {
    JNIOstream jniLogStream{env, logStream, 100};

    //get progress aesCryptProgressCallback method
    jclass callbackInterfaceClass = env->GetObjectClass(callbackInterface);
    jmethodID progressCallbackMethodId = env->GetMethodID(callbackInterfaceClass, "progressCallback", "(I)V");
    jmethodID completedCallbackMethodId = env->GetMethodID(callbackInterfaceClass, "completedCallback", "(I)V");
    auto aesCryptProgressCallback = [&](const std::string & instance, std::size_t totalBytes) {
        env->CallVoidMethod(callbackInterface, progressCallbackMethodId, (jint)totalBytes);
    };

    //convert password to u8string
    const char * passwordcstr = env->GetStringUTFChars(jpassword, NULL);
    std::u8string password(reinterpret_cast<const char8_t *>(passwordcstr));


    //TODO define these somewhere else?
    int bufferSize = 1049000;
    int iterations = 300000;

    JNIIstream jniIstream{env, inputStream, bufferSize};
    JNIOstream jniOstream{env, outputStream, bufferSize};

    Terra::AESCrypt::Engine::EncryptResult encrypt_result{};

    Terra::Logger::LoggerPointer logger = std::make_shared<Terra::Logger::Logger>(jniLogStream);

    Terra::AESCrypt::Engine::Encryptor encryptor{logger};

    //keep a global reference to encryptor to call cancel function
    encryptorPtr = &encryptor;
    //use struct destructor to cleanup reference
    Deferer deferer([&]() -> void {
        encryptorPtr = NULL;
    });

    std::vector<std::pair<std::string, std::string>> extensions = {};
    encrypt_result = encryptor.Encrypt(password, iterations, jniIstream, jniOstream, extensions, aesCryptProgressCallback, bufferSize / 10);

    // *** IMPORTANT NOTE: FINAL FLUSH IS REQUIRED TO WRITE LAST DATA ***
    jniOstream.flush();

    int completion_status = 0;
    if (ENCRYPT_RESULT_CODES.contains(encrypt_result)) {
        completion_status = ENCRYPT_RESULT_CODES.at(encrypt_result);
    }
    env->CallVoidMethod(callbackInterface, completedCallbackMethodId, (jint)completion_status);
    return (jboolean)(encrypt_result==Terra::AESCrypt::Engine::EncryptResult::Success);
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_dewdrop623_androidcrypt_JNIInterface_decrypt(JNIEnv *env, jclass jclass1, jstring jpassword, jobject inputStream, jobject outputStream, jobject callbackInterface, jobject logStream) {
    JNIOstream jniLogStream{env, logStream, 100};

    jclass callbackInterfaceClass = env->GetObjectClass(callbackInterface);
    jmethodID progressCallbackMethodId = env->GetMethodID(callbackInterfaceClass, "progressCallback", "(I)V");
    jmethodID completedCallbackMethodId = env->GetMethodID(callbackInterfaceClass, "completedCallback", "(I)V");
    auto aesCryptProgressCallback = [&](const std::string & instance, std::size_t bytesRead) {
        env->CallVoidMethod(callbackInterface, progressCallbackMethodId, (jint)bytesRead);
    };

    const char * passwordcstr = env->GetStringUTFChars(jpassword, NULL);
    std::u8string password(reinterpret_cast<const char8_t *>(passwordcstr));


    //TODO define these somewhere else?
    int bufferSize = 1049000;
    int iterations = 300000;

    JNIIstream jniIstream{env, inputStream, bufferSize};
    JNIOstream jniOstream{env, outputStream, bufferSize};

    Terra::AESCrypt::Engine::DecryptResult decrypt_result{};

    Terra::Logger::LoggerPointer logger = std::make_shared<Terra::Logger::Logger>(jniLogStream);

    Terra::AESCrypt::Engine::Decryptor decryptor{logger};

    //keep a global reference to encryptor to call cancel function
    decryptorPtr = &decryptor;
    //use struct destructor to cleanup reference
    Deferer deferer([&]() -> void {
        decryptorPtr = NULL;
    });

    std::vector<std::pair<std::string, std::string>> extensions = {};
    decrypt_result = decryptor.Decrypt(password, jniIstream, jniOstream, aesCryptProgressCallback, bufferSize / 10);

    // *** IMPORTANT NOTE: FINAL FLUSH IS REQUIRED TO WRITE LAST DATA ***
    jniOstream.flush();

    int completion_status = 0;
    if (DECRYPT_RESULT_CODES.contains(decrypt_result)) {
        completion_status = DECRYPT_RESULT_CODES.at(decrypt_result);
    }
    env->CallVoidMethod(callbackInterface, completedCallbackMethodId, (jint)completion_status);
    return (jboolean)(decrypt_result==Terra::AESCrypt::Engine::DecryptResult::Success);
}
