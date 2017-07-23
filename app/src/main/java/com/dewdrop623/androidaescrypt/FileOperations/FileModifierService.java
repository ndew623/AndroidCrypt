package com.dewdrop623.androidaescrypt.FileOperations;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;

import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.questiondialog.QuestionDialog;
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
    }
    public void testDialog() {
        Intent intent = new Intent(this, YesNoQuestionDialog.class);
        intent.putExtra(QuestionDialog.QUESTION_ARG, "Overwrite fake.file?");
        startActivity(intent);
    }

}
