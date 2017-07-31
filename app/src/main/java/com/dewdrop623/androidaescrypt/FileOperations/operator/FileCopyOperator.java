package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;

/**
 * copies files
 */

public class FileCopyOperator extends FileOperator {
    public static final String FILE_COPY_DESTINATION_ARG = "com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator.FILE_COPY_DESTINATION_ARG";

    private boolean conflict = false;
    private File outputFile;

    public FileCopyOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    protected void initMemVarFromArgs() {
        outputFile = new File(args.getString(FILE_COPY_DESTINATION_ARG) + "/" + file.getName());
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
        try {
            if (!outputFile.exists()) {
                outputFile.createNewFile();
            }
            FileChannel sourceChannel = null;
            FileChannel destinationChannel = null;
            try {
                sourceChannel = new FileInputStream(file).getChannel();
                destinationChannel = new FileOutputStream(outputFile).getChannel();
                destinationChannel.transferFrom(sourceChannel, 0, sourceChannel.size());
            } catch (IOException ioe) {
                fileModifierService.showToast(fileModifierService.getString(R.string.ioexception));
            } finally {
                if (sourceChannel != null) {
                    sourceChannel.close();
                }
                if (destinationChannel != null) {
                    destinationChannel.close();
                }
            }
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
        fileModifierService.updateNotification(100);
    }

    @Override
    protected boolean prepareAndValidate() {
        if(!FileUtils.fileMoveAndCopyValidationAndErrorToasts(file, outputFile.getParentFile(), fileModifierService)) {
            return false;
        }
        conflict = outputFile.exists();
        return true;
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
