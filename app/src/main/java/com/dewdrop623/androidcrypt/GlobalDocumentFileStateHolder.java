package com.dewdrop623.androidcrypt;

import android.support.v4.provider.DocumentFile;

/**
 * GlobalDocumentFileStateHolder holds the state of DocumentFile objects throughout the application.
 * This is necessary because there is no reliable way to recover a DocumentFile from its Uri string.
 */

public class GlobalDocumentFileStateHolder {
    private static DocumentFile savedCurrentDirectoryForRotate = null;
    private static DocumentFile initialFilePickerDirectory = null;

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
