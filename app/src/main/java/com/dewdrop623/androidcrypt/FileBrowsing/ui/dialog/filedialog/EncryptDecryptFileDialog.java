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
import android.widget.ImageButton;

import com.dewdrop623.androidcrypt.FileBrowsing.ui.MainActivity;
import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidcrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidcrypt.FileOperations.operator.AESCryptEncryptFileOperator;
import com.dewdrop623.androidcrypt.R;

import java.io.File;

/**
 * The file dialog that appears when encrypting and decrypting files. Takes passwords, file input, file output directory, file output name.
 * Checks that the destination exists and that the password field is not empty.
 * Sends command to fileViewer instance
 */

public class EncryptDecryptFileDialog extends FileDialog {

    public static final String DECRYPT_MODE_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.EncryptDecryptFileDialog.DECRYPT_MODE_KEY";
    public static final String PASSWORD_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.EncryptDecryptFileDialog.PASSWORD_KEY";
    public static final String CONFIRM_PASSWORD_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.EncryptDecryptFileDialog.CONFIRM_PASSWORD_KEY";
    public static final String SHOW_PASSWORD_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.EncryptDecryptFileDialog.SHOW_PASSWORD_KEY";
    public static final String OUTPUT_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.EncryptDecryptFileDialog.OUTPUT_DIRECTORY_KEY";
    public static final String OUTPUT_FILENAME_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.EncryptDecryptFileDialog.OUTPUT_FILENAME_KEY";

    private EditText passwordEditText;
    private EditText confirmPasswordEditText;
    private CheckBox showPasswordCheckbox;
    private EditText fileDestinationDirectoryEditText;
    private ImageButton selectDirectoryButton;
    private EditText fileNameEditText;
    private Button encryptDecryptButton;

