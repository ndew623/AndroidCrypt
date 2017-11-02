package com.dewdrop623.androidcrypt;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.OpenableColumns;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.text.InputType;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
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

import java.util.Arrays;

/**
 * A placeholder fragment containing a simple view.
 */
public class MainActivityFragment extends Fragment {

    //Using static variables to store the password rather than savedInstanceState and Intent extras because of paranoia.
    private static char[] password;

    private static final int SELECT_INPUT_FILE_REQUEST_CODE = 623;
    private static final int SELECT_OUTPUT_DIRECTORY_REQUEST_CODE = 8878;
    private static final int WRITE_FILE_PERMISSION_REQUEST_CODE = 440;

    private static final String SAVED_INSTANCE_STATE_SHOW_PASSWORD="com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_";
    private static final String SAVED_INSTANCE_STATE_OPERATION_MODE="com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_OPERATION_MODE";
    private static final String SAVED_INSTANCE_STATE_INPUT_URI="com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_INPUT_URI";
    private static final String SAVED_INSTANCE_STATE_OUTPUT_URI="com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_OUTPUT_URI";
    private static final String SAVED_INSTANCE_STATE_="com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_";

    //stores the type of operation/operation mode to be done
    private boolean operationMode = CryptoThread.OPERATION_TYPE_ENCRYPTION;
    private boolean hasModeState = false;
    private Uri inputFileUri = null;
    private Uri outputFileUri = null;
    private Bundle stateBundle;

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
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
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

        checkPermissions();

