package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;

import java.io.File;

/**
 * renames files
 */

public class FileRenameOperator extends FileOperator {
    public static final String NEW_FILENAME_ARG = "com.dewdrop623.androidaescrypt.FileOperations.operator.FileRenameOperator.NEW_FILENAME_ARG";
    boolean done = false;
    public FileRenameOperator(File file, Bundle args, FileModifierService fileModifierService) {
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
        File newFileName = new File(file.getAbsolutePath().substring(0, file.getAbsolutePath().lastIndexOf('/')+1)+args.getString(NEW_FILENAME_ARG));
        file.renameTo(newFileName);
    }
}
