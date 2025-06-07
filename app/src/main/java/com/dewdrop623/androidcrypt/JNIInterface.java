package com.dewdrop623.androidcrypt;

import java.io.InputStream;
import java.io.OutputStream;

public class JNIInterface {
    native public static void encrypt(String password, InputStream inputSteam, OutputStream outputStream, JNIProgressCallback progressCallback, OutputStream logStream);
    native public static void decrypt(String password, InputStream inputSteam, OutputStream outputStream, JNIProgressCallback progressCallback, OutputStream logStream);
}