        //Hide the keyboard that automatically pops up.
        getActivity().getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);

        if (stateBundle == null && savedInstanceState != null)
        {
            stateBundle = savedInstanceState;
        }
        restoreFromStateBundle(stateBundle);

        return view;
    }

    //Store the current state when MainActivityFragment is added to back stack.
    //onCreateView will be called when the MainActivityFragment is displayed again
    @Override
    public void onPause() {
        super.onPause();
        stateBundle = createOutStateBundle(null);
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(createOutStateBundle(outState));
    }

    /**
     * TODO remove?
     * onCreateView apparently runs before MainActivity initializes its views in MainActivity.onCreate()
     * this is an issue because enableEncryptionMode() changes the icon on the Floating Action Button,
     * which is one of MainActivity's views
     * therefore, this method can be called by MainActivity at the end of its onCreate() method
     */
    public void onMainActivityPostCreate() {
        if (isAdded() && !hasModeState) {
            //set the default mode
           // enableEncryptionMode();
        }
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.menu_main, menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_about:
                ((MainActivity)getActivity()).displayAboutFragment();
                return true;
        }
        return false;
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
    public void onResume() {
        super.onResume();
        ((MainActivity)getActivity()).setFabVisible(true);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == SELECT_INPUT_FILE_REQUEST_CODE && resultCode == Activity.RESULT_OK) {
            setContentUri(data.getData(), false);
        } else if (requestCode == SELECT_OUTPUT_DIRECTORY_REQUEST_CODE && resultCode == Activity.RESULT_OK) {
            setContentUri(data.getData(), true);
        } else if (resultCode != Activity.RESULT_CANCELED){
            showError(R.string.error_unexpected_response_from_saf);
        }
    }

    /**
     * called by MainActivity when the Floating Action Button is pressed.
     */
    public void actionButtonPressed() {
        if (isValidElsePrintErrors()) {
            Intent intent = new Intent(getContext(), CryptoService.class);
            intent.putExtra(CryptoService.INPUT_URI_EXTRA_KEY, inputFileUri.toString());
            intent.putExtra(CryptoService.OUTPUT_URI_EXTRA_KEY, outputFileUri.toString());
            intent.putExtra(CryptoService.VERSION_EXTRA_KEY, CryptoThread.VERSION_2);
            intent.putExtra(CryptoService.OPERATION_TYPE_EXTRA_KEY, operationMode);
            MainActivityFragment.password = passwordEditText.getText().toString().toCharArray();
            getContext().startService(intent);
        }
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
     * Shows the confirm password entry field, changes the member variable operationMode, and changes the icon on the Floating Action Button
     */
    private void enableEncryptionMode() {
        changeOperationTypeButtonAppearance(R.drawable.operation_mode_button_selected, R.drawable.operation_mode_button_selector);
        operationMode = CryptoThread.OPERATION_TYPE_ENCRYPTION;
        confirmPasswordTextView.setVisibility(View.VISIBLE);
        confirmPasswordEditText.setVisibility(View.VISIBLE);
        ((MainActivity)getActivity()).setFABIcon(R.drawable.ic_lock);
        hasModeState = true;
    }

    /**
     * Makes decryption mode active.
     * Hides the confirm password entry field, changes the member variable operationMode, and changes the icon on the Floating Action Button
     */
    private void enableDecryptionMode() {
        changeOperationTypeButtonAppearance(R.drawable.operation_mode_button_selector, R.drawable.operation_mode_button_selected);
        operationMode = CryptoThread.OPERATION_TYPE_DECRYPTION;
        confirmPasswordTextView.setVisibility(View.GONE);
        confirmPasswordEditText.setVisibility(View.GONE);
        ((MainActivity)getActivity()).setFABIcon(R.drawable.ic_unlock);
        hasModeState = true;
    }

    /*
    * Used to change the highlighting on the buttons when changing between encryption and decryption modes.
     */
    private void changeOperationTypeButtonAppearance(int encryptionDrawableId, int decryptionDrawableId) {
        encryptModeButton.setBackground(ResourcesCompat.getDrawable(getResources(), encryptionDrawableId, null));
        decryptModeButton.setBackground(ResourcesCompat.getDrawable(getResources(), decryptionDrawableId, null));
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
            if (operationMode == CryptoThread.OPERATION_TYPE_ENCRYPTION) {
                result = fileName.concat(".aes");
            } else if (operationMode == CryptoThread.OPERATION_TYPE_DECRYPTION) {
                if (fileName.substring(fileName.lastIndexOf('.')).equals(".aes")) {
                    result = fileName.substring(0, fileName.lastIndexOf('.'));
                } else {
                    result = "";
                }
            }
        }
        return result;
    }

    /*
    * returns true if the crypto operation can proceed and false otherwise.
    * displays messages about any issues to the user
     */
    private boolean isValidElsePrintErrors() {
        boolean valid = true;
        if (inputFileUri == null) {
            valid = false;
            showError(R.string.no_input_file_selected);
        } else if (outputFileUri == null) {
            valid = false;
            showError(R.string.no_output_file_selected);
        } else if (!passwordEditText.getText().toString().equals(confirmPasswordEditText.getText().toString())) {
            valid = false;
            showError(R.string.passwords_do_not_match);
        } else if (inputFileUri.equals(outputFileUri)) {
            valid = false;
            showError(R.string.the_input_and_output_files_must_be_different);
        }
        return valid;
    }

    /*
    * Get the password as a String and overwrite it in memory.
    * Overwriting the char[] may be useless since the EditText returns a String and AESCrypt requires a string,
    * but there isn't a good reason not to.
     */
    public static String getAndClearPassword() {
        if (MainActivityFragment.password == null) {
            return null;
        }
        String password = String.valueOf(MainActivityFragment.password);
        Arrays.fill(MainActivityFragment.password, '\0');
        MainActivityFragment.password = null;
        return password;
    }

    /*
    * Create a bundle that stores the state of MainActivityFragment and set MainActivityFragment.password
    * If used in onSaveInstanceState: preserve whatever values Android may put in the outState bundle already by passing in as systemOutStateBundle
    * If not called from onSaveInstanceState: pass null for systemOutStateBundle
     */
    private Bundle createOutStateBundle(Bundle systemOutStateBundle) {
        Bundle outState;
        if (systemOutStateBundle == null) {
            outState = new Bundle();
        } else {
            outState = systemOutStateBundle;
        }
        outState.putBoolean(SAVED_INSTANCE_STATE_SHOW_PASSWORD, showPasswordCheckbox.isChecked());
        outState.putBoolean(SAVED_INSTANCE_STATE_OPERATION_MODE, operationMode);
        if (inputFileUri != null) {
            outState.putString(SAVED_INSTANCE_STATE_INPUT_URI, inputFileUri.toString());
        }
        if (outputFileUri != null) {
            outState.putString(SAVED_INSTANCE_STATE_OUTPUT_URI, outputFileUri.toString());
        }
        MainActivityFragment.password = passwordEditText.getText().toString().toCharArray();
        return outState;
    }

    private void restoreFromStateBundle(Bundle stateBundle) {
        if (stateBundle == null) {
            setShowPassword(false);
            enableEncryptionMode();
        } else {
            setShowPassword(stateBundle.getBoolean(SAVED_INSTANCE_STATE_SHOW_PASSWORD, false));
            if (stateBundle.getBoolean(SAVED_INSTANCE_STATE_OPERATION_MODE, CryptoThread.OPERATION_TYPE_ENCRYPTION) == CryptoThread.OPERATION_TYPE_ENCRYPTION) {
                enableEncryptionMode();
            } else {
                enableDecryptionMode();
            }
            String inputUriString = stateBundle.getString(SAVED_INSTANCE_STATE_INPUT_URI, null);
            String outputUriString = stateBundle.getString(SAVED_INSTANCE_STATE_OUTPUT_URI, null);
            if (inputUriString != null) {
                setContentUri(Uri.parse(inputUriString), false);
            }
            if (outputUriString != null) {
                setContentUri(Uri.parse(outputUriString), true);
            }
            String password = getAndClearPassword();
            if (password != null) {
                passwordEditText.setText(password);
            }
        }
    }
}
