package com.dewdrop623.androidaescrypt.FileBrowsing;

import android.os.Environment;
import android.os.FileObserver;

import java.io.File;

/**
 * keeps track of current directory, monitors files for changes, takes commands from FileViewer and executes them
 */

public class FileBrowser {

    private String currentPath;
    private FileViewer fileViewer;
    private FileObserver fileObserver;

    public FileBrowser() {
        //TODO get path from settings, for now just use default external storage path
        currentPath = Environment.getExternalStorageDirectory().toString();
        monitorCurrentPathForChanges();
    }
    public void setFileViewer(FileViewer fileViewer) {
        this.fileViewer = fileViewer;
        updateFileViewer();
    }
    private void updateFileViewer() {
        File[] files = new File(currentPath).listFiles();
        if (files == null) {
            files = new File[]{};
        }
        fileViewer.setFileList(files);
    }
    private void monitorCurrentPathForChanges() {
        if (fileObserver != null) {
            fileObserver.stopWatching();
        }
        fileObserver = new FileObserver(currentPath) {
            @Override
            public void onEvent(int event, String path) {
                if (event == FileObserver.CLOSE_WRITE || event == FileObserver.CREATE || event == FileObserver.MODIFY || event == FileObserver.DELETE || event == FileObserver.MOVED_FROM
                        || event == FileObserver.MOVED_TO || event == FileObserver.ATTRIB)
                updateFileViewer();
            }
        };
        fileObserver.startWatching();
    }
    public void changePath(String newPath) {
        currentPath = newPath;
        monitorCurrentPathForChanges();
        updateFileViewer();
    }
    public void modifyFile(/*FileCommand fileCommand*/) {
        //TODO execute file commands
    }

}
