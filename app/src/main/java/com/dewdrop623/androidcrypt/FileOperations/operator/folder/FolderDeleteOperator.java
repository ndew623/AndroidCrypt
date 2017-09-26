package com.dewdrop623.androidcrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidcrypt.FileOperations.FileUtils;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidcrypt.R;

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
    public String getOperationName() {
        return fileModifierService.getString(R.string.deleting)+" "+file.getName();
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
        if (!silent) {
            fileModifierService.updateNotification(0);
        }
        deleteFolder(file);
        if (!silent) {
            fileModifierService.updateNotification(100);
        }
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
    protected boolean prepareAndValidate() {
        if (!file.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+file.getName());
            return false;
        }
        if (!file.canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_writable)+": "+file.getName());
            return false;
        }
        return true;
    }

    @Override
    protected void getInfoFromUser() {
        finishTakingInput();
    }
    public void runSilentNoThread() {
        initMemVarFromArgs();
        prepareAndValidate();
        doOperation();
    }
}
