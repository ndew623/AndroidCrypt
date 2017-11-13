package com.dewdrop623.androidcrypt;

import android.app.Notification;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Handler;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.NotificationManagerCompat;
import android.widget.Toast;

/**
 * Crypto service runs a background thread that does the encryption and decryption operations.
 */

public class CryptoService extends Service implements CryptoThread.ProgressDisplayer {

    public static final int START_FOREGROUND_ID = 1025;

    //Keys for the intent extras
    public static final String INPUT_URI_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.INPUT_URI_KEY";
    public static final String OUTPUT_URI_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.OUTPUT_URI_EXTRA_KEY";
    public static final String VERSION_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.VERSION_EXTRA_KEY";
    public static final String OPERATION_TYPE_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.OPERATION_TYPE_EXTRA_KEY";

    private static final String PROGRESS_DISPLAYER_ID = "com.dewdrop623.androidcrypt.CryptoService.PROGRESS_DISPLAYER_ID";

    @Override
    public void onCreate() {
        super.onCreate();
        startForeground(START_FOREGROUND_ID, buildProgressNotification(CryptoThread.OPERATION_TYPE_ENCRYPTION,-1));
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Uri inputUri = Uri.parse(intent.getStringExtra(INPUT_URI_EXTRA_KEY));
        Uri outputUri = Uri.parse(intent.getStringExtra(OUTPUT_URI_EXTRA_KEY));
        int version = intent.getIntExtra(VERSION_EXTRA_KEY, SettingsHelper.AESCRYPT_DEFAULT_VERSION);
        String password = MainActivityFragment.getAndClearPassword();
        boolean operationType = intent.getBooleanExtra(OPERATION_TYPE_EXTRA_KEY, CryptoThread.OPERATION_TYPE_DECRYPTION);

        CryptoThread.registerForProgressUpdate(PROGRESS_DISPLAYER_ID, this);

        if (password != null) {
            CryptoThread cryptoThread = new CryptoThread(this, inputUri, outputUri, password, version, operationType);
            cryptoThread.start();
        } else {
            showToastOnGuiThread(R.string.error_null_password);
            stopSelf();
        }

        return START_STICKY;
    }

    /**
     * Called by CryptoThread to report errors
     */
    public void showToastOnGuiThread(final int msg) {
        showToastOnGuiThread(getString(msg));
    }

    /**
     * Called by CryptoThread to report errors
     */
    public void showToastOnGuiThread(final String msg) {
        final Context context = this;
        new Handler(getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(context, msg, Toast.LENGTH_LONG).show();
            }
        });
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    /*
    * Create the notification that is displayed while the operation is ongoing.
    * if progress < 0: displayed without progress bar
     */
    private Notification buildProgressNotification(boolean operationType, int progress) {
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this);
        Intent resultIntent = new Intent(this, MainActivity.class);

        builder.setSmallIcon(operationType == CryptoThread.OPERATION_TYPE_ENCRYPTION?R.drawable.ic_lock:R.drawable.ic_unlock);

        if (progress < 0) {
            builder.setContentTitle(getString(R.string.app_name));
            builder.setContentText(getString(R.string.operation_in_progress));
        } else {
            builder.setContentTitle(operationType == CryptoThread.OPERATION_TYPE_ENCRYPTION?getString(R.string.encrypting):getString(R.string.decrypting));
            builder.setProgress(100, progress, false);
        }
        return builder.build();
    }

    //Implementation of CryptoThread.ProgressDisplayers interface. Called by CryptoThread to update the progress.
    //progress is out of 100.
    @Override
    public void update(boolean operationType, int progress) {
        NotificationManagerCompat notificationManager = (NotificationManagerCompat) NotificationManagerCompat.from(this);
        notificationManager.notify(START_FOREGROUND_ID, buildProgressNotification(operationType, progress));
    }
}
