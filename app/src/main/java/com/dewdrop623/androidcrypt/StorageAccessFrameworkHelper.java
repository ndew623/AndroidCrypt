package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * provides access to the StorageAccessFramework
 */

public class StorageAccessFrameworkHelper {

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
     *Pick a directory with the Storage Access Framework
     */
    public static void safPickDirectory(MainActivityFragment mainActivityFragment, final int REQUEST_CODE) {
        Intent intent = new Intent(Intent.ACTION_CREATE_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        mainActivityFragment.startActivityForResult(intent, REQUEST_CODE);
    }

    /*
    * return an input stream for a given Uri
    * */
    public static InputStream getUriInputStream (Context context, Uri uri) throws IOException {
        return context.getContentResolver().openInputStream(uri);
    }

    /**
     * return an output stream for a given Uri
     */
    public static OutputStream getUriOutputStream (Context context, Uri uri) throws IOException {
        return context.getContentResolver().openOutputStream(uri);
    }
}
