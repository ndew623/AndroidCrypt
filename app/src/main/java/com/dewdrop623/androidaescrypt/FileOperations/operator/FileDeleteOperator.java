package com.dewdrop623.androidaescrypt.FileOperations.operator;

import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;
import java.util.HashMap;

/**
 * deletes files
 */

public class FileDeleteOperator extends FileOperator {
    private boolean done=false;
    public FileDeleteOperator(File file, HashMap<String, String> args){
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
        file.delete();//TODO there needs to be more here. needs logic for nonempty directories and for symbolic links
        done=true;
    }
}
