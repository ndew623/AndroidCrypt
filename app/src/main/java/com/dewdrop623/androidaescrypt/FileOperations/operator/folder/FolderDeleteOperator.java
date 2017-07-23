package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;
import java.util.HashMap;

/**
 * recursively deletes folder contents
 */

public class FolderDeleteOperator extends FileOperator{
    boolean done = false;
    public FolderDeleteOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    public int getProgress() {
        if (done) {//TODO real progress updates
            return 100;
        }
        return 0;
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
        deleteFolder(file);
        done=true;
    }
    private void deleteFolder(File file) {
        File[] subfiles = file.listFiles();
        for (File subfile : subfiles) {
            if (subfile.isDirectory()) {
                deleteFolder(subfile);
            } else {
                subfile.delete();
            }
        }
        file.delete();
    }

    @Override
    protected void prepareAndValidate() {

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
