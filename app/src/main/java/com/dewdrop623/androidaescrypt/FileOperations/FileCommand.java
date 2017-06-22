package com.dewdrop623.androidaescrypt.FileOperations;

import java.io.File;
import java.util.HashMap;

/**
 * store a file operation task to be executed
 */

public class FileCommand {
    public File file;
    public FileOperationType fileOperationType;
    public HashMap<String, String> args;
    public FileCommand(File file, FileOperationType fileOperationType, HashMap<String, String> args) {
        this.file=file;
        this.fileOperationType=fileOperationType;
        this.args = args;
    }
}
