package com.dewdrop623.androidaescrypt.FileOperations.operator;

import java.io.File;

/**
 * creates folders
 */

public class CreateFolderOperator extends FileOperator{
    boolean done = false;

    public CreateFolderOperator(File file, String[] args) {
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
        file.mkdir();
        done = true;
    }
}
