package com.dewdrop623.androidaescrypt.FileOperations.operator;

import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;

/**
 * deletes files
 */

public class FileDeleteOperator extends FileOperator {
    private boolean done=false;
    public FileDeleteOperator(File file, String arg){
        super(file, arg);
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
        file.delete();//TODO there needs to be more here. needs logic for nonempty directories and for symbolic links
        done=true;
    }
}
