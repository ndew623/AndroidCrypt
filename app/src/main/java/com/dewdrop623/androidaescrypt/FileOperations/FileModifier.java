package com.dewdrop623.androidaescrypt.FileOperations;

import android.os.AsyncTask;

import com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptDecryptFileOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.CreateFolderOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileDeleteOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptEncryptFileOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileRenameOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.FolderDeleteOperator;

/**
 * takes a file command and executes it in the background
 */

public class FileModifier extends AsyncTask{
    private FileCommand fileCommand;
    public FileModifier(FileCommand fileCommand) {
        super();
        this.fileCommand = fileCommand;
    }
    @Override
    protected Object doInBackground(Object[] params) {
        if (fileCommand.file.isDirectory()) {
            folderOperation();
        } else {
            fileOperation();
        }
        return null;
    }
    private void fileOperation() {
        switch (fileCommand.fileOperationType) {
            case CREATE_FOLDER:
                new CreateFolderOperator(fileCommand.file, fileCommand.args).execute();
                break;
            case DELETE:
                new FileDeleteOperator(fileCommand.file, fileCommand.args).execute();
                break;
            case ENCRYPT:
                new AESCryptEncryptFileOperator(fileCommand.file, fileCommand.args).execute();
                break;
            case DECRYPT:
                new AESCryptDecryptFileOperator(fileCommand.file, fileCommand.args).execute();
                break;
            case MOVE:
                new FileMoveOperator(fileCommand.file, fileCommand.args).execute();
                break;
            case COPY:
                new FileCopyOperator(fileCommand.file, fileCommand.args).execute();
                break;
            case RENAME:
                new FileRenameOperator(fileCommand.file, fileCommand.args).execute();
                break;
        }
    }
    private void folderOperation() {
        switch (fileCommand.fileOperationType) {
            case DELETE:
                new FolderDeleteOperator(fileCommand.file, fileCommand.args).execute();
                break;
            case RENAME:
                new FileRenameOperator(fileCommand.file, fileCommand.args).execute();
                break;
        }
    }
}
