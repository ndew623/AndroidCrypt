package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;

import java.io.File;

/**
 * moves files
 */

public class FileMoveOperator extends FileOperator {
    public static final String FILE_MOVE_DESTINATION_ARG = "com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator.FILE_MOVE_DESTINATION_ARG";
    boolean done = false;
    public FileMoveOperator(File file, Bundle args, FileModifierService fileModifierService) {
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
    public void doOperation() {
        File destination = new File(args.getString(FILE_MOVE_DESTINATION_ARG)+"/"+file.getName());
        file.renameTo(destination);
    }
}
