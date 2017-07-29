package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * recursively deletes folder contents
 */

public class FolderDeleteOperator extends FileOperator{
    private int filesToBeDeleted = 0;
    private int filesDeleted = 0;

    public FolderDeleteOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    protected void initMemVarFromArgs() {

    }

    @Override
    protected void handleYesNoResponse(boolean yes) {

    }

    @Override
    protected void handleYesNoRememberAnswerResponse(boolean yes, boolean remember) {

    }

    @Override
    protected void handleTextOrCancelResponse(String response) {

    }

    @Override
    protected void doOperation() {
        filesToBeDeleted = FileUtils.countFilesInFolder(file);
        fileModifierService.updateNotification(0);
        deleteFolder(file);
        fileModifierService.updateNotification(100);
    }
    private void deleteFolder(File file) {
        File[] subfiles = file.listFiles();
        for (File subfile : subfiles) {
            if (subfile.isDirectory()) {
                deleteFolder(subfile);
            } else {
                subfile.delete();
                filesDeleted++;
            }
            FileUtils.notificationUpdate(filesDeleted, filesToBeDeleted, fileModifierService);
        }
        file.delete();
    }

    @Override
    protected void prepareAndValidate() {
        if (!file.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+file.getName());
            cancelOperation();
            return;
        }
        if (!file.canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_writable)+": "+file.getName());
            cancelOperation();
            return;
        }
    }

    @Override
    protected void getInfoFromUser() {
        finishTakingInput();
    }
    public void doOperationWithoutThreadOrUserQuestions() {
        initMemVarFromArgs();
        doOperation();
    }
}
