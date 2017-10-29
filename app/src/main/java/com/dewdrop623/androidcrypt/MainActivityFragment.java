package com.dewdrop623.androidcrypt;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.OpenableColumns;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.text.InputType;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

/**
 * A placeholder fragment containing a simple view.
 */
public class MainActivityFragment extends Fragment {

    private static final int SELECT_INPUT_FILE_REQUEST_CODE = 623;
    private static final int SELECT_OUTPUT_DIRECTORY_REQUEST_CODE = 8878;
    private static final int WRITE_FILE_PERMISSION_REQUEST_CODE = 440;

    //stores the type of operation/operation mode to be done
    private boolean operationType = CryptoThread.OPERATION_TYPE_ENCRYPTION;
    private Uri inputFileUri = null;
    private Uri outputFileUri = null;

    private Button encryptModeButton;
    private Button decryptModeButton;
    //TODO either add ability to hide missing files textview, or remove these references
    private LinearLayout missingFilesLinearLayout;
    private ImageButton missingFilesHideImageButton;
    private TextView missingFilesTextView;
    private TextView inputContentURITextView;
    private View inputContentURIUnderlineView;
    private TextView outputContentURITextView;
    private View outputContentURIUnderlineView;
    private FileSelectButton inputFileSelectButton;
    private FileSelectButton outputFileSelectButton;
    private EditText passwordEditText;
    private TextView confirmPasswordTextView;
    private EditText confirmPasswordEditText;
    private CheckBox showPasswordCheckbox;

    public MainActivityFragment() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_main, container, false);

        encryptModeButton = (Button) view.findViewById(R.id.encryptModeButton);
        decryptModeButton = (Button) view.findViewById(R.id.decryptModeButton);
        missingFilesLinearLayout = (LinearLayout) view.findViewById(R.id.missingFilesLinearLayout);
        missingFilesHideImageButton = (ImageButton) view.findViewById(R.id.missingFilesHideImageButton);
        missingFilesTextView = (TextView) view.findViewById(R.id.missingFilesTextView);
        inputContentURITextView = (TextView) view.findViewById(R.id.inputContentURITextView);
        inputContentURIUnderlineView = view.findViewById(R.id.inputContentURIUnderlineView);
        outputContentURITextView = (TextView) view.findViewById(R.id.outputContentURITextView);
        outputContentURIUnderlineView = view.findViewById(R.id.outputContentURIUnderlineView);
        inputFileSelectButton = (FileSelectButton) view.findViewById(R.id.selectInputFileButton);
        outputFileSelectButton = (FileSelectButton) view.findViewById(R.id.selectOutputFileButton);
        passwordEditText = (EditText) view.findViewById(R.id.passwordEditText);
        confirmPasswordTextView = (TextView) view.findViewById(R.id.confirmPasswordTextView);
        confirmPasswordEditText = (EditText) view.findViewById(R.id.confirmPasswordEditText);
        showPasswordCheckbox = (CheckBox) view.findViewById(R.id.showPasswordCheckbox);

        missingFilesTextView.setOnClickListener(missingFilesViewsOnClickListener);
        showPasswordCheckbox.setOnCheckedChangeListener(showPasswordCheckBoxOnCheckedChangeListener);
        outputFileSelectButton.setOnClickListener(outputFileSelectButtonOnClickListener);
        inputFileSelectButton.setOnClickListener(inputFileSelectButtonOnClickListener);
        encryptModeButton.setOnClickListener(operationModeButtonsOnClickListener);
        decryptModeButton.setOnClickListener(operationModeButtonsOnClickListener);

        setShowPassword(false);

        checkPermissions();

        //Hide the keyboard that automatically pops up.
        getActivity().getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);

        return view;
    }

    /**
     * onCreateView apparently runs before MainActivity initializes its views in MainActivity.onCreate()
     * this is an issue because enableEncryptionMode() changes the icon on the Floating Action Button,
     * which is one of MainActivity's views
     * therefore, this method can be called by MainActivity at the end of its onCreate() method
     */
    public void onPostMainActivityOnCreate() {
        //set the default mode
        enableEncryptionMode();
    }

    /*
    * This onClickListener is for click on either the textview or hide button of the missing files help textview.
     */
    private View.OnClickListener missingFilesViewsOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            switch (view.getId()) {
                case R.id.missingFilesHideImageButton:
                    break;
                case R.id.missingFilesTextView:
                    ((MainActivity)getActivity()).showMissingFilesHelpDialog();
                    break;
            }
        }
    };

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
        StorageAccessFrameworkHelper.safPickOutputFile(this, SELECT_OUTPUT_DIRECTORY_REQUEST_CODE, getDefaultOutputFileName());
    }

    /*
    * Set the inputFileUri or outputFileUri member variable and change UI. Pass null to clear the uri value and reset ui.
     */
    private void setContentUri(Uri uri, boolean output) {
        TextView contentURITextView;
        FileSelectButton fileSelectButton;
        View contentURIUnderlineView;

        if (output) {
            outputFileUri = uri;
            contentURITextView = outputContentURITextView;
            fileSelectButton = outputFileSelectButton;
            contentURIUnderlineView = outputContentURIUnderlineView;
        } else {
            inputFileUri = uri;
            contentURITextView = inputContentURITextView;
            fileSelectButton = inputFileSelectButton;
            contentURIUnderlineView = inputContentURIUnderlineView;
        }

        String contentURIText = "";
        int contentURITextViewVisibility = View.GONE;
        boolean fileSelectButtonMinimize = false;
        if (uri != null) {
            contentURIText = getFileNameFromUri(uri);
            contentURITextViewVisibility = View.VISIBLE;
            fileSelectButtonMinimize = true;
        }

        contentURITextView.setText(contentURIText);
        contentURITextView.setVisibility(contentURITextViewVisibility);
        fileSelectButton.setMinimized(fileSelectButtonMinimize);
        contentURIUnderlineView.setVisibility(contentURITextViewVisibility);
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
        showError(getString(stringId));
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
        confirmPasswordTextView.setVisibility(View.GONE);
        confirmPasswordEditText.setVisibility(View.GONE);
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

    /*thank you stack overflow*/
    private String getFileNameFromUri(Uri uri) {
        String result = null;
        if (uri.getScheme().equals("content")) {
            Cursor cursor = getActivity().getContentResolver().query(uri, null, null, null, null);
            try {
                if (cursor != null && cursor.moveToFirst()) {
                    result = cursor.getString(cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME));
                }
            } finally {
                cursor.close();
            }
        }
        if (result == null) {
            result = uri.getPath();
            int cut = result.lastIndexOf('/');
            if (cut != -1) {
                result = result.substring(cut + 1);
            }
        }
        return result;
    }

    /*return the default output filename based on the inputFileUri.
    *if inputFileUri is null, returns null.
    * if in encryption mode, append '.aes' to filename.
    * if in decryption mode, and input filename ends with '.aes', remove '.aes'
    * if in decryption mode and input filename does not end with '.aes', return empty string*/
    private String getDefaultOutputFileName() {
        String result = null;
        if (inputFileUri != null) {
            String fileName = getFileNameFromUri(inputFileUri);
            if (operationType == CryptoThread.OPERATION_TYPE_ENCRYPTION) {
                result = fileName.concat(".aes");
            } else if (operationType == CryptoThread.OPERATION_TYPE_DECRYPTION) {
                if (fileName.substring(fileName.lastIndexOf('.')).equals(".aes")) {
                    result = fileName.substring(0, fileName.lastIndexOf('.'));
                } else {
                    result = "";
                }
            }
        }
        return result;
    }
}
