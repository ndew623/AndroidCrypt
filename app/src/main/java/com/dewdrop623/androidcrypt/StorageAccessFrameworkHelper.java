package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.provider.OpenableColumns;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * provides access to the StorageAccessFramework
 */

public final class StorageAccessFrameworkHelper {

    private StorageAccessFrameworkHelper(){

    }

    /*
    * start an Storage Access Framework activity for selecting the input file
     */
    public static void safPickFile(MainActivityFragment mainActivityFragment, final int REQUEST_CODE) {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        mainActivityFragment.startActivityForResult(intent, REQUEST_CODE);
    }

    /**
     * Pick an ouput file with the Storage Access Framework. Automatically fill the file name field with defaultFileName.
     */
    public static void safPickOutputFile(MainActivityFragment mainActivityFragment, final int REQUEST_CODE, String defaultFileName) {
        Intent intent = new Intent(Intent.ACTION_CREATE_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        if (defaultFileName != null) {
            intent.putExtra(Intent.EXTRA_TITLE, defaultFileName);
        }
        mainActivityFragment.startActivityForResult(intent, REQUEST_CODE);
    }

    /**
     * Pick an ouput file with the Storage Access Framework. No default suggestion for a file name
     */
    public static void safPickOutputFile(MainActivityFragment mainActivityFragment, final int REQUEST_CODE) {
        safPickOutputFile(mainActivityFragment, REQUEST_CODE, null);
    }

    /*
    * return an input stream for a given Uri
    * */
    public static InputStream getUriInputStream(Context context, Uri uri) throws IOException {
        return context.getContentResolver().openInputStream(uri);
    }

    /**
     * return an output stream for a given Uri
     */
    public static OutputStream getUriOutputStream(Context context, Uri uri) throws IOException {
        return context.getContentResolver().openOutputStream(uri);
    }

    /*thank you stack overflow*/
    public static String getFileNameFromUri(Uri uri, Context context) {
        String result = getInfoFromURI(uri, context, OpenableColumns.DISPLAY_NAME);
        if (result == null) {
            result = uri.getPath();
            int cut = result.lastIndexOf('/');
            if (cut != -1) {
                result = result.substring(cut + 1);
            }
        }
        return result;
    }

    //Given a uri that points to a file, get its file size in bytes.
    public static long getFileSizeFromUri(Uri uri, Context context) {
        String resultString = getInfoFromURI(uri, context, OpenableColumns.SIZE);
        long resultLong = 0;
        if (resultString != null) {
            resultLong = Long.parseLong(resultString);
        }
        return resultLong;
    }

    /*
        Use cursor to get information about a file. (OpenableColumns.SIZE or OpenableColumns.DISPLAY_NAME). I guess it queries the filesystem's meta-data for the file?
        tbh I don't know how content resolver works.
     */
    private static String getInfoFromURI(Uri uri, Context context, String openableColumn) {
        String result = null;
        if (uri.getScheme().equals("content")) {
            Cursor cursor = context.getContentResolver().query(uri, null, null, null, null);
            try {
                if (cursor != null && cursor.moveToFirst()) {
                    result = cursor.getString(cursor.getColumnIndex(openableColumn));
                }
            } finally {
                cursor.close();
            }
        }
        return result;
    }
}
