package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;

import java.io.File;

/**
 * deletes files
 */

public class FileDeleteOperator extends FileOperator {
    private boolean done=false;
    public FileDeleteOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    public int getProgress() {
        if (done) {
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
    public void doOperation() {
        file.delete();//TODO there needs to be more here. needs logic for nonempty directories and for symbolic links
        done=true;
    }

    @Override
    protected void prepareAndValidate() {

    }

    @Override
    protected void getInfoFromUser() {
        finishTakingInput();
    }
}
