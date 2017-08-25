package com.dewdrop623.androidaescrypt.FileOperations;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;
import android.widget.Toast;

import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog.QuestionDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog.TextOrCancelQuestionDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog.YesNoQuestionDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog.YesNoRememberAnswerQuestionDialog;
import com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptDecryptFileOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptEncryptFileOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileDeleteOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.CreateFolderOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.FolderCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.FolderDeleteOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.FolderMoveOperator;
import com.dewdrop623.androidaescrypt.MainActivity;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

public class FileModifierService extends Service {

    public static final String FILEMODIFIERSERVICE_ARGS = "com.dewdrop623.androidaescrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_ARGS";
    public static final String FILEMODIFIERSERVICE_FILE = "com.dewdrop623.androidaescrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_FILE";
    public static final String FILEMODIFIERSERVICE_OPERATIONTYPE = "com.dewdrop623.androidaescrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_OPERATIONTYPE";
    public static int nextOperationProgressId = 1;
    private int fileOperationType = -1;
    private Bundle args;
    private File file;
    private int operationProgressId;

    @Override
    public void onCreate() {
        super.onCreate();
        operationProgressId = nextOperationProgressId;
        nextOperationProgressId++;
        //do in foreground so Android doesn't stop this service
        Intent resultIntent = new Intent(this, MainActivity.class);
        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
        stackBuilder.addParentStack(MainActivity.class);
        stackBuilder.addNextIntent(resultIntent);
        PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(operationProgressId, PendingIntent.FLAG_UPDATE_CURRENT);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(this).setSmallIcon(android.R.drawable.ic_menu_info_details)
                .setContentTitle(getString(R.string.app_name))
                .setContentText(getString(R.string.operation_in_progress))
                .setContentIntent(resultPendingIntent);
        startForeground(operationProgressId, builder.build());
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        args = intent.getBundleExtra(FILEMODIFIERSERVICE_ARGS);
        file = new File(args.getString(FILEMODIFIERSERVICE_FILE));
        fileOperationType = args.getInt(FILEMODIFIERSERVICE_OPERATIONTYPE);
        if (file.isDirectory()) {
            folderOperation();
        } else {
            fileOperation();
        }
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }
    private void fileOperation() {
        switch (fileOperationType) {
            case FileOperationType.DELETE:
                new FileDeleteOperator(file, args, this).run();
                break;
            case FileOperationType.ENCRYPT:
                new AESCryptEncryptFileOperator(file, args, this).run();
                break;
            case FileOperationType.DECRYPT:
                new AESCryptDecryptFileOperator(file, args, this).run();
                break;
            case FileOperationType.MOVE:
                new FileMoveOperator(file, args, this).run();
                break;
            case FileOperationType.COPY:
                new FileCopyOperator(file, args, this).run();
                break;
            default:
                break;
        }
    }
    private void folderOperation() {
        switch (fileOperationType) {
            case FileOperationType.CREATE_FOLDER:
                new CreateFolderOperator(file, args, this).run();
                break;
            case FileOperationType.DELETE:
                new FolderDeleteOperator(file, args, this).run();
                break;
            case FileOperationType.MOVE:
                new FolderMoveOperator(file, args, this).run();
                break;
            case FileOperationType.COPY:
                new FolderCopyOperator(file, args, this).run();
                break;
            default:
                break;
        }
    }
    public void askYesNo(String question) {
        Intent intent = new Intent(this, YesNoQuestionDialog.class);
        intent.putExtra(QuestionDialog.QUESTION_ARG, question);
        startActivity(intent);
    }
    public void askYesNoRememberAnswer(String question, int numberOfEvents, String typeOfEventString) {
        Intent intent = new Intent(this, YesNoRememberAnswerQuestionDialog.class);
        intent.putExtra(QuestionDialog.QUESTION_ARG, question);
        intent.putExtra(YesNoRememberAnswerQuestionDialog.NUM_OF_EVENTS_ARG, numberOfEvents);
        intent.putExtra(YesNoRememberAnswerQuestionDialog.TYPE_OF_EVENT_ARG, typeOfEventString);
        startActivity(intent);
    }
    public void askForTextOrCancel(String question) {
        Intent intent = new Intent(this, TextOrCancelQuestionDialog.class);
        intent.putExtra(QuestionDialog.QUESTION_ARG, question);
        startActivity(intent);
    }
    public void showToast(final String message) {
        final Context context = this;
        new Handler(getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(context, message, Toast.LENGTH_LONG).show();
            }
        });
    }
    public void updateNotification(int progress) {
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this).setSmallIcon(android.R.drawable.ic_menu_edit)
                .setContentTitle(getString(R.string.progress));
        if (progress == 100) {
            builder.setContentText(getString(R.string.done));
        } else {
            builder.setProgress(100, progress, false);
        }
        Intent resultIntent = new Intent(this, MainActivity.class);
        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
        stackBuilder.addParentStack(MainActivity.class);
        stackBuilder.addNextIntent(resultIntent);
        PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(operationProgressId, PendingIntent.FLAG_UPDATE_CURRENT);
        builder.setContentIntent(resultPendingIntent);
        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(operationProgressId, builder.build());
    }
}
