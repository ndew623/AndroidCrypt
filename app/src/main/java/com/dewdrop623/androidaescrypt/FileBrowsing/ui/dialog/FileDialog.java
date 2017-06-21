package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * super class for dialogs that interact with files
 */

public class FileDialog extends DialogFragment {
    public static final String PATH_ARGUMENT = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.PATH_ARGUMENT";
    protected FileBrowser fileBrowser;
    protected File file;

    private EditText inputEditText;

    protected void initFromArguments() {
        file = new File(getArguments().getString(PATH_ARGUMENT));
    }
    protected Dialog createDialog(String title, View view) {
        return createDialog(title, view, null);
    }
    protected Dialog createDialog(String title, View view, String positiveButtonText) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle(title);
        builder.setNegativeButton(R.string.cancel, null);

        if (positiveButtonText != null) {
            builder.setPositiveButton(positiveButtonText, positiveOnClickListener);
        }

        builder.setView(view);
        return builder.create();
    }
    protected View inflateLayout(int layoutId) {
        return ((LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE)).inflate(layoutId, null);
    }

    public void setFileBrowser(FileBrowser fileBrowser) {
        this.fileBrowser = fileBrowser;
    }

    Dialog.OnClickListener positiveOnClickListener = new Dialog.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
            positiveButtonOnClick();
        }
    };

    protected void positiveButtonOnClick() {
        if (fileBrowser == null) {
            Log.e("DebugFileOptionDialog", "You must call fileDialog.setFileBrowser(FileBrowser)");
            return;
        }
        return;
    }

    protected void getInputFromUser(String hint) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        inputEditText = new EditText(getActivity());
        inputEditText.setHint(hint);
        builder.setView(inputEditText);
        builder.setNegativeButton(R.string.cancel, null);
        builder.setPositiveButton(R.string.ok, inputPositiveButtonOnClickListener);
        builder.create().show();
    }
    private DialogInterface.OnClickListener inputPositiveButtonOnClickListener = new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
            inputPositiveButtonOnClick(inputEditText.getText().toString());
        }
    };
    protected void inputPositiveButtonOnClick(String input) {
        return;
    }
}
