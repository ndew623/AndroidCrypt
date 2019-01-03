package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.content.SharedPreferences;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

/**
 * SettingsHelper provides an interface to Android's SharedPreferences.
 */

public final class SettingsHelper {

    public static final int FILE_ICON_VIEWER=0;
    public static final int FILE_LIST_VIEWER=1;

    /*Defaults*/
    public static final int FILE_DEFAULT_VIEWER=FILE_ICON_VIEWER;
    public static final boolean USE_DARK_THEME_DEFAULT = false;
    public static final int AESCRYPT_DEFAULT_VERSION = CryptoThread.VERSION_2;

    private static final String SHARED_PREFERENCES_FILE = "com.dewdrop623.androidcrypt.SettingsHelper.SHARED_PREFERENCES_FILE";
    private static final String AESCRYPT_VERSION_PREF = "com.dewdrop623.androidcrypt.SettingsHelper.AESCRYPT_VERSION_PREF";
    private static final String FILE_VIEWER_TYPE = "com.dewdrop623.androidcrypt.FileBrowsing.ui.SettingsHelper.FILE_VIEWER_TYPE";
    private static final String MOUNTPOINT_URIS = "com.dewdrop623.androidcrypt.FileBrowsing.ui.SettingsHelper.MOUNTPOINT_URIS";
    private static final String USE_DARK_THEME = "com.dewdrop623.androidcrypt.SettingsHelper.USE_DARK_THEME";

    private static final String ENCODING_MAGIC_STRING = "!@#";

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

    private static void sharedPreferencesPutStringSet(Context context, String key, Set<String> values) {
        getSharedPreferencesFile(context).edit().putStringSet(key, values).apply();
    }

    /***********************************
     * Set settings
     */

    public static void setAESCryptVersion(Context context, int version) {
        if (version != CryptoThread.VERSION_1 && version != CryptoThread.VERSION_2) {
            version = AESCRYPT_DEFAULT_VERSION;
        }
        sharedPreferencesPutInt(context, AESCRYPT_VERSION_PREF, version);
    }

    //set the type of file viewer using this class's integer global constants for file viewer types
    public static void setFilePickerType(Context context, int fileViewerType) {
        sharedPreferencesPutInt(context, FILE_VIEWER_TYPE, fileViewerType);
    }

    public static void setExternalMountpointUris(Context context, HashMap<String,String> mountPointUriSet) {
        sharedPreferencesPutStringSet(context, MOUNTPOINT_URIS, encodeMountpointHashmapToSet(mountPointUriSet));
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

    //get the integer representing the file view type (as defined by this class's global constants)
    public static int getFilePickerType(Context context) {
        return getSharedPreferencesFile(context).getInt(FILE_VIEWER_TYPE, FILE_DEFAULT_VIEWER);
    }

    public static boolean getUseDarkTeme(Context context) {
        return getSharedPreferencesFile(context).getBoolean(USE_DARK_THEME, USE_DARK_THEME_DEFAULT);
    }

    public static HashMap<String,String> getExternalMountpointUris(Context context) {
        return decodeMountpointSetToHashmap(
                getSharedPreferencesFile(context).getStringSet(MOUNTPOINT_URIS, null));
    }

    /*
    * Convert the convenient Hashmap of mountpoints and their TreeUris to a set that can
    * be stored by SharedPreferences.
    * Encoding format
    * x!@#/path/to/mountpointTree:Uri/of/Mountpoint
    * (x is a number with an indeterminate number of digits. the file path is x chars long)
    * */
    private static Set<String> encodeMountpointHashmapToSet(HashMap<String, String> mountpoints) {
        Set<String> result = new HashSet<>();
        for (Map.Entry<String, String> entry : mountpoints.entrySet()) {
            result.add(Integer.valueOf(entry.getKey().length()).toString().concat(ENCODING_MAGIC_STRING).concat(entry.getKey()).concat(entry.getValue()));
        }
        return result;
    }

    /**
     * Decode Set stored by SharedPreferences to Hashmap that is convenient.
     */
    private static HashMap<String,String> decodeMountpointSetToHashmap(Set<String> mountpoints) {
        if (mountpoints == null) {
            return null;
        }
        HashMap<String, String> result = new HashMap<>();
        for (String s : mountpoints) {
            int mountDirLength = Integer.parseInt(s.substring(0,s.indexOf(ENCODING_MAGIC_STRING)));
            int prefixLength = s.indexOf(ENCODING_MAGIC_STRING)+ENCODING_MAGIC_STRING.length();
            result.put(s.substring(prefixLength,prefixLength+mountDirLength), s.substring(prefixLength+mountDirLength));
        }
        return result;
    }
}
