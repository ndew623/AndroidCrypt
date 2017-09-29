package com.dewdrop623.androidcrypt.FileBrowsing.ui;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Environment;

import java.io.File;

/**
 * provide an interface to the app settings shared preferences file
 */

public class SettingsHelper {

    private static final String SHARED_PREFERENCES_SETTINGS = "com.dewdrop623.androidcrypt.FileBrowsing.ui.SettingsHelper.SHARED_PREFERENCES_SETTINGS";
    private static final String SHARED_PREFERENCES_START_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.SettingsHelper.SHARED_PREFERENCES_START_DIRECTORY_KEY";
    private static SharedPreferences sharedPreferences;

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
        getSharedPreferencesSettingsFile(context).edit().clear().apply();
    }
}
