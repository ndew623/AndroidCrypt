package com.dewdrop623.androidcrypt;


import android.util.Log;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;

public class LogStream extends OutputStream {

    private String tag = "LogStream";

    public LogStream() {
        super();
    }

    public LogStream(String tag) {
        super();
        this.tag = tag;
    }
    private StringBuilder stringBuilder = new StringBuilder();

    @Override
    public void write(byte[] bytes) throws IOException {
        String str = new String(bytes, StandardCharsets.UTF_8);
        stringBuilder.append(str);
    }
    @Override
    public void write(int b) throws IOException {
        // Convert the byte to a character and append to the StringBuilder
        stringBuilder.append((char) b);
    }

    @Override
    public void flush() throws IOException {
        // Print the accumulated string
        String messageToPrint = stringBuilder.toString();
        Log.d(tag, messageToPrint);
        // Clear the StringBuilder for the next write
        stringBuilder.setLength(0);
    }

    @Override
    public void close() throws IOException {
        // Optionally, you can flush the remaining content when closing
        flush();
    }
}