    private boolean decryptMode = false;

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {

        View view = inflateLayout(R.layout.dialog_fragment_encrypt_decrypt_file);
        passwordEditText = (EditText) view.findViewById(R.id.passwordEditText);
        confirmPasswordEditText = (EditText) view.findViewById(R.id.confirmPasswordEditText);
        showPasswordCheckbox = (CheckBox) view.findViewById(R.id.showPasswordCheckbox);
        fileDestinationDirectoryEditText = (EditText) view.findViewById(R.id.fileDestinationDirectoryEditText);
        selectDirectoryButton = (ImageButton) view.findViewById(R.id.selectDirectoryButton);
        fileNameEditText = (EditText) view.findViewById(R.id.fileNameEditText);
        encryptDecryptButton = (Button) view.findViewById(R.id.encryptButton);
        encryptDecryptButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                positiveButtonOnClick();
            }
        });
        showPasswordCheckbox.setOnCheckedChangeListener(showPasswordCheckBoxOnCheckedChangeListener);
        selectDirectoryButton.setOnClickListener(selectDirectoryButtonOnClickListener);

        Bundle args = getArguments();
        decryptMode = args.getBoolean(DECRYPT_MODE_KEY, false);
        passwordEditText.setText(args.getString(PASSWORD_KEY, ""));
        confirmPasswordEditText.setText(args.getString(CONFIRM_PASSWORD_KEY,""));
        showPasswordCheckbox.setChecked(args.getBoolean(SHOW_PASSWORD_KEY,false));
        setShowPassword(showPasswordCheckbox.isChecked());
        fileDestinationDirectoryEditText.setText(args.getString(OUTPUT_DIRECTORY_KEY, file.getParent()));
        String outputFilenameValue = args.getString(OUTPUT_FILENAME_KEY, "");
        if (outputFilenameValue.isEmpty()) {
            if (decryptMode) {
                if (file.getName().substring(file.getName().lastIndexOf('.')).equals(".aes")) {
                    outputFilenameValue = file.getName().substring(0, file.getName().lastIndexOf('.'));
                }
            } else {
                outputFilenameValue = file.getName() + ".aes";
            }
        }
        fileNameEditText.setText(outputFilenameValue);
        return createDialog(R.string.encrypt+" "+file.getName(), view, null);
    }
    /*
    * ANONYMOUS CLASSES
     */
    private CheckBox.OnCheckedChangeListener showPasswordCheckBoxOnCheckedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
            setShowPassword(b);
        }
    };
    private View.OnClickListener selectDirectoryButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            fileViewer.selectEncryptDecryptOutputDirectory(getDialogState());
            dismiss();
        }
    };
    /*
    * PROTECTED METHODS
    * */

    //Overode method from FileDialog. checks that the passwords are non-empty, the confirmed password is ok, that there is an output filename and the output directory exists, and then sends a command to fileViewer
    @Override
    protected void positiveButtonOnClick() {
        super.positiveButtonOnClick();

        //get password, file name, and destination directory from ui
        String password1 = passwordEditText.getText().toString();
        File fileDesinationDirectory = new File(fileDestinationDirectoryEditText.getText().toString());
        String fileName = fileNameEditText.getText().toString();

        if (!decryptMode) {
            String password2 = confirmPasswordEditText.getText().toString();
            if (!password1.equals(password2)) {
                ((MainActivity) getActivity()).showToast(getString(R.string.no_match));
                return;
            }
        }

        if(password1.isEmpty()) {
            ((MainActivity)getActivity()).showToast(getString(R.string.password)+" "+getString(R.string.cannot_be_empty));
            return;
        }

        if(fileName.length()==0){
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
        //send command
        Bundle args = new Bundle();
        args.putString(AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_KEY_ARGUMENT,password1);
        args.putString(AESCryptEncryptFileOperator.AESCRYPT_FILE_OPERATOR_OUTPUT_FILE_ARGUMENT, fileDesinationDirectory.getAbsolutePath()+"/"+fileName);
        args.putString(FileModifierService.FILEMODIFIERSERVICE_FILE, file.getAbsolutePath());
        args.putInt(FileModifierService.FILEMODIFIERSERVICE_OPERATIONTYPE, decryptMode?FileOperationType.DECRYPT:FileOperationType.ENCRYPT);
        fileViewer.sendFileCommandToFileBrowser(args);
        dismiss();
    }

    /*
    * PRIVATE METHODS
     */

    //Change the input method on the EditText views to either show or hide the password.
    private void setShowPassword(boolean showPassword) {
        int inputType;
        if (showPassword) {
            inputType = InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;
        } else {
            inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD;
        }
        passwordEditText.setInputType(inputType);
        confirmPasswordEditText.setInputType(inputType);
    }

    //creates a bundle that stores the current state of the dialog for rotations and so it comes back when selectDirectoryButton is used
    private Bundle getDialogState() {
        Bundle dialogState = new Bundle();
        dialogState.putString(PATH_ARGUMENT, file.getAbsolutePath());
        dialogState.putBoolean(DECRYPT_MODE_KEY, decryptMode);
        dialogState.putString(PASSWORD_KEY, passwordEditText.getText().toString());
        dialogState.putString(CONFIRM_PASSWORD_KEY, confirmPasswordEditText.getText().toString());
        dialogState.putBoolean(SHOW_PASSWORD_KEY, showPasswordCheckbox.isChecked());
        dialogState.putString(OUTPUT_DIRECTORY_KEY, fileDestinationDirectoryEditText.getText().toString());
        dialogState.putString(OUTPUT_FILENAME_KEY, fileNameEditText.getText().toString());
        return dialogState;
    }

    /*
    * PUBLIC METHODS
     */
    //remember whether this dialog is being used to encrypt or decrypt when the screen rotates
    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putBoolean(DECRYPT_MODE_KEY, decryptMode);
        super.onSaveInstanceState(outState);
    }

    //setDecryptMode in onResume instead of onCreateDialog because setDecryptMode needs getDialog to not be null
    @Override
    public void onResume() {
        super.onResume();
        setDecryptMode(decryptMode);
    }

    //set the decrypt mode member variable and adjusts the UI accordingly
    public void setDecryptMode(boolean decryptMode) {
        this.decryptMode = decryptMode;
        if (decryptMode) {
            encryptDecryptButton.setText(R.string.decrypt);
            confirmPasswordEditText.setVisibility(View.GONE);
            getDialog().setTitle(getString(R.string.decrypt)+" "+file.getName());
        } else {
            encryptDecryptButton.setText(R.string.encrypt);
            confirmPasswordEditText.setVisibility(View.VISIBLE);
            getDialog().setTitle(getString(R.string.encrypt)+" "+file.getName());
        }
    }
}
