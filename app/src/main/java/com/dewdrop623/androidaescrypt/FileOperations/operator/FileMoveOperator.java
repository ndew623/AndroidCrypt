package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;
import android.util.Log;

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

    private String outputFileName;
    private boolean validFilename = false;
    private boolean conflict = false;
    private File outputFile;

    public FileMoveOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    protected void initMemVarFromArgs() {
        outputFileName = args.getString(FILE_NEW_NAME_ARG, file.getName());
        outputFile = new File(args.getString(FILE_MOVE_DESTINATION_ARG)+"/"+outputFileName);
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
        if (response == null) {
            cancelOperation();
            return;
        }
        validFilename = FileUtils.validFilename(response, fileModifierService);
        if (validFilename) {
            outputFile = new File(args.getString(FILE_MOVE_DESTINATION_ARG)+"/"+response);
            conflict = outputFile.exists();
        }
        getInfoFromUser();
    }

    @Override
    protected void doOperation() {
        if (FileUtils.onSameMountpoint(file, outputFile)) {
            file.renameTo(outputFile);
        } else {
            //file copy
            Bundle args = new Bundle();
            args.putString(FileCopyOperator.FILE_COPY_DESTINATION_ARG, outputFile.getParent());
            new FileCopyOperator(file, args, fileModifierService).doOperationWithoutThreadOrUserQuestions();
            //validate copy success
            if (outputFile.exists() && (outputFile.length() == file.length())) {
                //file delete
                new FileDeleteOperator(file, null, fileModifierService).doOperationWithoutThreadOrUserQuestions();
            } else {
                fileModifierService.showToast(fileModifierService.getString(R.string.error_moving_file)+": "+file.getName());
            }
        }
        fileModifierService.updateNotification(100);
    }

    @Override
    protected boolean prepareAndValidate() {
        if (!FileUtils.fileMoveAndCopyValidationAndErrorToasts(file, outputFile.getParentFile(), fileModifierService)) {
            return false;
        }
        validFilename = FileUtils.validFilename(outputFileName, fileModifierService);
        conflict = outputFile.exists();
        return true;
    }

    @Override
    protected void getInfoFromUser() {
        if (!validFilename) {
            askForTextOrCancel(fileModifierService.getString(R.string.invalid_filename)+". "+fileModifierService.getString(R.string.try_again)+"?");
        }
        else if (conflict) {
            askYesNo(fileModifierService.getString(R.string.overwrite)+" "+outputFile.getName()+"?");
        } else {
            finishTakingInput();
        }
    }
    @Override
    public void doOperationWithoutThreadOrUserQuestions() {
        initMemVarFromArgs();
        prepareAndValidate();
        doOperation();
    }
}
