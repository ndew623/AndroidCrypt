package com.dewdrop623.androidcrypt;

public abstract class JNICallbackInterface {
    public abstract void progressCallback(int totalBytes);
    public abstract void completedCallback(int status);
}
