package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;
import java.util.HashMap;

/**
 * recursively copies a directories contents
 */

public class FolderCopyOperator extends FileOperator{

    boolean done = false;

    public FolderCopyOperator(File file, HashMap<String, String> args) {
        super(file, args);
    }

    @Override
    public int getProgress() {
        if(done) {
            return 100;
        }
        return 0;
    }

    @Override
    public void execute() {
        File destination = new File(args.get(FileCopyOperator.FILE_COPY_DESTINATION_ARG));
        copyFolder(file, destination);
    }
    private void copyFolder(File folder, File destination) {
        File newFolder = new File(destination.getAbsolutePath()+"/"+folder.getName());
        newFolder.mkdir();
        File[] contents = folder.listFiles();
        for (File file : contents) {
            if (file.isDirectory()) {
                copyFolder(file, newFolder);
            } else {
                HashMap<String, String> args = new HashMap<>();
                args.put(FileCopyOperator.FILE_COPY_DESTINATION_ARG, newFolder.getAbsolutePath());
                new FileCopyOperator(file, args).execute();
            }
        }
    }
}
