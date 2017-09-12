package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog;


import android.app.Dialog;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.dewdrop623.androidaescrypt.FileBrowsing.ui.MainActivity;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.view.CaptionedImageButton;
import com.dewdrop623.androidaescrypt.R;

/**
 * appears when a file is selected and makes options available
 */

public class DebugFileOptionsDialog extends FileDialog {
    CaptionedImageButton encryptButton;
    CaptionedImageButton decryptButton;
    CaptionedImageButton copyButton;
    CaptionedImageButton moveButton;
    CaptionedImageButton renameButton;
    CaptionedImageButton deleteButton;
    CaptionedImageButton favoriteButton;
    CaptionedImageButton unFavoriteButton;
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        View view = inflateLayout(R.layout.dialogfragment_debug_file_options);
        encryptButton = (CaptionedImageButton) view.findViewById(R.id.encryptButton);
        decryptButton = (CaptionedImageButton) view.findViewById(R.id.decryptButton);
        copyButton = (CaptionedImageButton) view.findViewById(R.id.copyButton);
        moveButton = (CaptionedImageButton) view.findViewById(R.id.moveButton);
        renameButton = (CaptionedImageButton) view.findViewById(R.id.renameButton);
        deleteButton = (CaptionedImageButton) view.findViewById(R.id.deleteButton);
        favoriteButton = (CaptionedImageButton) view.findViewById(R.id.favoriteButton);
        unFavoriteButton = (CaptionedImageButton) view.findViewById(R.id.unfavoriteButton);

        encryptButton.setOnClickListener(buttonOnClickListener);
        decryptButton.setOnClickListener(buttonOnClickListener);
        copyButton.setOnClickListener(buttonOnClickListener);
        moveButton.setOnClickListener(buttonOnClickListener);
        renameButton.setOnClickListener(buttonOnClickListener);
        deleteButton.setOnClickListener(buttonOnClickListener);
        favoriteButton.setOnClickListener(buttonOnClickListener);
        unFavoriteButton.setOnClickListener(buttonOnClickListener);

        if (file.isDirectory()) {
            encryptButton.setVisibility(View.GONE);
            decryptButton.setVisibility(View.GONE);
        }

        if (((MainActivity)getActivity()).isInFavorites(file)){
            favoriteButton.setVisibility(View.GONE);
        } else {
            unFavoriteButton.setVisibility(View.GONE);
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
            } else if (v.getId() == moveButton.getId()) {
                fileViewer.moveFile(file);
                dismiss();
            } else if (v.getId() == favoriteButton.getId()) {
                ((MainActivity)getActivity()).addFavorite(file.getAbsolutePath());
                dismiss();
            } else if (v.getId() == unFavoriteButton.getId()) {
                ((MainActivity)getActivity()).removeFavorite(file.getAbsolutePath());
                dismiss();
            }
        }
    };
}
