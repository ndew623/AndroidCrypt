package com.dewdrop623.androidcrypt;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.text.InputType;
import android.text.SpannableString;
import android.text.style.ForegroundColorSpan;
import android.view.Gravity;
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
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Arrays;

/**
 * A placeholder fragment containing a simple view.
 */
public class MainActivityFragment extends Fragment implements CryptoThread.ProgressDisplayer {

    public static final String CALLBACK_SELECT_OUTPUT_FILE = "com.dewdrop623.androidcrypt.MainActivityFragment.CALLBACK_SELECT_OUTPUT_FILE";
    public static final String CALLBACK_SELECT_INPUT_FILE = "com.dewdrop623.androidcrypt.MainActivityFragment.CALLBACK_SELECT_INPUT_FILE";

    /*
        Using static variables to store the password rather than savedInstanceState and Intent extras because of paranoia.
        Putting the password as a String into the Android OS that way seems like asking for trouble.
     */
    private static char[] password = null;

    private static final int SELECT_INPUT_FILE_REQUEST_CODE = 623;
    private static final int SELECT_OUTPUT_DIRECTORY_REQUEST_CODE = 8878;
    private static final int WRITE_FILE_PERMISSION_REQUEST_CODE = 440;

    private static final String PROGRESS_DISPLAYER_ID = "com.dewdrop623.androidcrypt.MainActivityFragment.PROGRESS_DISPLAYER_ID";

    private static final String SAVED_INSTANCE_STATE_SHOW_PASSWORD = "com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_SHOW_PASSWORD";
    private static final String SAVED_INSTANCE_STATE_OPERATION_MODE = "com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_OPERATION_MODE";
    private static final String SAVED_INSTANCE_STATE_INPUT_URI = "com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_INPUT_URI";
    private static final String SAVED_INSTANCE_STATE_OUTPUT_URI = "com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_OUTPUT_URI";

    //stores the type of operation to be done
    private boolean operationMode = CryptoThread.OPERATION_TYPE_ENCRYPTION;

    private Uri inputFileUri = null;
    private Uri outputFileUri = null;
    private Bundle stateBundle;
    //see comment on this.onAttach(Context)
    private Context context;

    private Button encryptModeButton;
    private Button decryptModeButton;
    private TextView missingFilesTextView;
    private LinearLayout inputContentURILinearLayout;
    private TextView inputContentURITextView;
    private View inputContentURIUnderlineView;
    private LinearLayout outputContentURILinearLayout;
    private TextView outputContentURITextView;
    private View outputContentURIUnderlineView;
    private FileSelectButton inputFileSelectButton;
    private FileSelectButton outputFileSelectButton;
    private EditText passwordEditText;
    private EditText confirmPasswordEditText;
    private CheckBox showPasswordCheckbox;
    private LinearLayout progressDisplayLinearLayout;
    private TextView progressDisplayTextView;
    private ProgressBar progressDisplayProgressBar;
    private LinearLayout progressDispayCancelButton;

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
        missingFilesTextView = (TextView) view.findViewById(R.id.missingFilesTextView);
        inputContentURILinearLayout = (LinearLayout) view.findViewById(R.id.inputContentURILinearLayout);
        inputContentURITextView = (TextView) view.findViewById(R.id.inputContentURITextView);
        inputContentURIUnderlineView = view.findViewById(R.id.inputContentURIUnderlineView);
        outputContentURILinearLayout = (LinearLayout) view.findViewById(R.id.outputContentURILinearLayout);
        outputContentURITextView = (TextView) view.findViewById(R.id.outputContentURITextView);
        outputContentURIUnderlineView = view.findViewById(R.id.outputContentURIUnderlineView);
        inputFileSelectButton = (FileSelectButton) view.findViewById(R.id.selectInputFileButton);
        outputFileSelectButton = (FileSelectButton) view.findViewById(R.id.selectOutputFileButton);
        passwordEditText = (EditText) view.findViewById(R.id.passwordEditText);
        confirmPasswordEditText = (EditText) view.findViewById(R.id.confirmPasswordEditText);
        showPasswordCheckbox = (CheckBox) view.findViewById(R.id.showPasswordCheckbox);
        progressDisplayLinearLayout = (LinearLayout) view.findViewById(R.id.progressDisplayLinearLayout);
        progressDisplayTextView = (TextView) view.findViewById(R.id.progressDisplayTextView);
        progressDisplayProgressBar = (ProgressBar) view.findViewById(R.id.progressDisplayProgressBar);
        progressDispayCancelButton = (LinearLayout) view.findViewById(R.id.progressDisplayCancelButtonLinearLayout);

