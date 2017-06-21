package com.dewdrop623.androidaescrypt.FileOperations;

import android.os.AsyncTask;

import com.dewdrop623.androidaescrypt.FileOperations.operator.CreateFolderOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileDeleteOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileEncryptOperator;

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
        switch (fileCommand.fileOperationType) {
            case CREATE_FOLDER:
                new CreateFolderOperator(fileCommand.file, null).execute();
                break;
            case DELETE:
                new FileDeleteOperator(fileCommand.file, null).execute();
                break;
            case ENCRYPT:
                new FileEncryptOperator(fileCommand.file, fileCommand.arg).execute();
                break;
        }
        return null;
    }
}
