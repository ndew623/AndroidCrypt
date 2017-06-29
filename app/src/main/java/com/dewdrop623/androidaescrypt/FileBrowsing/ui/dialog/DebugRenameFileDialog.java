package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.app.Dialog;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.view.View;
import android.widget.EditText;

import com.dewdrop623.androidaescrypt.FileOperations.FileCommand;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileRenameOperator;
import com.dewdrop623.androidaescrypt.R;

import java.util.HashMap;

/**
 * ask user for filename
 */

public class DebugRenameFileDialog extends FileDialog {

    EditText fileNameEditText;

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        View view = inflateLayout(R.layout.dialogfragment_debug_rename_file);
        fileNameEditText = (EditText) view.findViewById(R.id.fileNameEditText);
        return createDialog(getString(R.string.rename)+" "+file.getName(), view, getString(R.string.ok));
    }

    @Override
    protected void positiveButtonOnClick() {
        super.positiveButtonOnClick();
        HashMap<String,String> args = new HashMap<>();
        args.put(FileRenameOperator.NEW_FILENAME_ARG, fileNameEditText.getText().toString());
        FileCommand fileCommand = new FileCommand(file, FileOperationType.RENAME, args);
        fileViewer.sendFileCommandToFileBrowser(fileCommand);
    }
}
