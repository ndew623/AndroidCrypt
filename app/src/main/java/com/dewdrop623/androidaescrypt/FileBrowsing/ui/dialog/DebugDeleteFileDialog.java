package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.app.Dialog;
import android.os.Bundle;
import android.support.annotation.NonNull;

import com.dewdrop623.androidaescrypt.FileOperations.FileCommand;
import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.R;

/**
 * confirm that file should be deleted
 */

public class DebugDeleteFileDialog extends FileDialog{
    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        return createDialog(getString(R.string.delete) + " " +file.getName()+"?", null, getString(R.string.delete));
    }

    @Override
    protected void positiveButtonOnClick() {
        super.positiveButtonOnClick();
        Bundle args = new Bundle();
        args.putString(FileModifierService.FILEMODIFIERSERVICE_FILE, file.getAbsolutePath());
        args.putInt(FileModifierService.FILEMODIFIERSERVICE_OPERATIONTYPE, FileOperationType.DELETE);
        fileViewer.sendFileCommandToFileBrowser(args);
    }
}
