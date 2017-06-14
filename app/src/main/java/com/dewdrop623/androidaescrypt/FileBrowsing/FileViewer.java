package com.dewdrop623.androidaescrypt.FileBrowsing;

import android.support.v4.app.Fragment;

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
}
