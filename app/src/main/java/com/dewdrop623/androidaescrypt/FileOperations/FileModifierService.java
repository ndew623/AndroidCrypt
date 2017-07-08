package com.dewdrop623.androidaescrypt.FileOperations;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;

import com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptDecryptFileOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptEncryptFileOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.CreateFolderOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileDeleteOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileRenameOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.FolderCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.FolderDeleteOperator;

import java.io.File;

public class FileModifierService extends Service {

    public static final String FILEMODIFIERSERVICE_ARGS = "com.dewdrop623.androidaescrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_ARGS";
    public static final String FILEMODIFIERSERVICE_FILE = "com.dewdrop623.androidaescrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_FILE";
    public static final String FILEMODIFIERSERVICE_OPERATIONTYPE = "com.dewdrop623.androidaescrypt.FileOperations.FileModifierService.FILEMODIFIERSERVICE_OPERATIONTYPE";

    private int fileOperationType = -1;
    private Bundle args;
    private File file;

    private Thread fileOperationThread;

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        args = intent.getBundleExtra(FILEMODIFIERSERVICE_ARGS);
        file = new File(args.getString(FILEMODIFIERSERVICE_FILE));
        fileOperationType = args.getInt(FILEMODIFIERSERVICE_OPERATIONTYPE);
        Runnable operator;
        if (file.isDirectory()) {
            operator = getFolderOperator();
        } else {
            operator = getFileOperator();
        }
        fileOperationThread = new Thread(operator);
        fileOperationThread.start();
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }
    private Runnable getFileOperator() {
        Runnable operator;
        switch (fileOperationType) {
            case FileOperationType.DELETE:
                operator = new FileDeleteOperator(file, args, this);
                break;
            case FileOperationType.ENCRYPT:
                operator = new AESCryptEncryptFileOperator(file, args, this);
                break;
            case FileOperationType.DECRYPT:
                operator = new AESCryptDecryptFileOperator(file, args, this);
                break;
            case FileOperationType.MOVE:
                operator = new FileMoveOperator(file, args, this);
                break;
            case FileOperationType.COPY:
                operator = new FileCopyOperator(file, args, this);
                break;
            case FileOperationType.RENAME:
                operator = new FileRenameOperator(file, args, this);
                break;
            default:
                operator = null;
                break;
        }
        return operator;
    }
    private Runnable getFolderOperator() {
        Runnable operator;
        switch (fileOperationType) {
            case FileOperationType.CREATE_FOLDER:
                operator = new CreateFolderOperator(file, args, this);
                break;
            case FileOperationType.DELETE:
                operator = new FolderDeleteOperator(file, args, this);
                break;
            case FileOperationType.MOVE:
                operator = new FileMoveOperator(file, args, this);
                break;
            case FileOperationType.COPY:
                operator = new FolderCopyOperator(file, args, this);
                break;
            case FileOperationType.RENAME:
                operator = new FileRenameOperator(file, args, this);
                break;
            default:
                operator = null;
                break;
        }
        return operator;
    }
}
