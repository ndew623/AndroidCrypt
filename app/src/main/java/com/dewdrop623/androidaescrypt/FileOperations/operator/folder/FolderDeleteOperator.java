package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;
import java.util.HashMap;

/**
 * recursively deletes folder contents
 */

public class FolderDeleteOperator extends FileOperator{
    boolean done = false;
    public FolderDeleteOperator(File file, HashMap<String, String> args) {
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
        deleteFolder(file);
        done=true;
    }
    private void deleteFolder(File file) {
        File[] subfiles = file.listFiles();
        for (File subfile : subfiles) {
            if (subfile.isDirectory()) {
                deleteFolder(subfile);
            } else {
                subfile.delete();
            }
        }
        file.delete();
    }
}
