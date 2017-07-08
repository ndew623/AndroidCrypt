package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;

import java.io.File;

/**
 * interface for classes that do work on files, implements runnable: to be done on seperate thread
 */

public abstract class FileOperator implements Runnable{
    protected File file;
    protected Bundle args;
    protected FileModifierService fileModifierService;
    public FileOperator(File file, Bundle args, FileModifierService fileModifierService) {
        this.file = file;
        this.args = args;
        this.fileModifierService = fileModifierService;
    }
    public abstract int getProgress();
    public void run() {
        doOperation();
        fileModifierService.stopSelf();
    }
    public abstract void doOperation();
}
