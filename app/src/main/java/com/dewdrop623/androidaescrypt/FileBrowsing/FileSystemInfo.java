package com.dewdrop623.androidaescrypt.FileBrowsing;

import android.os.Environment;

import java.io.File;

/**
 *Gets information about file and directories - probably not needed
 */
//TODO delete if never used
public class FileSystemInfo {

    public static boolean isDirectory(String path) {
        return (new File(path)).isDirectory();
    }
    public static File[] getFileListAtPath(String path) {
        File file = new File(path);
        if (file.isDirectory()) {
            return file.listFiles();
        } else {
            return null;
        }
    }
    public static long getFileSizeOf(String path) {
        return (new File(path)).length();
    }
    public static String getExternalStorageDirectory() {
        return Environment.getExternalStorageDirectory().toString();
    }
}
