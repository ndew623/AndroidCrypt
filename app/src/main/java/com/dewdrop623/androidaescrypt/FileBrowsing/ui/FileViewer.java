package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.Fragment;
import android.util.Log;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;

import java.io.File;

/**
 * intended to be extended by other file viewers, each with a different UI for file browsing
 */

public abstract class FileViewer extends Fragment{
    protected File[] fileList;
    protected FileBrowser fileBrowser;
    protected void onMoveOrCopy(File file) {

    }
    protected void goToHomeDirectory () {
        fileBrowser.changePath(FileBrowser.topLevelInternal);
    }
    public void setFileList(File[] fileList) {
        this.fileList=fileList;
    }
    public void setFileBrowser(FileBrowser fileBrowser) {
        this.fileBrowser=fileBrowser;
    }
    public void sendFileCommandToFileBrowser(Bundle args) {
        fileBrowser.modifyFile(args);
    }
    public void moveFile(File file) {
        onMoveOrCopy(file);
    }
    public void copyFile(File file) {
        onMoveOrCopy(file);
    }
}
