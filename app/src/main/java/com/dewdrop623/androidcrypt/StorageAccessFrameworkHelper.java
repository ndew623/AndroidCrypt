package com.dewdrop623.androidcrypt;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import android.provider.OpenableColumns;
import android.support.v4.provider.DocumentFile;
import android.support.v7.app.AlertDialog;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

/**
 * StorageAccessFrameworkHelper provides an interface to the StorageAccessFramework.
 */

public final class StorageAccessFrameworkHelper {

    public static final int SAF_SDCARD_REQUEST_CODE = 44;

    private StorageAccessFrameworkHelper() {

    }

    public static void findSDCardWithDialog(final Activity activity) {
        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        builder.setTitle(R.string.sdcard).setMessage(R.string.how_to_locate_sdcard)
                .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                findSDCard(activity);
            }
        });
        builder.show();
    }
    public static OutputStream getOutputStreamWithSAF(Context context, File newFile) throws IOException {
        OutputStream outputStream = null;
        try {
            outputStream = new FileOutputStream(newFile);
            //context.getContentResolver().openOutputStream((DocumentFile.fromFile(newFile.getParentFile())
              //      .createFile("", newFile.getName())).getUri());
        } catch (IOException ioe) {
            if (ioe.getCause().getMessage().equals("open failed: EACCES (Permission denied)")) {
                /**
                 * The newFile is probably on the SD Card.
                 *
                 * Split the file into its individual parts and use that combined with the DocumentFile
                 * to find the correct directory to write in.
                 * If the SD Card's name is not in the path and write access is denied, throw an exception.
                 * If DocumentFile.listFiles() does not contain the folder in newFile's path, throw an exception
                 */
                DocumentFile sdCardDirectory = DocumentFile.fromTreeUri(context, Uri.parse(SettingsHelper.getSdcardRoot(context)));
                String sdCardName = sdCardDirectory.getName();
                String newFilePathString = newFile.getAbsolutePath();
                String[] newFileSplitPath = newFilePathString.split("/");
                boolean foundSDCard = false;
                boolean fileNotFound = false;
                for (int i = 0; (i < newFileSplitPath.length) && !fileNotFound; i++) {
                    if (!foundSDCard) {
                        if (sdCardName.equals(newFileSplitPath[i])) {
                            foundSDCard = true;
                        }
                    } else if (i == newFileSplitPath.length - 1) {
                        outputStream = context.getContentResolver().openOutputStream(
                                sdCardDirectory.createFile("", newFileSplitPath[newFileSplitPath.length - 1])
                                        .getUri());
                    } else {
                        fileNotFound = true;
                        for (int j = 0; j < sdCardDirectory.listFiles().length; j++) {
                            if (newFileSplitPath[i].equals(sdCardDirectory.listFiles()[j].getName())) {
                                fileNotFound = false;
                                sdCardDirectory = sdCardDirectory.listFiles()[j];
                            }
                        }
                    }
                }
                if (!foundSDCard) {
                    throw new IOException(context.getString(R.string.file_write_access_denied));
                } else if (fileNotFound) {
                    throw new IOException(context.getString(R.string.file_not_found));
                }
            }
        }
        return outputStream;
    }

    private static void findSDCard(Activity activity) {
        activity.startActivityForResult(new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE), SAF_SDCARD_REQUEST_CODE);
    }

    private static void preApi21FindSDCard() {

    }

}
