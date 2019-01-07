package com.dewdrop623.androidcrypt.FileOperations;

import android.annotation.TargetApi;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.NotificationManagerCompat;
import android.util.Log;
import android.widget.Toast;

import com.dewdrop623.androidcrypt.CryptoThread;
import com.dewdrop623.androidcrypt.MainActivity;
import com.dewdrop623.androidcrypt.MainActivityFragment;
import com.dewdrop623.androidcrypt.R;
import com.dewdrop623.androidcrypt.SettingsHelper;

import java.io.File;

public class FileOperationService extends Service implements FileOperationThread.ProgressDisplayer {
    public static final int START_FOREGROUND_ID = 1025;

    //Keys for the intent extras
    public static final String INPUT_FILE_NAME_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.INPUT_URI_KEY";
    public static final String OUTPUT_FILE_NAME_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.OUTPUT_FILE_NAME_EXTRA_KEY";
    public static final String INPUT_FILENAME_KEY = "com.dewdrop623.androidcrypt.CryptoService.INPUT_FILENAME_KEY";
    public static final String OUTPUT_FILENAME_KEY = "com.dewdrop623.androidcrypt.CryptoService.OUTPUT_FILENAME_KEY";
    public static final String VERSION_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.VERSION_EXTRA_KEY";
    public static final String OPERATION_TYPE_EXTRA_KEY = "com.dewdrop623.androidcrypt.CryptoService.OPERATION_TYPE_EXTRA_KEY";
    public static final String DELETE_INPUT_FILE_KEY = "com.dewdrop623.androidcrypt.CryptoService.DELETE_INPUT_FILE_KEY";

    public static final String NOTIFICATION_CHANNEL_ID = "com.dewdrop623.androidcrypt.CryptoService.OPERATION_TYPE_EXTRA_KEY";

    private static final String PROGRESS_DISPLAYER_ID = "com.dewdrop623.androidcrypt.CryptoService.PROGRESS_DISPLAYER_ID";

    @Override
    public void onCreate() {
        super.onCreate();
        startForeground(START_FOREGROUND_ID, buildProgressNotification(FileOperationThread.FILE_OP_TYPE.ENCRYPTION_OP, -1, R.string.app_name, -1, -1));
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        /*
         * If the user closes the activity (swiping out of recents) immediately after an operation is started, this method will get called a second time, but intent == null.
         * Don't know why. Don't know how it doesn't trigger breakpoints, but intent==null means a NullPointerException. So this if condition handles that edge case.
         * You might think that CryptoThread will still be running after it was started the first time this method was called.
         * You'd be wrong. It starts, but stops when this method is called again, regardless of this if condition.
         * It is probably a bug in Android since doing this to other applications that use services causes the same problem for them.
         * */
        if (intent == null) {
            //if stopForeground isn't called here, a sticky notification appears that cannot be closed (there is no operation in progress) without force quitting the app.
            stopForeground(true);
            return START_NOT_STICKY;
        }
        String inputFileName = intent.getStringExtra(INPUT_FILE_NAME_EXTRA_KEY);
        String outputFileName = intent.getStringExtra(OUTPUT_FILE_NAME_EXTRA_KEY);

        File inputFile = new File(inputFileName);
        File outputFile = new File(outputFileName);

        int version = intent.getIntExtra(VERSION_EXTRA_KEY, SettingsHelper.AESCRYPT_DEFAULT_VERSION);
        String password = MainActivityFragment.getAndClearPassword();
        FileOperationThread.FILE_OP_TYPE operationType = FileOperationThread.FILE_OP_TYPE.values()
                [intent.getIntExtra(OPERATION_TYPE_EXTRA_KEY, FileOperationThread.FILE_OP_TYPE.ENCRYPTION_OP.ordinal())];
        boolean deleteSource = intent.getBooleanExtra(DELETE_INPUT_FILE_KEY, false);

        FileOperationThread.registerForProgressUpdate(PROGRESS_DISPLAYER_ID, this);

        FileOperationThread fileOperationThread = null;
        switch (operationType) {
            case ENCRYPTION_OP:
                if (password != null) {
                    fileOperationThread = new EncryptionOperationThread(this, inputFile, outputFile, password, version, deleteSource);
                } else {
                    showToastOnGuiThread(R.string.error_null_password);
                    stopSelf();
                }
                break;
            case DECRYPTION_OP:
                if (password != null) {
                    fileOperationThread = new DecryptionOperationThread(this, inputFile, outputFile, password, deleteSource);
                } else {
                    showToastOnGuiThread(R.string.error_null_password);
                    stopSelf();
                }
                break;
            case TAR_OP:
                //TODO have any sort of implementation of FileOperationThread to do something here
                break;
        }
        if (fileOperationThread != null) {
            fileOperationThread.start();
        } else {
            Log.e("AndroidCrypt", "Null file operation thread.");
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
        // TO(never)DO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    private Notification buildProgressNotification(FileOperationThread.FILE_OP_TYPE operationType, int progress, int completedMessageStringId, int minutesToCompletion, int secondsToCompletion) {

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            createNotificationChannel();
        }

        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, NOTIFICATION_CHANNEL_ID);

        Intent resultIntent = new Intent(this, MainActivity.class);
        PendingIntent resultPendingIntent = PendingIntent.getActivity(this, 0, resultIntent, 0);

        /*TODO, conditional needs three cases when tar operation is ready*/
        builder.setSmallIcon(operationType == FileOperationThread.FILE_OP_TYPE.ENCRYPTION_OP ? R.drawable.ic_lock_png : R.drawable.ic_unlock_png);
        builder.setContentIntent(resultPendingIntent);

        if (progress < 0) {
            builder.setContentTitle(getString(R.string.app_name));
            builder.setContentText(getString(R.string.operation_in_progress));
        } else if (progress < 100) {
            /*TODO conditional needs three cases when tar opertaion*/
            String title = operationType == FileOperationThread.FILE_OP_TYPE.ENCRYPTION_OP ? getString(R.string.encrypting) : getString(R.string.decrypting);
            if (minutesToCompletion != -1) {
                title = title.concat(" " + minutesToCompletion + "m");
            }
            if (secondsToCompletion != -1) {
                title = title.concat(" " + secondsToCompletion + "s");
            }
            builder.setContentTitle(title);
            builder.setProgress(100, progress, false);
        } else {
            builder.setContentTitle(getString(R.string.app_name));
            builder.setContentText(getString(completedMessageStringId));
        }
        return builder.build();
    }

    @TargetApi(Build.VERSION_CODES.O)
    private void createNotificationChannel() {
        CharSequence channelName = getString(R.string.app_name);
        NotificationChannel notificationChannel = new NotificationChannel(NOTIFICATION_CHANNEL_ID, channelName, NotificationManager.IMPORTANCE_LOW);
        notificationChannel.enableLights(false);
        notificationChannel.enableVibration(false);
        ((NotificationManager) getSystemService(NOTIFICATION_SERVICE)).createNotificationChannel(notificationChannel);
    }

    //Implementation of CryptoThread.ProgressDisplayers interface. Called by CryptoThread to update the progress.
    //progress is out of 100.
    public void update(FileOperationThread.FILE_OP_TYPE operationType, int progress, int completedMessageStringId, int minutesToCompletion, int secondsToCompletion) {
        NotificationManagerCompat notificationManager = (NotificationManagerCompat) NotificationManagerCompat.from(this);
        notificationManager.notify(START_FOREGROUND_ID, buildProgressNotification(operationType, progress, completedMessageStringId, minutesToCompletion, secondsToCompletion));
    }
}
