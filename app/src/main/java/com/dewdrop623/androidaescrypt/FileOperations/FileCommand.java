package com.dewdrop623.androidaescrypt.FileOperations;

import java.io.File;

/**
 * store a file operation task to be executed
 */

public class FileCommand {
    public File file;
    public FileOperationType fileOperationType;
    public String arg;
    public FileCommand(File file, FileOperationType fileOperationType, String arg) {
        this.file=file;
        this.fileOperationType=fileOperationType;
        this.arg = arg;
    }
}
