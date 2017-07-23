package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

import android.os.Bundle;
import android.support.v4.app.Fragment;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;

import java.io.File;

/**
 * intended to be extended by other file viewers, each with a different UI for file browsing
 */

public abstract class FileViewer extends Fragment{
    protected File[] fileList;
    protected FileBrowser fileBrowser;
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
    protected void onMoveOrCopy(File file) {

    }
}
