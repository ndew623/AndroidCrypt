package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog;


import android.app.Dialog;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;

import com.dewdrop623.androidaescrypt.FileBrowsing.ui.MainActivity;
import com.dewdrop623.androidaescrypt.R;

/**
 * appears when a file is selected and makes options available
 */

public class DebugFileOptionsDialog extends FileDialog {
    Button encryptButton;
    Button decryptButton;
    ImageButton copyButton;
    //Button moveButton;
    Button renameButton;
    Button deleteButton;
    Button addFavoriteButton;
    Button removeFavoriteButton;
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        View view = inflateLayout(R.layout.dialogfragment_debug_file_options);
        encryptButton = (Button) view.findViewById(R.id.encryptButton);
        decryptButton = (Button) view.findViewById(R.id.decryptButton);
        copyButton = (ImageButton) view.findViewById(R.id.copyButton);
        //moveButton = (Button) view.findViewById(R.id.moveButton);
        renameButton = (Button) view.findViewById(R.id.renameButton);
        deleteButton = (Button) view.findViewById(R.id.deleteButton);
        addFavoriteButton = (Button) view.findViewById(R.id.addFavoriteButton);
        removeFavoriteButton = (Button) view.findViewById(R.id.removeFavoriteButton);

        encryptButton.setOnClickListener(buttonOnClickListener);
        decryptButton.setOnClickListener(buttonOnClickListener);
        copyButton.setOnClickListener(buttonOnClickListener);
       // moveButton.setOnClickListener(buttonOnClickListener);
        renameButton.setOnClickListener(buttonOnClickListener);
        deleteButton.setOnClickListener(buttonOnClickListener);
        addFavoriteButton.setOnClickListener(buttonOnClickListener);
        removeFavoriteButton.setOnClickListener(buttonOnClickListener);

        if (file.isDirectory()) {
            encryptButton.setVisibility(View.GONE);
            decryptButton.setVisibility(View.GONE);
        }

        if (((MainActivity)getActivity()).isInFavorites(file)){
            addFavoriteButton.setVisibility(View.GONE);
        } else {
            removeFavoriteButton.setVisibility(View.GONE);
        }

        return createDialog(file.getName(), view, null);
    }
    private View.OnClickListener buttonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (fileViewer == null) {
                Log.e("DebugFileOptionDialog", "You must call fileDialog.setFileBrowser(FileBrowser)");
                return;
            }
            if (v.getId() == deleteButton.getId()) {
                showNewDialogAndDismiss(new DebugDeleteFileDialog());
            } else if (v.getId() == encryptButton.getId()) {
                showNewDialogAndDismiss(new DebugEncryptFileDialog());
            } else if (v.getId() == decryptButton.getId()) {
                showNewDialogAndDismiss(new DebugDecryptFileDialog());
            } else if (v.getId() == renameButton.getId()) {
                showNewDialogAndDismiss(new DebugRenameFileDialog());
            } else if (v.getId() == copyButton.getId()) {
                fileViewer.copyFile(file);
                dismiss();
            }/* else if (v.getId() == moveButton.getId()) {
                fileViewer.moveFile(file);
                dismiss();
            } */else if (v.getId() == addFavoriteButton.getId()) {
                ((MainActivity)getActivity()).addFavorite(file.getAbsolutePath());
                dismiss();
            } else if (v.getId() == removeFavoriteButton.getId()) {
                ((MainActivity)getActivity()).removeFavorite(file.getAbsolutePath());
                dismiss();
            }
        }
    };
}
