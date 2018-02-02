package com.dewdrop623.androidcrypt;

import android.support.v4.provider.DocumentFile;

/**
 * GlobalDocumentFileStateHolder holds the state of DocumentFile objects throughout the application.
 * This is necessary because there is no reliable way to recover a DocumentFile from its Uri string.
 */

public class GlobalDocumentFileStateHolder {
    private static DocumentFile savedCurrentDirectoryForRotate = null;
    private static DocumentFile initialFilePickerDirectory = null;
    private static DocumentFile inputFileParentDirectory = null;
    private static DocumentFile outputFileParentDirectory = null;

    public static DocumentFile getInputFileParentDirectory() {
        return inputFileParentDirectory;
    }

    public static void setInputFileParentDirectory(DocumentFile inputFileParentDirectory) {
        GlobalDocumentFileStateHolder.inputFileParentDirectory = inputFileParentDirectory;
    }

    public static DocumentFile getOutputFileParentDirectory() {
        return outputFileParentDirectory;
    }

    public static void setOutputFileParentDirectory(DocumentFile outputFileParentDirectory) {
        GlobalDocumentFileStateHolder.outputFileParentDirectory = outputFileParentDirectory;
    }

    public static void setSavedCurrentDirectoryForRotate(DocumentFile savedCurrentDirectoryForRotate) {
        GlobalDocumentFileStateHolder.savedCurrentDirectoryForRotate = savedCurrentDirectoryForRotate;
    }

    public static DocumentFile getAndClearSavedCurrentDirectoryForRotate() {
        DocumentFile savedCurrentDirectoryForRotate = GlobalDocumentFileStateHolder.savedCurrentDirectoryForRotate;
        GlobalDocumentFileStateHolder.savedCurrentDirectoryForRotate = null;
        return savedCurrentDirectoryForRotate;
    }

    public static boolean savedCurrentDirectoryForRotateIsNull() {
        return savedCurrentDirectoryForRotate == null;
    }

    public static void setInitialFilePickerDirectory(DocumentFile initialFilePickerDirectory) {
        GlobalDocumentFileStateHolder.initialFilePickerDirectory = initialFilePickerDirectory;
    }

    public static DocumentFile getAndClearInitialFilePickerDirectory() {
        DocumentFile initialFilePickerDirectory = GlobalDocumentFileStateHolder.initialFilePickerDirectory;
        GlobalDocumentFileStateHolder.initialFilePickerDirectory = null;
        return initialFilePickerDirectory;
    }

    public static boolean initialFilePickerDirectoryIsNull() {
        return initialFilePickerDirectory == null;
    }
}
