package com.dewdrop623.androidcrypt;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.text.InputType;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

/**
 * A placeholder fragment containing a simple view.
 */
public class MainActivityFragment extends Fragment {

    private static final int SELECT_INPUT_FILE_REQUEST_CODE = 623;
    private static final int SELECT_OUTPUT_DIRECTORY_REQUEST_CODE = 8878;
    private static final int WRITE_FILE_PERMISSION_REQUEST_CODE = 440;

    private boolean operationType = CryptoThread.OPERATION_TYPE_ENCRYPTION;
    private Uri inputFileUri = null;
    private Uri outputFileUri = null;

    private Button encryptModeButton;
    private Button decryptModeButton;
    private TextView inputContentURITextView;
    private FileSelectButton inputFileSelectButton;
    private FileSelectButton outputFileSelectButton;
    private EditText passwordEditText;
    private TextView confirmPasswordTextView;
    private EditText confirmPasswordEditText;
    private CheckBox showPasswordCheckbox;
    private TextView outputContentURITextView;

    public MainActivityFragment() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_main, container, false);

        encryptModeButton = (Button) view.findViewById(R.id.encryptModeButton);
        decryptModeButton = (Button) view.findViewById(R.id.decryptModeButton);
        inputContentURITextView = (TextView) view.findViewById(R.id.inputContentURITextView);
        inputFileSelectButton = (FileSelectButton) view.findViewById(R.id.selectInputFileButton);
        outputFileSelectButton = (FileSelectButton) view.findViewById(R.id.selectOutputFileButton);
        passwordEditText = (EditText) view.findViewById(R.id.passwordEditText);
        confirmPasswordTextView = (TextView) view.findViewById(R.id.confirmPasswordTextView);
        confirmPasswordEditText = (EditText) view.findViewById(R.id.confirmPasswordEditText);
        showPasswordCheckbox = (CheckBox) view.findViewById(R.id.showPasswordCheckbox);
        outputContentURITextView = (TextView) view.findViewById(R.id.outputContentURITextView);

        showPasswordCheckbox.setOnCheckedChangeListener(showPasswordCheckBoxOnCheckedChangeListener);
        outputFileSelectButton.setOnClickListener(outputFileSelectButtonOnClickListener);
        inputFileSelectButton.setOnClickListener(inputFileSelectButtonOnClickListener);
        encryptModeButton.setOnClickListener(operationModeButtonsOnClickListener);
        decryptModeButton.setOnClickListener(operationModeButtonsOnClickListener);

        setShowPassword(false);

        checkPermissions();

        return view;
    }

    /**
     * onCreateView apparently runs before MainActivity initializes its views in MainActivity.onCreate()
     * this is an issue because the OnCheckedChangeListener for the encryption/decryption RadioGroup changes the icon on the Floating Action Button,
     * which is one of MainActivity's views
     * therefore, this method can be called by MainActivity at the end of its onCreate() method
     */
    public void onPostMainActivityOnCreate() {
        //set the default mode
        enableEncryptionMode();
    }

    private View.OnClickListener operationModeButtonsOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            switch (view.getId()) {
                case R.id.encryptModeButton:
                    enableEncryptionMode();
                    break;
                case R.id.decryptModeButton:
                    enableDecryptionMode();
                    break;
            }
        }
    };

    private CheckBox.OnCheckedChangeListener showPasswordCheckBoxOnCheckedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
            setShowPassword(b);
        }
    };

    private View.OnClickListener inputFileSelectButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            selectInputFile();
        }
    };

    private View.OnClickListener outputFileSelectButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            selectOutputFile();
        }
    };

    /**
     * ask StorageAccessFramework to allow user to pick a file
     */
    private void selectInputFile() {
        StorageAccessFrameworkHelper.safPickFile(this, SELECT_INPUT_FILE_REQUEST_CODE);
    }

    /**
     * ask StorageAccessFramework to allow user to pick a directory
     */
    private void selectOutputFile() {
        StorageAccessFrameworkHelper.safPickDirectory(this, SELECT_OUTPUT_DIRECTORY_REQUEST_CODE);
    }

    /*
    * Set the inputFileUri or outputFileUri member variable and change UI. Pass null to clear the uri value and reset ui.
     */
    private void setContentUri(Uri uri, boolean output) {
        TextView contentURITextView;
        FileSelectButton fileSelectButton;

        if (output) {
            outputFileUri = uri;
            contentURITextView = outputContentURITextView;
            fileSelectButton = outputFileSelectButton;
        } else {
            inputFileUri = uri;
            contentURITextView = inputContentURITextView;
            fileSelectButton = inputFileSelectButton;
        }

        String contentURIText = "";
        int contentURITextViewVisibility = View.GONE;
        boolean fileSelectButtonMinimize = false;
        if (uri != null) {
            contentURIText = uri.getLastPathSegment();
            contentURITextViewVisibility = View.VISIBLE;
            fileSelectButtonMinimize = true;
        }

        contentURITextView.setText(contentURIText);
        contentURITextView.setVisibility(contentURITextViewVisibility);
        fileSelectButton.setMinimized(fileSelectButtonMinimize);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == SELECT_INPUT_FILE_REQUEST_CODE && resultCode == Activity.RESULT_OK) {
            setContentUri(data.getData(), false);
        } else if (requestCode == SELECT_OUTPUT_DIRECTORY_REQUEST_CODE) {
            setContentUri(data.getData(), true);
        } else {
            showError(R.string.error_unexpected_response_from_saf);
        }
    }

    /**
     * called by MainActivity when the Floating Action Button is pressed.
     */
    public void actionButtonPressed() {
        if (inputFileUri == null || outputFileUri == null) {
            return ;
        }
        Intent intent = new Intent(getContext(), CryptoService.class);
        intent.putExtra(CryptoService.INPUT_URI_EXTRA_KEY, inputFileUri.toString());
        intent.putExtra(CryptoService.OUTPUT_URI_EXTRA_KEY, outputFileUri.toString());
        //TODO probably major security flaw to put encryption keys in an intent. research and change if necessary.
        intent.putExtra(CryptoService.PASSWORD_EXTRA_KEY, passwordEditText.getText().toString());
        intent.putExtra(CryptoService.VERSION_EXTRA_KEY, CryptoThread.VERSION_2);
        intent.putExtra(CryptoService.OPERATION_TYPE_EXTRA_KEY, operationType);
        getContext().startService(intent);
    }

    //check for the necessary permissions. destroy and recreate the activity if permissions are asked for so that the files (which couldn't be seen previously) will be displayed
    private void checkPermissions() {
        if(ContextCompat.checkSelfPermission(getContext(), android.Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED ) {
            ActivityCompat.requestPermissions(getActivity(), new String[] {android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_FILE_PERMISSION_REQUEST_CODE);
        }
    }

    /*
    * Display an error to the user.
    * */
    private void showError(String error) {
        Toast.makeText(getContext(), error, Toast.LENGTH_SHORT).show();
    }

    /*
    * Display an error to the user.
    * */
    private void showError(int stringId) {
        Toast.makeText(getContext(), stringId, Toast.LENGTH_SHORT).show();
    }

    private MainActivityFragment referenceToThisForAnonymousClassButtonListeners() {
        return this;
    }

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

    /**
     * Makes encryption mode active.
     * Shows the confirm password entry field, changes the member variable operationType, and changes the icon on the Floating Action Button
     */
    private void enableEncryptionMode() {
        changeOperationTypeButtonAppearance(R.drawable.operation_mode_button_selected, R.drawable.operation_mode_button_selector);
        operationType = CryptoThread.OPERATION_TYPE_ENCRYPTION;
        confirmPasswordTextView.setVisibility(View.VISIBLE);
        confirmPasswordEditText.setVisibility(View.VISIBLE);
        ((MainActivity)getActivity()).setFABIcon(R.drawable.ic_lock);
    }

    /**
     * Makes decryption mode active.
     * Hides the confirm password entry field, changes the member variable operationType, and changes the icon on the Floating Action Button
     */
    private void enableDecryptionMode() {
        changeOperationTypeButtonAppearance(R.drawable.operation_mode_button_selector, R.drawable.operation_mode_button_selected);
        operationType = CryptoThread.OPERATION_TYPE_DECRYPTION;
        confirmPasswordTextView.setVisibility(View.INVISIBLE);
        confirmPasswordEditText.setVisibility(View.INVISIBLE);
        ((MainActivity)getActivity()).setFABIcon(R.drawable.ic_unlock);
    }

    /*
    * Used to change the highlighting on the buttons when changing between encryption and decryption modes.
     */
    private void changeOperationTypeButtonAppearance(int encryptionDrawableId, int decryptionDrawableId) {
        encryptModeButton.setBackground(ResourcesCompat.getDrawable(getResources(), encryptionDrawableId, null));
        decryptModeButton.setBackground(ResourcesCompat.getDrawable(getResources(), decryptionDrawableId, null));
    }

    /*
    * check if user input is valid before trying to do an operation, return true if it is. Also, notify users of any issues.
    * */
    private boolean validationCheck() {
        boolean valid = true;
        if (inputFileUri == null) {
            showError(R.string.no_input_file_selected);
            valid = false;
        }
        return valid;
    }
}
