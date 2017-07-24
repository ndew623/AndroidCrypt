package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * creates folders
 */

public class CreateFolderOperator extends FileOperator {
    boolean done = false;
    public static final String CREATE_FOLDER_OPERATOR_FOLDER_NAME = "com.dewdrop623.androidaescrypt.FileOperations.operator.folder.CreateFolderOperator.CREATE_FOLDER_OPERATOR_FOLDER_NAME";

    private File outputFile;
    boolean validFilename = true;

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
        outputFile.mkdir();
        done = true;
    }
    @Override
    protected void initMemVarFromArgs() {
        outputFile = new File(file.getAbsolutePath()+"/"+args.getString(CREATE_FOLDER_OPERATOR_FOLDER_NAME));
    }

    @Override
    protected void getInfoFromUser() {
        if (validFilename) {
            finishTakingInput();
        } else {
            askForTextOrCancel(fileModifierService.getString(R.string.invalid_filename)+". "+fileModifierService.getString(R.string.try_again)+"?");
        }
    }
    @Override
    protected void prepareAndValidate() {
        if(!file.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+file.getName());
            cancelOperation();
            return;
        }
        if(file.canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_writable)+": "+file.getName());
            cancelOperation();
            return;
        }
        validFilename = validFilename(args.getString(CREATE_FOLDER_OPERATOR_FOLDER_NAME));
    }
    @Override
    protected void handleYesNoResponse(boolean yes) {

    }

    @Override
    protected void handleYesNoRememberAnswerResponse(boolean yes, boolean remember) {

    }

    @Override
    protected void handleTextOrCancelResponse(String response) {
        if (response==null) {
            cancelOperation();
            return;
        }
        if (validFilename(response)) {
            outputFile = new File(file.getAbsolutePath()+"/"+response);
            finishTakingInput();
        } else {
            askForTextOrCancel(fileModifierService.getString(R.string.invalid_filename)+". "+fileModifierService.getString(R.string.try_again)+"?");
        }
    }
}
