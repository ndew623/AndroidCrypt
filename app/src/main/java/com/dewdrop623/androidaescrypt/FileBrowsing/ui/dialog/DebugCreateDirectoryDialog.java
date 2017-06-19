package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.FileOperations.FileCommand;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * get directory name and create a directory
 */

public class DebugCreateDirectoryDialog extends DialogFragment{

    public static final String NEW_DIRECTORY_PATH_ARGUMENT = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.NEW_DIRECTORY_PATH_ARGUMENT";
    private String path;
    private FileBrowser fileBrowser;
    private EditText folderNameEditText;

    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Bundle args = getArguments();
        path = args.getString(NEW_DIRECTORY_PATH_ARGUMENT);
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle(R.string.create_folder);
        builder.setNegativeButton(R.string.cancel, null);
        builder.setPositiveButton(R.string.create, positiveOnClickListener);

        View view = ((LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE)).inflate(R.layout.dialogfragment_debug_create_directory, null);
        folderNameEditText = (EditText) view.findViewById(R.id.folderNameEditText);
        builder.setView(view);


        return builder.create();
    }
    public void setFileBrowser(FileBrowser fileBrowser) {
        this.fileBrowser = fileBrowser;
    }
    Dialog.OnClickListener positiveOnClickListener = new Dialog.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
            if (fileBrowser == null) {
                Log.e("DebugFileOptionDialog", "You must call createFolderDialog.setFileBrowser(FileBrowser)");
                return;
            }
            String folderName = folderNameEditText.getText().toString();
            if (folderName.length() == 0) {
                //TODO give error to user, toast maybe
                return;
            }

            File newFolderFile = new File(path+"/"+folderName);
            FileCommand fileCommand = new FileCommand(newFolderFile, FileOperationType.CREATE_FOLDER);
            fileBrowser.modifyFile(fileCommand);
        }
    };

}