package com.dewdrop623.androidcrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidcrypt.R;

import java.io.File;

/**
 * deletes files
 */

public class FileDeleteOperator extends FileOperator {

    public FileDeleteOperator(File file, Bundle args, FileModifierService fileModifierService) {
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
    public void doOperation() {
        file.delete();
        if (!silent) {
            fileModifierService.updateNotification(100);
        }
    }

    @Override
    protected boolean prepareAndValidate() {
        if (!file.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.file_does_not_exist)+": "+file.getName());
            return false;
        }
        if (!file.canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.file_not_writable)+": "+file.getName());
            return false;
        }
        return true;
    }

    @Override
    protected void getInfoFromUser() {
        finishTakingInput();
    }

    @Override
    public void runSilentNoThread() {
        initMemVarFromArgs();
        prepareAndValidate();
        doOperation();
    }
}
