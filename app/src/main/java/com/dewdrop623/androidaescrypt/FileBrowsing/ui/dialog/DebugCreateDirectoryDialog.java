package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.app.Dialog;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import com.dewdrop623.androidaescrypt.FileOperations.FileCommand;
import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.FileOperations.operator.folder.CreateFolderOperator;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * get directory name and create a directory
 */

public class DebugCreateDirectoryDialog extends FileDialog{

    private EditText folderNameEditText;

    public Dialog onCreateDialog(Bundle savedInstanceState) {
        View view = inflateLayout(R.layout.dialogfragment_debug_create_directory);
        folderNameEditText = (EditText) view.findViewById(R.id.folderNameEditText);
        return createDialog(getString(R.string.create_folder), view, getString(R.string.create));
    }

    @Override
    protected void positiveButtonOnClick() {
        super.positiveButtonOnClick();
        String folderName = folderNameEditText.getText().toString();
        if (folderName.length() == 0) {
            //TODO give error to user, toast maybe
            return;
        }
        Bundle args = new Bundle();
        args.putString(CreateFolderOperator.CREATE_FOLDER_OPERATOR_FOLDER_NAME, folderName);
        args.putInt(FileModifierService.FILEMODIFIERSERVICE_OPERATIONTYPE, FileOperationType.CREATE_FOLDER);
        args.putString(FileModifierService.FILEMODIFIERSERVICE_FILE, file.getAbsolutePath());
        fileViewer.sendFileCommandToFileBrowser(args);
    }
}