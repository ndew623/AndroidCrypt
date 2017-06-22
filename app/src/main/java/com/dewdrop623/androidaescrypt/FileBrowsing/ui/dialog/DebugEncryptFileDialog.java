package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

import android.app.Dialog;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.dewdrop623.androidaescrypt.FileOperations.FileCommand;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.FileOperations.operator.AESCryptEncryptFileOperator;
import com.dewdrop623.androidaescrypt.MainActivity;
import com.dewdrop623.androidaescrypt.R;

import java.util.HashMap;

import es.vocali.util.AESCrypt;

/**
 * get password and filename from user to encrypt file
 */

public class DebugEncryptFileDialog extends FileDialog {

    private EditText passwordEditText;
    private EditText confirmPasswordEditText;
    private EditText fileNameEditText;
    private Button encryptButton;

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        View view = inflateLayout(R.layout.dialogfragment_debug_encrypt_file);
        passwordEditText = (EditText) view.findViewById(R.id.passwordEditText);
        confirmPasswordEditText = (EditText) view.findViewById(R.id.confirmPasswordEditText);
        fileNameEditText = (EditText) view.findViewById(R.id.fileNameEditText);
        encryptButton = (Button) view.findViewById(R.id.encryptButton);
        encryptButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                positiveButtonOnClick();
            }
        });

        fileNameEditText.setText(file.getName()+".aes");

        return createDialog(R.string.encrypt+" "+file.getName(), view, null);
    }

    //TODO check if file exists
    @Override
    protected void positiveButtonOnClick() {
        super.positiveButtonOnClick();
        String password1 = passwordEditText.getText().toString();
        String password2 = confirmPasswordEditText.getText().toString();
        if (!password1.equals(password2)) {
            ((MainActivity)getActivity()).showToast(getString(R.string.no_match));
            return;
        }
        if(password1.isEmpty()) {
            ((MainActivity)getActivity()).showToast(getString(R.string.password)+" "+getString(R.string.cannot_be_empty));
            return;
        }
        String fileName = fileNameEditText.getText().toString();
        if(fileName.length()==0){
            ((MainActivity)getActivity()).showToast(getString(R.string.file_name)+" "+getString(R.string.cannot_be_empty));
            return;
        }
        HashMap<String, String> args = new HashMap<>();
        args.put(AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT,password1);
        args.put(AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_FILENAME_ARGUMENT, fileName);
        FileCommand fileCommand = new FileCommand(file, FileOperationType.ENCRYPT, args);
        fileBrowser.modifyFile(fileCommand);
        dismiss();
    }
}
