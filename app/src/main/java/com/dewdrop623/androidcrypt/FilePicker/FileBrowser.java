package com.dewdrop623.androidcrypt.FilePicker;

import android.content.Context;
import android.os.Environment;
import android.os.FileObserver;
import android.os.Handler;
import android.os.Looper;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * FilePickerFragment uses FileBrowser to get the information it needs about the file system.
 */

public class FileBrowser {

    private File currentDirectory;
    private FilePickerFragment filePickerFragment;
    private FileObserver fileChangeMonitor;

    //TODO delete
    //public static final DocumentFile internalStorageHome = DocumentFile.fromFile(Environment.getExternalStorageDirectory());
    //public static final String PARENT_FILE_NAME = "..";

    public FileBrowser(Context context) {
        currentDirectory = getHomeDirectory();
        monitorCurrentPathForChanges();
    }
    public void setFilePickerFragment(FilePickerFragment filePickerFragment) {
        this.filePickerFragment = filePickerFragment;
        updateFileViewer();
    }
    public void updateFileViewer() {
        ArrayList<File> files = new ArrayList<>(Arrays.asList(currentDirectory.listFiles()));
        filePickerFragment.setFileList(files);
    }
    private void monitorCurrentPathForChanges() {

        if (fileChangeMonitor != null) {
            fileChangeMonitor.stopWatching();
        }
        fileChangeMonitor = new FileObserver(currentDirectory.getAbsolutePath()) {
            @Override
            public void onEvent(int event, String path) {
                event &= FileObserver.ALL_EVENTS;
                if (event == FileObserver.CLOSE_WRITE || event == FileObserver.CREATE || event == FileObserver.MODIFY || event == FileObserver.DELETE || event == FileObserver.MOVED_FROM
                        || event == FileObserver.MOVED_TO || event == FileObserver.ATTRIB)

                    new Handler(Looper.getMainLooper()).post(new Runnable() {//create handler and post runnable so that updateFileViewer is called from UI thread and can change UI
                        @Override
                        public void run() {
                            updateFileViewer();
                        }
                    });
            }
        };
        fileChangeMonitor.startWatching();
    }
    /*TODO delete
    public String getCurrentPathName() {
        return StorageAccessFrameworkHelper.getDocumentFilePath(currentDirectory);
    }*/

    //change the displayed file path to newDirectory if it is a directory. If the new path is ".." then change the current path to the parent of the current directory.
    public void setCurrentDirectory(File newDirectory) {
        if(newDirectory != null && newDirectory.exists() && !newDirectory.isDirectory()) {
            return;
        }
        currentDirectory = newDirectory;
        monitorCurrentPathForChanges();
        updateFileViewer();
    }

    /**
     * Tries to change the currentDirectory to currentDirectory's parent directory.
     * Reports whether or not parent directory existed.
     */
    public boolean goToParentDirectory() {
        boolean hadParentDirectory = false;
        if (currentDirectory.getParentFile() != null) {
            setCurrentDirectory(currentDirectory.getParentFile());
            hadParentDirectory = true;
        }
        return hadParentDirectory;
    }
    /*TODO delete
    public String getCurrentDirectoryUriString() {
        return currentDirectory.getUri().toString();
    }*/

    public File getCurrentDirectory() {
        return currentDirectory;
    }
    public File getHomeDirectory() {
        return Environment.getExternalStorageDirectory();
    }
}
