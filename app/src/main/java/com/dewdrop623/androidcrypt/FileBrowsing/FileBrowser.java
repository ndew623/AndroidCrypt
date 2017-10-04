package com.dewdrop623.androidcrypt.FileBrowsing;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.FileObserver;
import android.os.Handler;
import android.os.Looper;

import com.dewdrop623.androidcrypt.FileBrowsing.ui.SettingsHelper;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer;
import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;

import java.io.File;

/**
 * Every FileViewer instance interacts with the file system through FileBrowser
 * FileBrowser:
 * -keeps references to important directories
 * -keeps track of the current directory
 * -monitors files for changes
 * -sends updated file lists for FileViewer to display
 * -takes commands from FileViewer and executes them
 */

public class FileBrowser {

    private File currentPath;
    private FileViewer fileViewer;
    private FileObserver fileChangeMonitor;

    public static final File parentDirectory = new File("..");
    public static final File root = new File("/");
    public static final File homeDirectory = Environment.getExternalStorageDirectory();

    public FileBrowser(Context context) {
        currentPath = new File(SettingsHelper.getStartDirectory(context));
        monitorCurrentPathForChanges();
    }
    public void setFileViewer(FileViewer fileViewer) {
        this.fileViewer = fileViewer;
        updateFileViewer();
    }
    private void updateFileViewer() {
        File[] files = currentPath.listFiles();
        if (files == null) {
            files = new File[]{};
        }
        fileViewer.setFileList(files);
    }
    private void monitorCurrentPathForChanges() {
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
        fileChangeMonitor.startWatching();
    }
    public File getCurrentPath() {
        return currentPath;
    }

    //change the displayed file path to newPath if it is a directory. If the new path is ".." then change the current path to the parent of the current directory.
    public void setCurrentPath(File newPath) {
        if(!newPath.isDirectory()) {
            return;
        }
        if (newPath == parentDirectory) {
            currentPath = currentPath.getParentFile();
        } else {
            currentPath = newPath;
        }
        monitorCurrentPathForChanges();
        updateFileViewer();
    }

    public void modifyFile(Bundle args) {
        Intent intent = new Intent(fileViewer.getContext(), FileModifierService.class);
        intent.putExtra(FileModifierService.FILEMODIFIERSERVICE_ARGS, args);
        fileViewer.getContext().startService(intent);
    }
}
