package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * SettingsHelper provides an interface to Android's SharedPreferences.
 */

public final class SettingsHelper {

    /*Defaults*/
    public static final boolean USE_DARK_THEME_DEFAULT = false;
    public static final int AESCRYPT_DEFAULT_VERSION = CryptoThread.VERSION_3;

    private static final String SHARED_PREFERENCES_FILE = "com.dewdrop623.androidcrypt.SettingsHelper.SHARED_PREFERENCES_FILE";
    private static final String AESCRYPT_VERSION_PREF = "com.dewdrop623.androidcrypt.SettingsHelper.AESCRYPT_VERSION_PREF";
    private static final String USE_DARK_THEME = "com.dewdrop623.androidcrypt.SettingsHelper.USE_DARK_THEME";

    private static SharedPreferences sharedPreferences;

    private SettingsHelper() {

    }

    private static SharedPreferences getSharedPreferencesFile(Context context) {
        if (sharedPreferences == null) {
            sharedPreferences = context.getSharedPreferences(SHARED_PREFERENCES_FILE, 0);
        }
        return sharedPreferences;
    }

    private static void sharedPreferencesPutInt(Context context, String key, int value) {
        getSharedPreferencesFile(context).edit().putInt(key, value).apply();
    }

    private static void sharedPreferencesPutBoolean(Context context, String key, boolean value) {
        getSharedPreferencesFile(context).edit().putBoolean(key, value).apply();
    }

    private static void sharedPreferencesPutString(Context context, String key, String value) {
        getSharedPreferencesFile(context).edit().putString(key, value).apply();
    }

    /***********************************
     * Set settings
     */

    public static void setAESCryptVersion(Context context, int version) {
        if (version != CryptoThread.VERSION_1 && version != CryptoThread.VERSION_2 && version != CryptoThread.VERSION_3) {
            version = AESCRYPT_DEFAULT_VERSION;
        }
        sharedPreferencesPutInt(context, AESCRYPT_VERSION_PREF, version);
    }

    public static void setUseDarkTheme(Context context, boolean show) {
        sharedPreferencesPutBoolean(context, USE_DARK_THEME, show);
    }

    /***********************************
     * Get settings
     */

    public static int getAESCryptVersion(Context context) {
        return getSharedPreferencesFile(context).getInt(AESCRYPT_VERSION_PREF, AESCRYPT_DEFAULT_VERSION);
    }

    public static boolean getUseDarkTeme(Context context) {
        return getSharedPreferencesFile(context).getBoolean(USE_DARK_THEME, USE_DARK_THEME_DEFAULT);
    }
}
