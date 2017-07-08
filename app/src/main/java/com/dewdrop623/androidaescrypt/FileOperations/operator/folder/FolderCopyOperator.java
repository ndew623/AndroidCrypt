package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;

/**
 * recursively copies a directories contents
 */

public class FolderCopyOperator extends FileOperator{

    boolean done = false;

    public FolderCopyOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    public int getProgress() {
        if(done) {
            return 100;
        }
        return 0;
    }

    @Override
    public void doOperation() {
        File destination = new File(args.getString(FileCopyOperator.FILE_COPY_DESTINATION_ARG));
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
                Bundle args = new Bundle();
                args.putString(FileCopyOperator.FILE_COPY_DESTINATION_ARG, newFolder.getAbsolutePath());
                new FileCopyOperator(file, args, fileModifierService).run();
            }
        }
    }
}
