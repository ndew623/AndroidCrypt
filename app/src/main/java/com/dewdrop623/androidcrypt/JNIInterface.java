package com.dewdrop623.androidcrypt;

import java.io.InputStream;
import java.io.OutputStream;

public class JNIInterface {
    native public static boolean encrypt(String password, InputStream inputSteam, OutputStream outputStream, JNICallbackInterface progressCallback, OutputStream logStream);
    native public static boolean decrypt(String password, InputStream inputSteam, OutputStream outputStream, JNICallbackInterface progressCallback, OutputStream logStream);
    native public static void cancel();
}
