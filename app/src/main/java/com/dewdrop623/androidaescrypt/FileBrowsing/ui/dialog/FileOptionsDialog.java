package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.util.Log;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * base class for Dialogs that display options for files
 */

public class FileOptionsDialog extends DialogFragment {
    public static final String FILE_PATH_ARGUMENT = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.FILENAME_ARGUMENT";
    protected FileBrowser fileBrowser = null;
    protected File file;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Bundle args = getArguments();
        file = new File(args.getString(FILE_PATH_ARGUMENT));
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle(file.getName());
        builder.setNegativeButton(R.string.cancel, null);
        builder.setPositiveButton(R.string.delete, positiveOnClickListener);
        return builder.create();
    }
    public void setFileBrowser(FileBrowser fileBrowser) {
        this.fileBrowser = fileBrowser;
    }
    private Dialog.OnClickListener positiveOnClickListener = new Dialog.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
            positiveButtonOnClickBehavior();
        }
    };
    protected void positiveButtonOnClickBehavior() {
        if (fileBrowser == null) {
            Log.e("DebugFileOptionDialog", "You must call fileOptionsDialog.setFileBrowser(FileBrowser)");
            return;
        }
        return;
    }
}
