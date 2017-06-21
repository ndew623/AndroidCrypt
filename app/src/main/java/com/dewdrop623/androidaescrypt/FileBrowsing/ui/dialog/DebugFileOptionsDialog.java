package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;


import android.app.Dialog;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.dewdrop623.androidaescrypt.FileOperations.FileCommand;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.R;

/**
 * appears when a file is selected and makes options available
 */

public class DebugFileOptionsDialog extends FileDialog {
    Button encryptButton;
    Button deleteButton;
    FileCommand fileCommandWaitingForInput = null;
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        initFromArguments();
        View view = inflateLayout(R.layout.dialogfragment_debug_file_options);
        encryptButton = (Button) view.findViewById(R.id.encryptButton);
        deleteButton = (Button) view.findViewById(R.id.deleteButton);
        encryptButton.setOnClickListener(inputNeededButtonOnClickListener);
        deleteButton.setOnClickListener(noInputButtonOnClickListener);
        return createDialog(file.getName(), view);
    }
    private View.OnClickListener noInputButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (fileBrowser == null) {
                Log.e("DebugFileOptionDialog", "You must call fileDialog.setFileBrowser(FileBrowser)");
                return;
            }
            FileOperationType fileOperationType = FileOperationType.NONE;
            if (v.getId() == deleteButton.getId()) {
                fileOperationType = FileOperationType.DELETE;
            }
            fileBrowser.modifyFile(new FileCommand(file, fileOperationType, null));
            dismiss();
        }
    };
    private View.OnClickListener inputNeededButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (fileBrowser == null) {
                Log.e("DebugFileOptionDialog", "You must call fileDialog.setFileBrowser(FileBrowser)");
                return;
            }
            FileOperationType fileOperationType = FileOperationType.NONE;
            String hint = "";
            if (v.getId() == encryptButton.getId()) {
                fileOperationType = FileOperationType.ENCRYPT;
                hint = getString(R.string.enter_password);
            }
            fileCommandWaitingForInput = new FileCommand(file, fileOperationType, null);
            getInputFromUser(hint);
        }
    };
    @Override
    protected void inputPositiveButtonOnClick(String input) {
        fileCommandWaitingForInput.arg = input;
        fileBrowser.modifyFile(fileCommandWaitingForInput);
        dismiss();
    }
}
