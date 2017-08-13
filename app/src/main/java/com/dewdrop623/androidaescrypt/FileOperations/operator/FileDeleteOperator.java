package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * deletes files
 */

public class FileDeleteOperator extends FileOperator {

    public FileDeleteOperator(File file, Bundle args, FileModifierService fileModifierService) {
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
    public void doOperation() {
        file.delete();
        fileModifierService.updateNotification(100);
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
    public void doOperationWithoutThreadOrUserQuestions() {
        initMemVarFromArgs();
        prepareAndValidate();
        doOperation();
    }
}
