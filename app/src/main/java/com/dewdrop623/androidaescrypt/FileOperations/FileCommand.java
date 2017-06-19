package com.dewdrop623.androidaescrypt.FileOperations;

import java.io.File;

/**
 * store a file operation task to be executed
 */

public class FileCommand {
    public File file;
    public FileOperationType fileOperationType;
    public FileCommand(File file, FileOperationType fileOperationType) {
        this.file=file;
        this.fileOperationType=fileOperationType;
    }
}
