package com.dewdrop623.androidaescrypt.FileOperations.operator;

import java.io.File;

/**
 * interface for classes that do work on files
 */

public abstract class FileOperator {
    protected File file;
    protected String arg;
    public FileOperator(File file, String arg) {
        this.file = file;
        this.arg = arg;
    }
    public abstract int getProgress();
    public abstract void execute();
}
