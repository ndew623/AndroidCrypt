package com.dewdrop623.androidcrypt;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.provider.OpenableColumns;
import android.support.v7.app.AlertDialog;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * StorageAccessFrameworkHelper provides an interface to the StorageAccessFramework.
 */

public final class StorageAccessFrameworkHelper {

    public static final int SAF_SDCARD_REQUEST_CODE = 44;

    private StorageAccessFrameworkHelper() {

    }

    public static void findSDCardWithDialog(final Activity activity) {
        //show dialog
        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        builder.setTitle(R.string.sdcard).setMessage(R.string.how_to_locate_sdcard).setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                findSDCard(activity);
            }
        });
        //positive button listener that calls findSDCard
    }
    public static OutputStream getOutputStreamWithSAF(Context context, File file) throws FileNotFoundException {
        Uri uri = Uri.fromFile(file);
        return context.getContentResolver().openOutputStream(uri);
    }
    private static void findSDCard(Activity activity) {
        activity.startActivityForResult(new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE), SAF_SDCARD_REQUEST_CODE);
    }

    private static void preApi21FindSDCard() {

    }

}
