package com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog;

import android.app.Dialog;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.text.InputType;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;

import com.dewdrop623.androidcrypt.FileBrowsing.ui.MainActivity;
import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidcrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidcrypt.FileOperations.operator.AESCryptEncryptFileOperator;
import com.dewdrop623.androidcrypt.R;

import java.io.File;

/**
 * decrypts files
 */

public class DebugDecryptFileDialog extends FileDialog {

    private EditText passwordEditText;
    private CheckBox showPasswordCheckbox;
    private EditText fileDestinationDirectoryEditText;
    private EditText fileNameEditText;
    private Button decryptButton;

    private File fileDesinationDirectory;

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        View view = inflateLayout(R.layout.dialogfragment_debug_decrypt_file);
        passwordEditText = (EditText) view.findViewById(R.id.passwordEditText);
        showPasswordCheckbox = (CheckBox) view.findViewById(R.id.showPasswordCheckbox);
        fileDestinationDirectoryEditText = (EditText) view.findViewById(R.id.fileDestinationDirectoryEditText);
        fileNameEditText = (EditText) view.findViewById(R.id.fileNameEditText);
        decryptButton = (Button) view.findViewById(R.id.decryptButton);
        decryptButton.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                positiveButtonOnClick();
            }
        });

        showPasswordCheckbox.setOnCheckedChangeListener(showPasswordCheckBoxOnCheckedChangeListener);
        setShowPassword(false);
        fileDestinationDirectoryEditText.setText(file.getParent());
        if (file.getName().substring(file.getName().lastIndexOf('.')).equals(".aes")) {
            fileNameEditText.setText(file.getName().substring(0,file.getName().lastIndexOf('.')));
        }

        return createDialog(getString(R.string.decrypt)+" "+file.getName(), view, null);
    }


    private CheckBox.OnCheckedChangeListener showPasswordCheckBoxOnCheckedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
            setShowPassword(b);
        }
    };

    private void setShowPassword(boolean showPassword) {
        int inputType;
        if (showPassword) {
            inputType = InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;
        } else {
            inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD;
        }
        passwordEditText.setInputType(inputType);
    }

    @Override
    protected void positiveButtonOnClick() {
        super.positiveButtonOnClick();
        String password = passwordEditText.getText().toString();
        fileDesinationDirectory = new File(fileDestinationDirectoryEditText.getText().toString());
        if(password.length()==0) {
            ((MainActivity)getActivity()).showToast(getString(R.string.password)+" "+getString(R.string.cannot_be_empty));
            return;
        }
        String fileName = fileNameEditText.getText().toString();
        if(fileName.length()==0) {
            ((MainActivity)getActivity()).showToast(getString(R.string.file_name)+" "+getString(R.string.cannot_be_empty));
            return;
        }
        if (!fileDesinationDirectory.exists()) {
            ((MainActivity) getActivity()).showToast(getString(R.string.destination_does_not_exist));
            return;
        }
        if (!fileDesinationDirectory.isDirectory()) {
            ((MainActivity)getActivity()).showToast(getString(R.string.destination_is_not_a_directory));
            return;
        }
        Bundle args = new Bundle();
        args.putString(AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT, password);
        args.putString(AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_OUTPUT_FILE_ARGUMENT, fileDesinationDirectory.getAbsolutePath()+"/"+fileName);
        args.putString(FileModifierService.FILEMODIFIERSERVICE_FILE, file.getAbsolutePath());
        args.putInt(FileModifierService.FILEMODIFIERSERVICE_OPERATIONTYPE, FileOperationType.DECRYPT);
        fileViewer.sendFileCommandToFileBrowser(args);
        dismiss();
    }
}
