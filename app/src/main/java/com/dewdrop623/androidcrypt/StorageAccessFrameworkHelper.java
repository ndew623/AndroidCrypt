package com.dewdrop623.androidcrypt;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.support.v4.provider.DocumentFile;
import android.support.v7.app.AlertDialog;
import android.view.ContextThemeWrapper;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

/**
 * StorageAccessFrameworkHelper provides an interface to the StorageAccessFramework.
 */

public final class StorageAccessFrameworkHelper {

    public static final int SAF_REMOVABLE_STORAGE_REQUEST_CODE = 44;

    private StorageAccessFrameworkHelper() {

    }

    public static void findRemovableStorageWithDialog(final Activity activity) {
        AlertDialog.Builder builder;
        if (SettingsHelper.getUseDarkTeme(activity)) {
            ContextThemeWrapper contextThemeWrapper = new ContextThemeWrapper(activity, R.style.DarkAlertDialogTheme);
            builder = new AlertDialog.Builder(contextThemeWrapper);
        } else {
            builder = new AlertDialog.Builder(activity);
        }
        /*TODO update strings when supporting multiple removable storage*/
        builder.setTitle(R.string.sdcard).setMessage(R.string.how_to_locate_sdcard)
                .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        findSDCard(activity);
                    }
                });
        builder.show();
    }

    @TargetApi(21)
    private static void findSDCard(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            activity.startActivityForResult(new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE), SAF_REMOVABLE_STORAGE_REQUEST_CODE);
        }
    }

    /**
     * Search the mountpoints of external storage for the name of the removable storage.
     * If not found, return null.
     */
    public static String findLikelyRemovableStoragePathFromName(Context context, String name) {
        if (name != null) {
            String[] externalStorageDirs = getExternalStorageDirectories(context);
            for (int i = 0; i < externalStorageDirs.length; i++) {
                if (externalStorageDirs[i].contains(name)) {
                    return externalStorageDirs[i];
                }
            }
        }
        return null;
    }

    /* returns external storage paths (directory of external memory card) as array of Strings
    * thank you stack overflow*/
    public static String[] getExternalStorageDirectories(Context context) {

        List<String> results = new ArrayList<>();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) { //Method 1 for KitKat & above
            File[] externalDirs = context.getExternalFilesDirs(null);

            for (File file : externalDirs) {
                String path = file.getPath().split("/Android")[0];

                /*boolean addPath = false;


                This code would exclude internal storage. (Internal as in hardware-wise.)
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    addPath = Environment.isExternalStorageRemovable(file);
                } else {
                    addPath = Environment.MEDIA_MOUNTED.equals(EnvironmentCompat.getStorageState(file));
                }

                if (addPath) {
                    results.add(path);
                }*/
                results.add(path);
            }
        }

        if (results.isEmpty()) { //Method 2 for all versions
            // better variation of: http://stackoverflow.com/a/40123073/5002496
            String output = "";
            try {
                ProcessBuilder processBuilder = new ProcessBuilder().command("sh", "-c", "mount | grep /dev/block/vold");
                final Process process = processBuilder.redirectErrorStream(true).start();
                /*final Process process = new ProcessBuilder().command("")
                        .redirectErrorStream(true).start();*/
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
                    String[] splitVoldPoint = voldPoint.split(" ");
                    if (splitVoldPoint.length > 2) {
                        results.add(voldPoint.split(" ")[2]);
                    }
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

    public static boolean canSupportSDCardOnAndroidVersion() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP;
    }

    public static String getDocumentFilePath(DocumentFile documentFile) {
        return getDocumentFilePath(documentFile, null);
    }

    /**
     * Gets a file path string for a document file.
     * childFileName can be null, or used to get the path of a file that doesn't exist yet
     * (DocumentFiles must exist unlike java.io.file)
     * if documentFile is null, return the empty string
     */
    public static String getDocumentFilePath(DocumentFile documentFile, String childFileName) {

        if (documentFile == null) {
            return new String();
        }

        StringBuilder pathNameBuilder = new StringBuilder();
        pathNameBuilder.append(documentFile.getName());
        while (documentFile.getParentFile() != null) {
            pathNameBuilder.insert(0, File.separator);
            pathNameBuilder.insert(0, documentFile.getParentFile().getName());
            documentFile = documentFile.getParentFile();
        }

        if (childFileName != null) {
            pathNameBuilder.append(File.separator);
            pathNameBuilder.append(childFileName);
        }

        return pathNameBuilder.toString();
    }

    /**
     * Trade a java.io.File that points to an external mountpoint (e.g. sdcard or usb drive)
     * for a DocumentFile of that same location. (DocumentFiles can be used to write files
     * to those locations.)
     * Use the map stored in SharedPreferences to make the connection.
     * Return null if no match found.
     */
    private static DocumentFile getDocumentFileForMountpoint(Context context, File mountpoint) throws IOException {
        HashMap<String,String> mountpointUris = SettingsHelper.getExternalMountpointUris(context);
        for (Map.Entry<String,String> e : mountpointUris.entrySet()) {
            if (e.getKey().equals(mountpoint.getPath())) {
                return DocumentFile.fromTreeUri(context, Uri.parse(e.getValue()));
            }
        }
        throw new IOException("Could not get permission to write file. Try adding permission for external storage devices in settings.");
    }

    /**
     * Get the directory that is the mountpoint that file is under.
     * Returns null if the file is not found under any mountpoint.
     */
    private static File getMountpointOfFile(Context context, File file) throws FileNotFoundException {
        String [] mountpoints = getExternalStorageDirectories(context);
        for (String s : mountpoints) {
            if (file.getAbsolutePath().startsWith(s)) {
                return new File(s);
            }
        }
        throw new FileNotFoundException("Could not find mountpoint of filesystem containing output file.");
    }

    public static OutputStream getFileOutputStream(Context context, File outputFile) throws IOException {
        if (outputFile.getParentFile().canWrite()) {
            outputFile.createNewFile();
            return new FileOutputStream(outputFile);
        }
        File mountpoint = getMountpointOfFile(context, outputFile);
        DocumentFile documentFile = getDocumentFileForMountpoint(context, mountpoint);
        Stack<String> fileNamesOnPath = new Stack<>();
        File f = outputFile.getParentFile();
        while (!mountpoint.equals(f)) {
            fileNamesOnPath.push(f.getName());
            f = f.getParentFile();
        }
        while (fileNamesOnPath.size() > 0) {
            documentFile = documentFile.findFile(fileNamesOnPath.pop());
            if (documentFile == null) {
                throw new FileNotFoundException("AndroidCrypt Error 310");
            }
        }
        documentFile = documentFile.createFile("txt", outputFile.getName());
        if (documentFile == null) {
            throw new IOException("Could not create file using DocumentFile API");
        }
        return context.getContentResolver().openOutputStream(documentFile.getUri(), "w");
    }
}
