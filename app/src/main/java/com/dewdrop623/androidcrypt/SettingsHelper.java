package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * SettingsHelper provides an interface to Android's SharedPreferences.
 */

public final class SettingsHelper {

    private static final String SHARED_PREFERENCES_FILE = "com.dewdrop623.androidcrypt.SettingsHelper.SHARED_PREFERENCES_FILE";

    private static final String AESCRYPT_VERSION_PREF = "com.dewdrop623.androidcrypt.SettingsHelper.AESCRYPT_VERSION_PREF";

    public static final int AESCRYPT_DEFAULT_VERSION = CryptoThread.VERSION_2;

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

    public static void setAESCryptVersion(Context context, int version) {
        if (version != CryptoThread.VERSION_1 && version != CryptoThread.VERSION_2) {
            version = AESCRYPT_DEFAULT_VERSION;
        }
        sharedPreferencesPutInt(context, AESCRYPT_VERSION_PREF, version);
    }

    public static int getAESCryptVersion(Context context) {
        return getSharedPreferencesFile(context).getInt(AESCRYPT_VERSION_PREF, AESCRYPT_DEFAULT_VERSION);
    }
}
