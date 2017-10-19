package com.dewdrop623.androidcrypt;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;

/**
 * Crypto service runs a background thread that does the encryption and decryption operations.
 */

public class CryptoService extends Service {
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
