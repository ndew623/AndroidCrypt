package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;


import com.dewdrop623.androidaescrypt.FileOperations.FileCommand;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;

/**
 * appears when a file is selected and make options available
 */

public class DebugFileOptionsDialog extends FileOptionsDialog {
    @Override
    protected void positiveButtonOnClickBehavior()  {
        super.positiveButtonOnClickBehavior();
        FileCommand fileCommand = new FileCommand(file, FileOperationType.DELETE);
        fileBrowser.modifyFile(fileCommand);
    }
}
