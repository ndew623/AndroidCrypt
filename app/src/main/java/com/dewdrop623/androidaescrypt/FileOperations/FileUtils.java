package com.dewdrop623.androidaescrypt.FileOperations;

import java.io.File;

/**
 * common functionality for fileoperators
 */

public class FileUtils {
    public static int countFilesInFolder(File folder) {
        int count = 0;
        for(File file : folder.listFiles()) {
            if (file.isDirectory()) {
                count+=countFilesInFolder(file);
            } else {
                count++;
            }
        }
        return count;
    }
}
