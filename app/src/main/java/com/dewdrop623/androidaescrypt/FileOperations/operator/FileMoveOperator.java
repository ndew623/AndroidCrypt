package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * moves files (also used when renaming a file)
 */

public class FileMoveOperator extends FileOperator {
    public static final String FILE_MOVE_DESTINATION_ARG = "com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator.FILE_MOVE_DESTINATION_ARG";
    public static final String FILE_NEW_NAME_ARG = "com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator.FILE_NEW_NAME_ARG";

    private boolean conflict = false;
    private File outputFile;

    public FileMoveOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    protected void initMemVarFromArgs() {
        outputFile = new File(args.getString(FILE_MOVE_DESTINATION_ARG)+"/"+args.getString(FILE_NEW_NAME_ARG, file.getName()));
    }

    @Override
    protected void handleYesNoResponse(boolean yes) {
        if (yes) {
            finishTakingInput();
        } else {
            cancelOperation();
        }
    }

    @Override
    protected void handleYesNoRememberAnswerResponse(boolean yes, boolean remember) {

    }

    @Override
    protected void handleTextOrCancelResponse(String response) {

    }

    @Override
    protected void doOperation() {
        file.renameTo(outputFile);
        fileModifierService.updateNotification(100);
    }

    @Override
    protected void prepareAndValidate() {
        if (FileUtils.fileMoveAndCopyValidationAndErrorToasts(file, outputFile.getParentFile(), fileModifierService)) {
            cancelOperation();
        }
        conflict = outputFile.exists();
    }

    @Override
    protected void getInfoFromUser() {
        if (conflict) {
            askYesNo(fileModifierService.getString(R.string.overwrite)+" "+outputFile.getName()+"?");
        } else {
            finishTakingInput();
        }
    }
    @Override
    public void doOperationWithoutThreadOrUserQuestions() {
        initMemVarFromArgs();
        doOperation();
    }
}
