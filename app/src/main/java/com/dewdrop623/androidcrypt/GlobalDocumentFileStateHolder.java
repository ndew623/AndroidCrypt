package com.dewdrop623.androidcrypt;

import android.support.v4.provider.DocumentFile;

/**
 * GlobalDocumentFileStateHolder holds the state of DocumentFile objects throughout the application.
 * This is necessary because there is no reliable way to recover a DocumentFile from its Uri string.
 */

public class GlobalDocumentFileStateHolder {
    //used by MainActivityFragment
    private static DocumentFile savedInputParentDirectoryForRotate = null;
    private static DocumentFile savedOutputParentDirectoryForRotate = null;

    //used by FilePicker
    private static DocumentFile savedCurrentDirectoryForRotate = null;
    private static DocumentFile initialFilePickerDirectory = null;

    //used to get rw access to files for encryption
    private static DocumentFile inputFileParentDirectory = null;
    private static DocumentFile outputFileParentDirectory = null;


    public static DocumentFile getAndClearSavedInputParentDirectoryForRotate() {
        DocumentFile savedInputDirectoryForRotate = GlobalDocumentFileStateHolder.savedInputParentDirectoryForRotate;
        GlobalDocumentFileStateHolder.savedInputParentDirectoryForRotate = null;
        return savedInputDirectoryForRotate;
    }

    public static void setSavedInputParentDirectoryForRotate(DocumentFile savedInputParentDirectoryForRotate) {
        GlobalDocumentFileStateHolder.savedInputParentDirectoryForRotate = savedInputParentDirectoryForRotate;
    }

    public static DocumentFile getAndClearSavedOutputParentDirectoryForRotate() {
        DocumentFile savedOutputDirectoryForRotate = GlobalDocumentFileStateHolder.savedOutputParentDirectoryForRotate;
        GlobalDocumentFileStateHolder.savedOutputParentDirectoryForRotate = null;
        return savedOutputDirectoryForRotate;
    }

    public static void setSavedOutputParentDirectoryForRotate(DocumentFile savedOutputParentDirectoryForRotate) {
        GlobalDocumentFileStateHolder.savedOutputParentDirectoryForRotate = savedOutputParentDirectoryForRotate;
    }

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
