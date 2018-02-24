package com.dewdrop623.androidcrypt.FilePicker;

import android.content.Context;
import android.os.Environment;
import android.support.v4.provider.DocumentFile;

import com.dewdrop623.androidcrypt.StorageAccessFrameworkHelper;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * FilePicker uses FileBrowser to get the information it needs about the file system.
 */

public class FileBrowser {

    private DocumentFile currentDirectory;
    private FilePicker filePicker;

    public static final DocumentFile internalStorageHome = DocumentFile.fromFile(Environment.getExternalStorageDirectory());
    public static final String PARENT_FILE_NAME = "..";

    public FileBrowser(Context context) {
        currentDirectory = internalStorageHome;
        monitorCurrentPathForChanges();
    }
    public void setFilePicker(FilePicker filePicker) {
        this.filePicker = filePicker;
        updateFileViewer();
    }
    public void updateFileViewer() {
        ArrayList<DocumentFile> files = new ArrayList<>(Arrays.asList(currentDirectory.listFiles()));
        filePicker.setFileList(files);
    }
    private void monitorCurrentPathForChanges() {
        /*

        TODO figure out how this works with DocumentFile
        if (fileChangeMonitor != null) {
            fileChangeMonitor.stopWatching();
        }
        fileChangeMonitor = new FileObserver(currentPath.getAbsolutePath()) {
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
        fileChangeMonitor.startWatching();*/
    }

    public String getCurrentPathName() {
        return StorageAccessFrameworkHelper.getDocumentFilePath(currentDirectory);
    }

    //change the displayed file path to newDirectory if it is a directory. If the new path is ".." then change the current path to the parent of the current directory.
    public void setCurrentDirectory(DocumentFile newDirectory) {
        if(!newDirectory.isDirectory()) {
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

    public String getCurrentDirectoryUriString() {
        return currentDirectory.getUri().toString();
    }

    public DocumentFile getCurrentDirectory() {
        return currentDirectory;
    }
    
}
