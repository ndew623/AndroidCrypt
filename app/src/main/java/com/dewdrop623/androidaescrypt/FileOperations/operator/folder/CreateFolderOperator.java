package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.database.sqlite.SQLiteDoneException;
import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;

/**
 * creates folders
 */

public class CreateFolderOperator extends FileOperator {
    boolean done = false;
    public static final String CREATE_FOLDER_OPERATOR_FOLDER_NAME = "com.dewdrop623.androidaescrypt.FileOperations.operator.folder.CreateFolderOperator.CREATE_FOLDER_OPERATOR_FOLDER_NAME";

    public CreateFolderOperator(File file, Bundle args, FileModifierService fileModifierService) {
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
    public void doOperation() {
        new File(file.getAbsolutePath()+"/"+args.getString(CREATE_FOLDER_OPERATOR_FOLDER_NAME)).mkdir();
        done = true;
    }
    @Override
    protected void initMemVarFromArgs() {

    }

    @Override
    protected void getInfoFromUser() {
        finishTakingInput();
    }
    @Override
    protected void prepareAndValidate() {

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
}
