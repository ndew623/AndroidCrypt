package com.dewdrop623.androidcrypt.FileOperations;

import android.app.Notification;
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

import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.FileOperationDialog;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.questiondialog.QuestionDialog;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.questiondialog.TextOrCancelQuestionDialog;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.questiondialog.YesNoQuestionDialog;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.questiondialog.YesNoRememberAnswerQuestionDialog;
import com.dewdrop623.androidcrypt.FileOperations.operator.AESCryptDecryptFileOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.AESCryptEncryptFileOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileDeleteOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.folder.CreateFolderOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.folder.FolderCopyOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.folder.FolderDeleteOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.folder.FolderMoveOperator;
import com.dewdrop623.androidcrypt.R;

import java.io.File;
import java.util.HashMap;

public class FileModifierService extends Service {

    public static final String FILEMODIFIERSERVICE_ARGS = "com.dewdrop623.androidcrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_ARGS";
    public static final String FILEMODIFIERSERVICE_FILE = "com.dewdrop623.androidcrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_FILE";
    public static final String FILEMODIFIERSERVICE_OPERATIONTYPE = "com.dewdrop623.androidcrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_OPERATIONTYPE";
    public static final String OPERATION_ID_ARG = "com.dewdrop623.androidcrypt.FileOperations.FileModifierService.OPERATION_ID_ARG";
    public static final String OPERATION_NAME_ARG = "com.dewdrop623.androidcrypt.FileOperations.FileModifierService.OPERATION_NAME_ARG";
    public static int nextOperationId = 1;
    private static HashMap<Integer, FileOperator> currentFileOperators = new HashMap<>();
    private int fileOperationType = -1;
    private Bundle args;
    private File file;
    private int operationId;

    @Override
    public void onCreate() {
        super.onCreate();
        operationId = nextOperationId;
        nextOperationId++;
        //do in foreground so Android doesn't stop this service
        startForeground(operationId, buildNotification(false, 0));
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        args = intent.getBundleExtra(FILEMODIFIERSERVICE_ARGS);
        file = new File(args.getString(FILEMODIFIERSERVICE_FILE,""));
        fileOperationType = args.getInt(FILEMODIFIERSERVICE_OPERATIONTYPE);
        FileOperator fileOperator;
        if (file.isDirectory()) {
            fileOperator = folderOperation();
        } else {
            fileOperator = fileOperation();
        }
        currentFileOperators.put(operationId, fileOperator);
        fileOperator.run();
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }
    //cancels the operator with the given id. return value is whether or not the operation still existed to be canceled
    public static boolean cancelOperator(int operationId) {
        boolean exists = currentFileOperators.containsKey(operationId);
        if (exists) {
            currentFileOperators.get(operationId).cancelOperation();
        }
        return exists;
    }
    private FileOperator fileOperation() {
        FileOperator result = null;
        switch (fileOperationType) {
            case FileOperationType.DELETE:
                result = new FileDeleteOperator(file, args, this);
                break;
            case FileOperationType.ENCRYPT:
                result = new AESCryptEncryptFileOperator(file, args, this);
                break;
            case FileOperationType.DECRYPT:
                result = new AESCryptDecryptFileOperator(file, args, this);
                break;
            case FileOperationType.MOVE:
                result = new FileMoveOperator(file, args, this);
                break;
            case FileOperationType.COPY:
                result = new FileCopyOperator(file, args, this);
                break;
            default:
                break;
        }
        return result;
    }
    private FileOperator folderOperation() {
        FileOperator result = null;
        switch (fileOperationType) {
            case FileOperationType.CREATE_FOLDER:
                result = new CreateFolderOperator(file, args, this);
                break;
            case FileOperationType.DELETE:
                result = new FolderDeleteOperator(file, args, this);
                break;
            case FileOperationType.MOVE:
                result = new FolderMoveOperator(file, args, this);
                break;
            case FileOperationType.COPY:
                result = new FolderCopyOperator(file, args, this);
                break;
            default:
                break;
        }
        return result;
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
        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(operationId, buildNotification(true, progress));
    }
    private Notification buildNotification(boolean hasProgress, int progress) {

        NotificationCompat.Builder builder = new NotificationCompat.Builder(this);

        Intent resultIntent = new Intent(this, FileOperationDialog.class);

        resultIntent.putExtra(FileModifierService.OPERATION_ID_ARG, operationId);
        if (currentFileOperators.containsKey(operationId)) {
            resultIntent.putExtra(FileModifierService.OPERATION_NAME_ARG, currentFileOperators.get(operationId).getOperationName());
        }

        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
        stackBuilder.addParentStack(FileOperationDialog.class);
        stackBuilder.addNextIntent(resultIntent);
        PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(operationId, PendingIntent.FLAG_UPDATE_CURRENT);
        builder.setContentIntent(resultPendingIntent);

        if (hasProgress) {
            builder.setSmallIcon(android.R.drawable.ic_menu_edit)
                    .setContentTitle(getString(R.string.progress));
            if (progress == 100) {
                builder.setContentText(getString(R.string.done));
            } else {
                builder.setProgress(100, progress, false);
            }
        } else {
            builder.setContentTitle(getString(R.string.app_name))
                    .setContentText(getString(R.string.operation_in_progress));
        }
        return builder.build();
    }

}
