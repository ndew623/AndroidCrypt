package com.dewdrop623.androidcrypt.FileBrowsing.ui;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Environment;

import com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.IconFileViewer;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.ListFileViewer;

import java.io.File;

/**
 * provide an interface to the app settings shared preferences file
 */

public class SettingsHelper {

    public static final int FILE_ICON_VIEWER=0;
    public static final int FILE_LIST_VIEWER=1;
    public static final int FILE_DEFAULT_VIEWER=FILE_ICON_VIEWER;

    private static final String SHARED_PREFERENCES_SETTINGS = "com.dewdrop623.androidcrypt.FileBrowsing.ui.SettingsHelper.SHARED_PREFERENCES_SETTINGS";
    private static final String SHARED_PREFERENCES_START_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.SettingsHelper.SHARED_PREFERENCES_START_DIRECTORY_KEY";
    private static final String SHARED_PREFERENCES_FILE_VIEWER_TYPE = "com.dewdrop623.androidcrypt.FileBrowsing.ui.SettingsHelper.SHARED_PREFERENCES_FILE_VIEWER_TYPE ";
    private static SharedPreferences sharedPreferences;

    private static boolean layoutChanged = false;

    //get the sharedprefences file
    private static SharedPreferences getSharedPreferencesSettingsFile(Context context) {
        if (sharedPreferences == null) {
            sharedPreferences = context.getSharedPreferences(SHARED_PREFERENCES_SETTINGS, 0);
        }
        return sharedPreferences;
    }

    //set the starting directory in shared preferences
    public static void setStartDirectory(Context context, File directory) {
        getSharedPreferencesSettingsFile(context).edit().putString(SHARED_PREFERENCES_START_DIRECTORY_KEY, directory.getAbsolutePath()).apply();
    }

    //returns the preferential start directory. if the option has not been set a default value will be returned
    public static String getStartDirectory (Context context) {
        return getSharedPreferencesSettingsFile(context).getString(SHARED_PREFERENCES_START_DIRECTORY_KEY, Environment.getExternalStorageDirectory().toString());
    }

    //change settings to their default values
    public static void revertToDefaults(Context context) {
        layoutChanged = true;
        getSharedPreferencesSettingsFile(context).edit().clear().apply();
    }

    //set the type of file viewer using this class's integer global constants for file viewer types
    public static void setFileViewerType(Context context, int fileViewerType) {
        layoutChanged = true;
        getSharedPreferencesSettingsFile(context).edit().putInt(SHARED_PREFERENCES_FILE_VIEWER_TYPE, fileViewerType).apply();
    }

    //return the an initialized instance of the implementation of FileViewer that is in settings
    public static FileViewer getCorrectFileViewerInstance(Context context) {
        int fileViewerType = getSharedPreferencesSettingsFile(context).getInt(SHARED_PREFERENCES_FILE_VIEWER_TYPE, FILE_DEFAULT_VIEWER);
        switch (fileViewerType) {
            case FILE_ICON_VIEWER:
                return new IconFileViewer();
            case FILE_LIST_VIEWER:
                return new ListFileViewer();
            default:
                return new IconFileViewer();//should be impossible to reach
        }
    }

    //get the integer representing the file view type (as defined by this class's global constants)
    public static int getFileViewerType(Context context) {
        return getSharedPreferencesSettingsFile(context).getInt(SHARED_PREFERENCES_FILE_VIEWER_TYPE, FILE_DEFAULT_VIEWER);
    }

    //MainActivity can use this to know if it needs to recreate itself after a settings change
    public static boolean layoutChanged() {
        boolean changed = layoutChanged;
        layoutChanged = false;
        return changed;
    }

}
