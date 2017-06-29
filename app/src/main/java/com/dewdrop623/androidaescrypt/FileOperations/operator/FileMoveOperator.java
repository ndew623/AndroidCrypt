package com.dewdrop623.androidaescrypt.FileOperations.operator;

import java.io.File;

import java.util.HashMap;

/**
 * moves files
 */

public class FileMoveOperator extends FileOperator {
    public static final String FILE_MOVE_DESTINATION_ARG = "com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator.FILE_MOVE_DESTINATION_ARG";
    boolean done = false;
    public FileMoveOperator(File file, HashMap<String, String> args) {
        super(file, args);
    }

    @Override
    public int getProgress() {
        if (done) {//TODO real progress updates
            return 100;
        }
        return 0;
    }

    @Override
    public void execute() {
        File destination = new File(args.get(FILE_MOVE_DESTINATION_ARG)+"/"+file.getName());
        file.renameTo(destination);
    }
}
