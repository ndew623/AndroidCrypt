package com.dewdrop623.androidcrypt;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.support.v4.os.EnvironmentCompat;
import android.support.v4.provider.DocumentFile;
import android.support.v7.app.AlertDialog;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

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
        if (newFile.getParentFile().canWrite()) {
            outputStream = new FileOutputStream(newFile);
        } else {
            /**
             * The newFile is probably on the SD Card.
             *
             * Split the file path into its individual parts and use that combined with the
             * DocumentFile to find the correct directory to write in.
             * If the SD Card's name is not in the path, throw an exception.
             * If DocumentFile.listFiles() does not contain the folders in newFile's path, throw an exception
             * Very ugly, but it works.
             */
            String uriString = SettingsHelper.getSdcardRoot(context);
            if (uriString == null) {
                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
                    throw new IOException(context.getString(R.string.file_write_access_denied));
                }
                throw new IOException(context.getString(R.string.write_permission_denied_if_sdcard_grant_permission_in_settings));
            }
            DocumentFile sdCardDirectory = DocumentFile.fromTreeUri(context, Uri.parse(uriString));
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

                if (!foundSDCard) {
                    throw new IOException(context.getString(R.string.file_write_access_denied));
                } else if (fileNotFound) {
                    throw new IOException(context.getString(R.string.file_not_found));
                }
            }
        }
        return outputStream;
    }

    @TargetApi(21)
    private static void findSDCard(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            activity.startActivityForResult(new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE), SAF_SDCARD_REQUEST_CODE);
        }
    }

    /**
     * Search the mountpoints of external storage for the name of the SD card.
     * If not found, return null.
     */
    public static String findLikelySDCardPathFromSDCardName(Context context, String sdCardName) {
        String[] externalStorageDirs = getExternalStorageDirectories(context);
        for (int i = 0; i < externalStorageDirs.length; i++) {
            if (externalStorageDirs[i].contains(sdCardName)) {
                return externalStorageDirs[i];
            }
        }
        return null;
    }

    /* returns external storage paths (directory of external memory card) as array of Strings
    * thank you stack overflow*/
    private static String[] getExternalStorageDirectories(Context context) {

        List<String> results = new ArrayList<>();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) { //Method 1 for KitKat & above
            File[] externalDirs = context.getExternalFilesDirs(null);

            for (File file : externalDirs) {
                String path = file.getPath().split("/Android")[0];

                boolean addPath = false;

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    addPath = Environment.isExternalStorageRemovable(file);
                } else {
                    addPath = Environment.MEDIA_MOUNTED.equals(EnvironmentCompat.getStorageState(file));
                }

                if (addPath) {
                    results.add(path);
                }
            }
        }

        if (results.isEmpty()) { //Method 2 for all versions
            // better variation of: http://stackoverflow.com/a/40123073/5002496
            String output = "";
            try {
                final Process process = new ProcessBuilder().command("mount | grep /dev/block/vold")
                        .redirectErrorStream(true).start();
                process.waitFor();
                final InputStream is = process.getInputStream();
                final byte[] buffer = new byte[1024];
                while (is.read(buffer) != -1) {
                    output = output + new String(buffer);
                }
                is.close();
            } catch (final Exception e) {
                e.printStackTrace();
            }
            if (!output.trim().isEmpty()) {
                String devicePoints[] = output.split("\n");
                for (String voldPoint : devicePoints) {
                    results.add(voldPoint.split(" ")[2]);
                }
            }
        }

        //Below few lines is to remove paths which may not be external memory card, like OTG (feel free to comment them out)
        /*if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            for (int i = 0; i < results.size(); i++) {
                if (!results.get(i).toLowerCase().matches(".*[0-9a-f]{4}[-][0-9a-f]{4}")) {
                    Log.d(LOG_TAG, results.get(i) + " might not be extSDcard");
                    results.remove(i--);
                }
            }
        } else {
            for (int i = 0; i < results.size(); i++) {
                if (!results.get(i).toLowerCase().contains("ext") && !results.get(i).toLowerCase().contains("sdcard")) {
                    Log.d(LOG_TAG, results.get(i)+" might not be extSDcard");
                    results.remove(i--);
                }
            }
        }*/

        String[] storageDirectories = new String[results.size()];
        for (int i = 0; i < results.size(); ++i) storageDirectories[i] = results.get(i);

        return storageDirectories;
    }
}
