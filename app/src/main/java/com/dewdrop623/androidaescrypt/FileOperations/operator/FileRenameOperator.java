package com.dewdrop623.androidaescrypt.FileOperations.operator;

import java.io.File;
import java.util.HashMap;

/**
 * renames files
 */

public class FileRenameOperator extends FileOperator {
    public static final String NEW_FILENAME_ARG = "com.dewdrop623.androidaescrypt.FileOperations.operator.FileRenameOperator.NEW_FILENAME_ARG";
    boolean done = false;
    public FileRenameOperator(File file, HashMap<String, String> args) {
        super(file, args);
    }

    @Override
    public int getProgress() {
        if (done) {
            return 100;
        }
        return 0;
    }

    @Override
    public void execute() {
        File newFileName = new File(file.getAbsolutePath().substring(0, file.getAbsolutePath().lastIndexOf('/')+1)+args.get(NEW_FILENAME_ARG));
        file.renameTo(newFileName);
    }
}
