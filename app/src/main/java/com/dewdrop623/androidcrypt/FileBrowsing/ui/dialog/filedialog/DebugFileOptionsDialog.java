package com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog;


import android.app.Dialog;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.dewdrop623.androidcrypt.FileBrowsing.ui.MainActivity;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.view.CaptionedImageButton;
import com.dewdrop623.androidcrypt.FileOperations.FileUtils;
import com.dewdrop623.androidcrypt.R;

import java.text.DateFormat;

/**
 * appears when a file is selected and makes options available
 */

public class DebugFileOptionsDialog extends FileDialog {
    TextView fileAttributesTextView;
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
        fileAttributesTextView = (TextView) view.findViewById(R.id.fileAttributesTextView);
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

        String attributesString = "";
        attributesString += getText(R.string.parent_folder)+": "+file.getParent()+"\n";
        if (!file.isDirectory()) {
            attributesString += getText(R.string.file_size) + ": "+ FileUtils.humanReadableBytes(file.length())+"\n";
        }
        attributesString += getText(R.string.last_modified)+": "+ DateFormat.getDateInstance().format(file.lastModified())+" "+DateFormat.getTimeInstance().format(file.lastModified())+"\n";
        attributesString += getText(R.string.readable)+": "+(file.canRead()?"Yes":"No")+"\n";
        attributesString += getText(R.string.writable)+": "+(file.canWrite()?"Yes":"No");
        fileAttributesTextView.setText(attributesString);

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
                showNewDialogAndDismiss(new EncryptDecryptFileDialog());
            } else if (v.getId() == decryptButton.getId()) {
                Bundle decryptArgs = new Bundle();
                decryptArgs.putBoolean(EncryptDecryptFileDialog.DECRYPT_MODE_KEY, true);
                showNewDialogAndDismiss(new EncryptDecryptFileDialog(), decryptArgs);
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
