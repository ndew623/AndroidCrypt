package com.dewdrop623.androidcrypt;

import android.app.Activity;
import android.app.Fragment;
import android.content.Intent;
import android.net.Uri;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * provides access to the StorageAccessFramework
 */

public class StorageAccessFrameworkHelper {

    /*
    * start an Storage Access Framework activity for selecting the input file
     */
    public static void safOpenFile(MainActivityFragment mainActivityFragment, final int REQUEST_CODE) {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        mainActivityFragment.startActivityForResult(intent, REQUEST_CODE);
    }

    /**
     *Pick a directory with the Storage Access Framework
     */
    public static void safPickDirectory(MainActivityFragment mainActivityFragment, final int REQUEST_CODE) {
        //TODO do this
    }

    /*
    * return an input stream for a given Uri
    * */
    public static InputStream getUriInputStream (MainActivityFragment mainActivityFragment, Uri uri) throws IOException {
        return mainActivityFragment.getActivity().getContentResolver().openInputStream(uri);
    }
}
