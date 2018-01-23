package com.dewdrop623.androidcrypt;

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
import android.support.v4.provider.DocumentFile;
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

import java.io.File;
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
    private static final String SAVED_INSTANCE_STATE_INPUT_PARENT_URI = "com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_INPUT_PARENT_URI";
    private static final String SAVED_INSTANCE_STATE_OUTPUT_PARENT_URI = "com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_OUTPUT_PARENT_URI";
    private static final String SAVED_INSTANCE_STATE_INPUT_FILENAME = "com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_INPUT_FILENAME";
    private static final String SAVED_INSTANCE_STATE_OUTPUT_FILENAME = "com.dewdrop623.androidcrypt.MainActivityFragment.SAVED_INSTANCE_STATE_OUTPUT_FILENAME";

    //stores the type of operation to be done
    private boolean operationMode = CryptoThread.OPERATION_TYPE_ENCRYPTION;
    private boolean showPassword = false;
    private DocumentFile inputFileParentDirectory = null;
    String inputFileName;
    private DocumentFile outputFileParentDirectory = null;
    String outputFileName;
    //see comment on this.onAttach(Context)
    private Context context;

    private Button encryptModeButton;
    private Button decryptModeButton;
    private LinearLayout inputFilePathLinearLayout;
    private TextView inputFilePathTextView;
    private View inputFilePathUnderlineView;
    private LinearLayout outputFilePathLinearLayout;
    private TextView outputFilePathTextView;
    private View outputFilePathUnderlineView;
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
        inputFilePathLinearLayout = (LinearLayout) view.findViewById(R.id.inputFilePathLinearLayout);
        inputFilePathTextView = (TextView) view.findViewById(R.id.inputFilePathTextView);
        inputFilePathUnderlineView = view.findViewById(R.id.inputFilePathUnderlineView);
        outputFilePathLinearLayout = (LinearLayout) view.findViewById(R.id.outputFilePathLinearLayout);
        outputFilePathTextView = (TextView) view.findViewById(R.id.outputFilePathTextView);
        outputFilePathUnderlineView = view.findViewById(R.id.outputFilePathUnderlineView);
        inputFileSelectButton = (FileSelectButton) view.findViewById(R.id.selectInputFileButton);
        outputFileSelectButton = (FileSelectButton) view.findViewById(R.id.selectOutputFileButton);
        passwordEditText = (EditText) view.findViewById(R.id.passwordEditText);
        confirmPasswordEditText = (EditText) view.findViewById(R.id.confirmPasswordEditText);
        showPasswordCheckbox = (CheckBox) view.findViewById(R.id.showPasswordCheckbox);
        progressDisplayLinearLayout = (LinearLayout) view.findViewById(R.id.progressDisplayLinearLayout);
        progressDisplayTextView = (TextView) view.findViewById(R.id.progressDisplayTextView);
        progressDisplayProgressBar = (ProgressBar) view.findViewById(R.id.progressDisplayProgressBar);
        progressDispayCancelButton = (LinearLayout) view.findViewById(R.id.progressDisplayCancelButtonLinearLayout);

        showPasswordCheckbox.setOnCheckedChangeListener(showPasswordCheckBoxOnCheckedChangeListener);
        outputFileSelectButton.setOnClickListener(outputFileSelectButtonOnClickListener);
        inputFileSelectButton.setOnClickListener(inputFileSelectButtonOnClickListener);
        encryptModeButton.setOnClickListener(operationModeButtonsOnClickListener);
        decryptModeButton.setOnClickListener(operationModeButtonsOnClickListener);
        progressDispayCancelButton.setOnClickListener(progressDispayCancelButtonOnClickListener);

        checkPermissions();

        //Hide the keyboard that automatically pops up.
        getActivity().getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);

        updateUI(savedInstanceState);

        CryptoThread.registerForProgressUpdate(PROGRESS_DISPLAYER_ID, this);

        //Check if there is an operation in progress. If there is, get an update show the progress bar and cancel button immediately, rather than waiting for CryptoThread to push an update.
        if (CryptoThread.operationInProgress) {
            update(CryptoThread.getCurrentOperationType(), CryptoThread.getProgressUpdate(), CryptoThread.getCompletedMessageStringId());
        }

        return view;
    }

    //Store the current state when MainActivityFragment is added to back stack.
    //onCreateView will be called when the MainActivityFragment is displayed again
    //onSaveInstance state WILL NOT do this when the view is hidden
    @Override
    public void onPause() {
        super.onPause();
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
                ((MainActivity) getActivity()).displaySecondaryFragmentScreen(new AboutFragment(), context.getString(R.string.action_about), null);
                return true;
            case R.id.action_settings:
                ((MainActivity) getActivity()).displaySecondaryFragmentScreen(new SettingsFragment(), context.getString(R.string.action_settings), null);
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
    private View.OnClickListener progressDispayCancelButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            CryptoThread.cancel();
        }
    };

    /**
     * Show the FilePicker fragment so the user can pick a file.
     * Open the file picker in the same folder that an already picked input file came from.
     */
    public void selectInputFile() {
        String initialFolder = null;
        ((MainActivity) getActivity()).pickFile(false, inputFileParentDirectory, null);
    }

    /**
     * Open the FilePicker fragment so the user can pick an output file.
     * if an output file has previously been selected open its directory.
     * else if an input file has already been selected open that directory.
     */
    public void selectOutputFile() {
        DocumentFile initialFolder = outputFileParentDirectory;
        if (initialFolder == null) {
            initialFolder = inputFileParentDirectory;
        }
        ((MainActivity) getActivity()).pickFile(true, initialFolder, getDefaultOutputFileName());
    }

    public void setFile(DocumentFile file, String filename, boolean isOutput) {
        if (isOutput) {
            outputFileParentDirectory = file;
            outputFileName = filename;
        } else {
            inputFileParentDirectory = file;
            inputFileName = filename;
        }
        if (context != null) {
            updateFileUI(isOutput);
        }
    }

    /*
    * Set the inputFileParentDirectory (isOutput == false) or outputFileParentDirectory (isOutput == true) member variable and change UI of the file select buttons.
    * Pass null to clear the uri value and reset ui.
     */
    private void updateFileUI(boolean isOutput) {
        String filePath;
        TextView filePathTextView;
        FileSelectButton fileSelectButton;
        View filePathUnderlineView;
        LinearLayout filePathLinearLayout;
        String filePathTextPrefix;
        if (isOutput) {
            filePath = StorageAccessFrameworkHelper.getDocumentFilePath(outputFileParentDirectory, outputFileName);
            filePathTextView = outputFilePathTextView;
            fileSelectButton = outputFileSelectButton;
            filePathUnderlineView = outputFilePathUnderlineView;
            filePathLinearLayout = outputFilePathLinearLayout;
            filePathTextPrefix = context.getString(R.string.output_file).concat(": ");
        } else {
            filePath = StorageAccessFrameworkHelper.getDocumentFilePath(inputFileParentDirectory, inputFileName);
            filePathTextView = inputFilePathTextView;
            fileSelectButton = inputFileSelectButton;
            filePathUnderlineView = inputFilePathUnderlineView;
            filePathLinearLayout = inputFilePathLinearLayout;
            filePathTextPrefix = context.getString(R.string.input_file).concat(": ");
        }
        int filePathTextViewVisibility = View.GONE;
        int filePathUnderlineViewVisibility = View.INVISIBLE;
        boolean fileSelectButtonMinimize = false;
        int gravity = Gravity.CENTER;
        if (!filePath.isEmpty()) {
            filePathTextViewVisibility = View.VISIBLE;
            filePathUnderlineViewVisibility = View.VISIBLE;
            fileSelectButtonMinimize = true;
            gravity = Gravity.START | Gravity.CENTER_VERTICAL;
        }
        fileSelectButton.setMinimized(fileSelectButtonMinimize);
        SpannableString contentURISpannableString = new SpannableString(filePathTextPrefix.concat(filePath));
        contentURISpannableString.setSpan(new ForegroundColorSpan(Color.GRAY), 0, filePathTextPrefix.length(), 0);
        filePathTextView.setText(contentURISpannableString);
        filePathTextView.setVisibility(filePathTextViewVisibility);
        filePathUnderlineView.setVisibility(filePathUnderlineViewVisibility);
        filePathLinearLayout.setGravity(gravity);
    }

    /**
     * called by MainActivity when the Floating Action Button is pressed.
     */
    public void actionButtonPressed() {
        if (isValidElsePrintErrors()) {
            //Can't use getContext() or getActivity(). See comment on this.onAttach(Context)
            Intent intent = new Intent(context, CryptoService.class);
            intent.putExtra(CryptoService.INPUT_FILE_PATH_EXTRA_KEY, inputFileParentDirectory.getUri().toString());
            intent.putExtra(CryptoService.OUTPUT_FILE_PATH_EXTRA_KEY, outputFileParentDirectory.getUri().toString());
            intent.putExtra(CryptoService.INPUT_FILENAME_KEY, inputFileName);
            intent.putExtra(CryptoService.OUTPUT_FILENAME_KEY, outputFileName);
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
        this.showPassword = showPassword;
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


    /*return the default output filename based on the inputFileParentDirectory.
    *if inputFileParentDirectory is null, returns null.
    * if in encryption mode, append '.aes' to filename.
    * if in decryption mode, and input filename ends with '.aes', remove '.aes'
    * if in decryption mode and input filename does not end with '.aes', return empty string*/
    private String getDefaultOutputFileName() {
        String result = null;
        if (inputFileParentDirectory != null) {
            String fileName = inputFileParentDirectory.getName();
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
        if (inputFileParentDirectory == null) {
            valid = false;
            showError(R.string.no_input_file_selected);
        } else if (outputFileParentDirectory == null) {
            valid = false;
            showError(R.string.no_output_file_selected);
        } else if (operationMode == CryptoThread.OPERATION_TYPE_ENCRYPTION && !passwordEditText.getText().toString().equals(confirmPasswordEditText.getText().toString())) {
            valid = false;
            showError(R.string.passwords_do_not_match);
        } else if (inputFileParentDirectory.equals(outputFileParentDirectory)) {
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
        outState.putBoolean(SAVED_INSTANCE_STATE_SHOW_PASSWORD, showPassword);
        outState.putBoolean(SAVED_INSTANCE_STATE_OPERATION_MODE, operationMode);
        if (inputFileParentDirectory != null) {
            outState.putString(SAVED_INSTANCE_STATE_INPUT_PARENT_URI, inputFileParentDirectory.getUri().toString());
            outState.putString(SAVED_INSTANCE_STATE_INPUT_FILENAME, inputFileName);
        }
        if (outputFileParentDirectory != null) {
            outState.putString(SAVED_INSTANCE_STATE_OUTPUT_PARENT_URI, outputFileParentDirectory.getUri().toString());
            outState.putString(SAVED_INSTANCE_STATE_OUTPUT_FILENAME, outputFileName);
        }

        /*
         * may not be able to get password from view if screen was rotated while viewing another fragment.
         */
        if (passwordEditText != null) {
            MainActivityFragment.password = passwordEditText.getText().toString().toCharArray();
        }
        return outState;
    }

    /*
    * Update the UI to match the member variables and/or savedInstanceState
    * */
    private void updateUI(Bundle savedInstanceState) {
        if (savedInstanceState == null) {
            savedInstanceState = new Bundle();
        }
        setShowPassword(savedInstanceState.getBoolean(SAVED_INSTANCE_STATE_SHOW_PASSWORD, showPassword));
        if (savedInstanceState.getBoolean(SAVED_INSTANCE_STATE_OPERATION_MODE, operationMode) == CryptoThread.OPERATION_TYPE_ENCRYPTION) {
            enableEncryptionMode();
        } else {
            enableDecryptionMode();
        }
        String inputFileParentUriString = savedInstanceState.getString(SAVED_INSTANCE_STATE_INPUT_PARENT_URI, null);
        String outputFileParentUriString = savedInstanceState.getString(SAVED_INSTANCE_STATE_OUTPUT_PARENT_URI, null);
        if (inputFileParentUriString != null) {
            String filename = savedInstanceState.getString(SAVED_INSTANCE_STATE_INPUT_FILENAME, null);
            setFile(DocumentFile.fromSingleUri(getContext(), Uri.parse(inputFileParentUriString)), filename, false);
        }
        updateFileUI(false);
        if (outputFileParentUriString != null) {
            String filename = savedInstanceState.getString(SAVED_INSTANCE_STATE_OUTPUT_FILENAME, null);
            setFile(DocumentFile.fromSingleUri(getContext(), Uri.parse(outputFileParentUriString)), filename, true);
        }
        updateFileUI(true);
        String password = getAndClearPassword();
        if (password != null) {
            passwordEditText.setText(password);
        }
    }
}
