package com.dewdrop623.androidcrypt;

import android.app.Notification;
import android.app.Service;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.support.annotation.IntDef;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;

/**
 * Crypto service runs a background thread that does the encryption and decryption operations.
 */

public class CryptoService extends Service {

    public static final int START_FOREGROUND_ID = 1025;

    //Keys for the intent extras
    public static final String INPUT_URI_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.INPUT_URI_KEY";
    public static final String OUTPUT_URI_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.OUTPUT_URI_EXTRA_KEY";
    public static final String PASSWORD_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.PASSWORD_EXTRA_KEY";
    public static final String VERSION_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.VERSION_EXTRA_KEY";
    public static final String OPERATION_TYPE_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.OPERATION_TYPE_EXTRA_KEY";

    @Override
    public void onCreate() {
        super.onCreate();
        startForeground(START_FOREGROUND_ID, buildNotification());
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Uri inputUri = Uri.parse(intent.getStringExtra(INPUT_URI_EXTRA_KEY));
        Uri outputUri = Uri.parse(intent.getStringExtra(OUTPUT_URI_EXTRA_KEY));
        String password = intent.getStringExtra(PASSWORD_EXTRA_KEY);
        int version = intent.getIntExtra(VERSION_EXTRA_KEY, CryptoThread.VERSION_2);
        boolean operationType = intent.getBooleanExtra(OPERATION_TYPE_EXTRA_KEY, CryptoThread.OPERATION_TYPE_DECRYPTION);

        CryptoThread cryptoThread = new CryptoThread(this, inputUri, outputUri, password, version, operationType);
        cryptoThread.start();

        return START_STICKY;
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    //TODO, this notification should change appearance for encryption and decryption
    //TODO this notification should display the current progress
    private Notification buildNotification() {
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this);
        Intent resultIntent = new Intent(this, MainActivity.class);

        builder.setSmallIcon(R.drawable.ic_lock);
        builder.setContentTitle(getString(R.string.app_name)).setContentText("placeholder notification");
        return  builder.build();
    }
}