        missingFilesTextView.setOnClickListener(missingFilesTextViewOnClickListener);
        showPasswordCheckbox.setOnCheckedChangeListener(showPasswordCheckBoxOnCheckedChangeListener);
        outputFileSelectButton.setOnClickListener(outputFileSelectButtonOnClickListener);
        inputFileSelectButton.setOnClickListener(inputFileSelectButtonOnClickListener);
        encryptModeButton.setOnClickListener(operationModeButtonsOnClickListener);
        decryptModeButton.setOnClickListener(operationModeButtonsOnClickListener);
        progressDispayCancelButton.setOnClickListener(progressDispayCancelButtonOnClickListener);

        checkPermissions();

        //Hide the keyboard that automatically pops up.
        getActivity().getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);

        if (stateBundle == null && savedInstanceState != null) {
            stateBundle = savedInstanceState;
        }
        restoreFromStateBundle(stateBundle);

        CryptoThread.registerForProgressUpdate(PROGRESS_DISPLAYER_ID, this);

        //Check if there is an operation in progress. If there is, get an update show the progress bar and cancel button immediately, rather than waiting for CryptoThread to push an update.
        if (CryptoThread.operationInProgress) {
            update(CryptoThread.getCurrentOperationType(), CryptoThread.getProgressUpdate(), CryptoThread.getCompletedMessageStringId());
        }

        return view;
    }

    //Store the current state when MainActivityFragment is added to back stack.
    //onCreateView will be called when the MainActivityFragment is displayed again
    //onSaveInstance state won't necessarily do this when the view is hidden
    @Override
    public void onPause() {
        super.onPause();
        stateBundle = createOutStateBundle(null);
    }

    /*
    * Let MainActivity know that the user has returned to this fragment.
    * */
    @Override
    public void onResume() {
        super.onResume();
        ((MainActivity) getActivity()).returnedToMainFragment();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(createOutStateBundle(outState));
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.menu_main, menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_about:
                ((MainActivity) getActivity()).displaySecondaryFragmentScreen(new AboutFragment(), getString(R.string.action_about));
                return true;
            case R.id.action_settings:
                ((MainActivity) getActivity()).displaySecondaryFragmentScreen(new SettingsFragment(), getString(R.string.action_settings));
                return true;
        }
        return false;
    }

    /*
    * Apparently there is a bug in Android that causes getActivity()/getContext() to return null sometimes (after a rotate in this case).
    * This is a workaround.
    */
    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        this.context = context;
    }

    /*
        The Storage Access Framework has a result for us. Let's do the appropriate actions with it.
     */
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == SELECT_INPUT_FILE_REQUEST_CODE && resultCode == Activity.RESULT_OK) {
            setUriAndUpdateUI(data.getData(), false);
        } else if (requestCode == SELECT_OUTPUT_DIRECTORY_REQUEST_CODE && resultCode == Activity.RESULT_OK) {
            setUriAndUpdateUI(data.getData(), true);
        } else if (resultCode != Activity.RESULT_CANCELED) {
            showError(R.string.error_unexpected_response_from_saf);
        }
    }

    /*
    * Implementation of ProgressDisplayer interface. Called by CryptoThread to show progress.
    * Has to be done on the gui thread.
     */
    @Override
    public void update(final boolean operationType, final int progress, final int completedMessageStringId) {
        final Context context = getContext();
        if (context != null) {
            new Handler(context.getMainLooper()).post(new Runnable() {
                @Override
                public void run() {
                    progressDisplayLinearLayout.setVisibility(progress == 100 ? View.INVISIBLE : View.VISIBLE);
                    progressDisplayProgressBar.setProgress(progress);
                    if (operationType == CryptoThread.OPERATION_TYPE_ENCRYPTION) {
                        progressDisplayTextView.setText(R.string.encrypting);
                    } else {
                        progressDisplayTextView.setText(R.string.decrypting);
                    }
                }
            });
        }
    }

    /*
         * This onClickListener is for click on help textview.
         */
    private View.OnClickListener missingFilesTextViewOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            ((MainActivity) getActivity()).showMissingFilesHelpDialog(false, (MainActivityFragment) getParentFragment(), null);
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
            selectInputFile(false);
        }
    };

    private View.OnClickListener outputFileSelectButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            selectOutputFile(false);
        }
    };
    private View.OnClickListener progressDispayCancelButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            CryptoThread.cancel();
        }
    };

    /**
     * ask StorageAccessFramework to allow user to pick a file
     * if:
     * -dont show this again has not been selected from the show internal storage help dialog
     * and:
     * -this is not a callback from that same help dialog
     * then:
     * show that dialog before selecting a file
     */
    public void selectInputFile(boolean callback) {
        if (SettingsHelper.getShowInternalStorageHelp(getContext()) && !callback) {
            ((MainActivity) getActivity()).showMissingFilesHelpDialog(true, this, CALLBACK_SELECT_INPUT_FILE);
        } else {
            StorageAccessFrameworkHelper.safPickFile(this, SELECT_INPUT_FILE_REQUEST_CODE);
        }
    }

    /**
     * ask StorageAccessFramework to allow user to pick a directory
     * if:
     * -dont show this again has not been selected from the show internal storage help dialog
     * and:
     * -this is not a callback from that same help dialog
     * then:
     * show that dialog before selecting a file
     */
    public void selectOutputFile(boolean callback) {
        if (SettingsHelper.getShowInternalStorageHelp(getContext()) && !callback) {
            ((MainActivity) getActivity()).showMissingFilesHelpDialog(true, this, CALLBACK_SELECT_OUTPUT_FILE);
        } else {
            StorageAccessFrameworkHelper.safPickOutputFile(this, SELECT_OUTPUT_DIRECTORY_REQUEST_CODE, getDefaultOutputFileName());
        }
    }

    /*
    * Set the inputFileUri (output == false) or outputFileUri (output == true) member variable and change UI of the file select buttons.
    * Pass null to clear the uri value and reset ui.
     */
    private void setUriAndUpdateUI(Uri uri, boolean output) {
        TextView contentURITextView;
        FileSelectButton fileSelectButton;
        View contentURIUnderlineView;
        LinearLayout contentURILinearLayout;
        String contentURITextPrefix;
        if (output) {
            outputFileUri = uri;
            contentURITextView = outputContentURITextView;
            fileSelectButton = outputFileSelectButton;
            contentURIUnderlineView = outputContentURIUnderlineView;
            contentURILinearLayout = outputContentURILinearLayout;
            contentURITextPrefix = getString(R.string.output_file).concat(": ");
        } else {
            inputFileUri = uri;
            contentURITextView = inputContentURITextView;
            fileSelectButton = inputFileSelectButton;
            contentURIUnderlineView = inputContentURIUnderlineView;
            contentURILinearLayout = inputContentURILinearLayout;
            contentURITextPrefix = getString(R.string.input_file).concat(": ");
        }

        String contentURIText = "";
        int contentURITextViewVisibility = View.GONE;
        int contentURIUnderlineViewVisibility = View.INVISIBLE;
        boolean fileSelectButtonMinimize = false;
        int gravity = Gravity.CENTER;
        if (uri != null) {
            contentURIText = StorageAccessFrameworkHelper.getFileNameFromUri(uri, context);
            contentURITextViewVisibility = View.VISIBLE;
            contentURIUnderlineViewVisibility = View.VISIBLE;
            fileSelectButtonMinimize = true;
            gravity = Gravity.START | Gravity.CENTER_VERTICAL;
        }
        fileSelectButton.setMinimized(fileSelectButtonMinimize);
        SpannableString contentURISpannableString = new SpannableString(contentURITextPrefix.concat(contentURIText));
        contentURISpannableString.setSpan(new ForegroundColorSpan(Color.GRAY), 0, contentURITextPrefix.length(), 0);
        contentURITextView.setText(contentURISpannableString);
        contentURITextView.setVisibility(contentURITextViewVisibility);
        contentURIUnderlineView.setVisibility(contentURIUnderlineViewVisibility);
        contentURILinearLayout.setGravity(gravity);
    }

    /**
     * called by MainActivity when the Floating Action Button is pressed.
     */
    public void actionButtonPressed() {
        if (isValidElsePrintErrors()) {
            //Can't use getContext() or getActivity(). See comment on this.onAttach(Context)
            Intent intent = new Intent(context, CryptoService.class);
            intent.putExtra(CryptoService.INPUT_URI_EXTRA_KEY, inputFileUri.toString());
            intent.putExtra(CryptoService.OUTPUT_URI_EXTRA_KEY, outputFileUri.toString());
            intent.putExtra(CryptoService.VERSION_EXTRA_KEY, SettingsHelper.getAESCryptVersion(getContext()));
            intent.putExtra(CryptoService.OPERATION_TYPE_EXTRA_KEY, operationMode);
            MainActivityFragment.password = passwordEditText.getText().toString().toCharArray();
            context.startService(intent);
        }
    }

    //check for the necessary permissions. destroy and recreate the activity if permissions are asked for so that the files (which couldn't be seen previously) will be displayed
    private void checkPermissions() {
        if (ContextCompat.checkSelfPermission(context, android.Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
            ActivityCompat.requestPermissions(getActivity(), new String[]{android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_FILE_PERMISSION_REQUEST_CODE);
        }
    }

    /*
    * Display an error to the user via toast.
    * */
    private void showError(String error) {
        Toast.makeText(context, error, Toast.LENGTH_SHORT).show();
    }

    /*
    * Display an error to the user via toast.
    * */
    private void showError(int stringId) {
        showError(context.getString(stringId));
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

        //Fix the typeface. Android wants to change it to a monospace font everytime showPassword is set to false. This makes the edittext hint change appearance... and it looks ugly.
        passwordEditText.setTypeface(Typeface.DEFAULT);
        confirmPasswordEditText.setTypeface(Typeface.DEFAULT);
    }

    /**
     * Makes encryption mode active.
     * Shows the confirm password entry field, changes the member variable operationMode, updates appearance of operation mode buttons, and changes the icon on the Floating Action Button
     */
    private void enableEncryptionMode() {
        changeOperationTypeButtonAppearance(R.drawable.operation_mode_button_selected, R.drawable.operation_mode_button_selector);
        operationMode = CryptoThread.OPERATION_TYPE_ENCRYPTION;
        confirmPasswordEditText.setVisibility(View.VISIBLE);
        encryptModeButton.setTextColor(ContextCompat.getColor(context, android.R.color.white));
        decryptModeButton.setTextColor(ContextCompat.getColor(context, android.R.color.darker_gray));
        ((MainActivity) getActivity()).setFABIcon(R.drawable.ic_lock);
    }

    /**
     * Makes decryption mode active.
     * Hides the confirm password entry field, changes the member variable operationMode, updates appearance of operation mode buttons, and changes the icon on the Floating Action Button
     */
    private void enableDecryptionMode() {
        changeOperationTypeButtonAppearance(R.drawable.operation_mode_button_selector, R.drawable.operation_mode_button_selected);
        operationMode = CryptoThread.OPERATION_TYPE_DECRYPTION;
        confirmPasswordEditText.setVisibility(View.INVISIBLE);
        encryptModeButton.setTextColor(ContextCompat.getColor(context, android.R.color.darker_gray));
        decryptModeButton.setTextColor(ContextCompat.getColor(context, android.R.color.white));
        ((MainActivity) getActivity()).setFABIcon(R.drawable.ic_unlock);
    }

    /*
    * Used to change the highlighting on the buttons when changing between encryption and decryption modes.
     */
    private void changeOperationTypeButtonAppearance(int encryptionDrawableId, int decryptionDrawableId) {
        encryptModeButton.setBackground(ResourcesCompat.getDrawable(getResources(), encryptionDrawableId, null));
        decryptModeButton.setBackground(ResourcesCompat.getDrawable(getResources(), decryptionDrawableId, null));
    }


    /*return the default output filename based on the inputFileUri.
    *if inputFileUri is null, returns null.
    * if in encryption mode, append '.aes' to filename.
    * if in decryption mode, and input filename ends with '.aes', remove '.aes'
    * if in decryption mode and input filename does not end with '.aes', return empty string*/
    private String getDefaultOutputFileName() {
        String result = null;
        if (inputFileUri != null) {
            String fileName = StorageAccessFrameworkHelper.getFileNameFromUri(inputFileUri, context);
            if (operationMode == CryptoThread.OPERATION_TYPE_ENCRYPTION) {
                result = fileName.concat(".aes");
            } else if (operationMode == CryptoThread.OPERATION_TYPE_DECRYPTION) {
                if (fileName.lastIndexOf('.') != -1 && fileName.substring(fileName.lastIndexOf('.')).equals(".aes")) {
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
        } else if (operationMode == CryptoThread.OPERATION_TYPE_ENCRYPTION && !passwordEditText.getText().toString().equals(confirmPasswordEditText.getText().toString())) {
            valid = false;
            showError(R.string.passwords_do_not_match);
        } else if (inputFileUri.equals(outputFileUri)) {
            valid = false;
            showError(R.string.the_input_and_output_files_must_be_different);
        } else if (CryptoThread.operationInProgress) {
            valid = false;
            showError(R.string.another_operation_is_already_in_progress);
        }
        return valid;
    }

    /*
    * Get the password as a String and overwrite it in memory.
    * Overwriting the char[] here may be useless since the EditText returns the password as a String and AESCrypt requires it as a String,
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
    * If used in onSaveInstanceState: preserve whatever values Android may put in the outState bundle already by passing it in as systemOutStateBundle
    * If not called from onSaveInstanceState: pass null for systemOutStateBundle
    * Sets the static char[] to the password in passwordEditText.
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

    /*
    * Given a bundle from createOutStateBundle, restore the state of the fragment.
    * Retrieves and clears the password from the char[] MainActivityFragment.password
    * */
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
                setUriAndUpdateUI(Uri.parse(inputUriString), false);
            }
            if (outputUriString != null) {
                setUriAndUpdateUI(Uri.parse(outputUriString), true);
            }
            String password = getAndClearPassword();
            if (password != null) {
                passwordEditText.setText(password);
            }
        }
    }
}
