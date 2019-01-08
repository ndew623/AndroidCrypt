package com.dewdrop623.androidcrypt.FilePicker;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.app.AlertDialog;
import android.text.method.ScrollingMovementMethod;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.dewdrop623.androidcrypt.MainActivity;
import com.dewdrop623.androidcrypt.R;
import com.dewdrop623.androidcrypt.SettingsHelper;
import com.dewdrop623.androidcrypt.StorageAccessFrameworkHelper;

import java.io.File;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * FilePickerFragment fragment allows users to select a file.
 * Child fragments determine what implementation of android's AbsListView to use.
 * Child fragments define the behavior for the listviewadapter's getView method.
 */

public abstract class FilePickerFragment extends Fragment {

    /**
     * Keys for initialization arguments bundle
     */
    public static final String IS_OUTPUT_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePickerFragment.IS_OUTPUT_KEY";
    public static final String DEFAULT_OUTPUT_FILENAME_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePickerFragment.DEFAULT_OUTPUT_FILENAME_KEY";
    public static final String INITIAL_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePickerFragment.INITIAL_DIRECTORY_KEY";

    /**
     * Save instance state keys
     */
    public static final String CURRENT_DIRECTORY_STATE_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePickerFragment.CURRENT_DIRECTORY_STATE_KEY";

    ////////////////////////////////
    //MEMBER VARIABLES
    ///////////////////////////////

    //for storing the current state
    private boolean isOutput;
    private boolean selectingFiles = false;

    //the TextView that shows the current directory under the actionbar
    private TextView currentPathTextView;
    private LinearLayout fileNameInputLinearLayout;
    private EditText fileNameEditText;
    private Button fileNameOkButton;

    private FileBrowser fileBrowser;

    //The list view to display the filenames
    protected AbsListView fileListView;
    private FileListAdapter fileListAdapter;

    ///////////////////////////////
    //ANONYMOUS CLASSES
    /////////////////////////////////
    //a comparator to help sort file alphabetically
    private Comparator<File> fileNameAlphabeticalComparator = new Comparator<File>() {
        @Override
        public int compare(File file1, File file2) {
            int result;
            ///TODO figure out if these special cases for the parent file (i.e. "..") are necessary
            /*if (file1.first.equals(FileBrowser.PARENT_FILE_NAME)) {
                result = -1;
            } else if (file2.first.equals(FileBrowser.PARENT_FILE_NAME)) {
                result = 1;
            } else {*/
                result = file1.getName().toLowerCase().compareTo(file2.getName().toLowerCase());
            /*}*/
            return result;
        }
    };

    // the onClickListener for file items in the fileListView
    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileListAdapter.getItem(position);
            /*TODO might needs to modify to handle having .. in directory
            DocumentFile clickedFile;
            if (clickedFile.equals(FileBrowser.PARENT_FILE_NAME)) {
                clickedFile = fileBrowser.getCurrentDirectory().getParentFile();
            } else {
                clickedFile = fileBrowser.getCurrentDirectory().findFile(clickedFile);
            }*/
            if (clickedFile.isDirectory()) {
                fileBrowser.setCurrentDirectory(clickedFile);
            } else {
                if (isOutput) {
                    fileNameEditText.setText(clickedFile.getName());
                } else {
                    //TODO probably don't need to pass parent file and file name seperately now using java.io.File and not DocumentFile
                    ((MainActivity) getActivity()).filePicked(clickedFile.getParentFile(), clickedFile.getName(), isOutput);
                    ((MainActivity) getActivity()).superOnBackPressed();
                }
            }
        }
    };

    //the onLongClick listener for file items in the fileListView
    private AdapterView.OnItemLongClickListener onItemLongClickListener = new AdapterView.OnItemLongClickListener() {
        @Override
        public boolean onItemLongClick(AdapterView<?> adapterView, View view, int i, long l) {
            ((CheckBox)view.findViewById(R.id.fileSelectCheckbox)).setChecked(true);
            startMultiSelection();
            return true;
        }
    };

    private View.OnClickListener fileNameOkButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            String fileName = fileNameEditText.getText().toString();
            File newFileParentDirectory = fileBrowser.getCurrentDirectory();
            String error = checkFileErrors(newFileParentDirectory, fileName);
            if (error.isEmpty()) {
                ((MainActivity) getActivity()).filePicked(newFileParentDirectory, fileName, isOutput);
                ((MainActivity) getActivity()).superOnBackPressed();
            } else {
                Toast.makeText(getContext(), error, Toast.LENGTH_SHORT).show();
            }
        }
    };

    /////////////////////////
    //PUBLIC METHODS
    /////////////////////////

    /*create a FileBrowser and assign it to the member variable
    * get the savedInstanceState and put it in a member variable
    * */

    //create the options menu
    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        //create file viewer menu with a home button and a refresh button
        inflater.inflate(R.menu.file_viewer_menu, menu);
        if (selectingFiles) {
            menu.findItem(R.id.sdcard_button).setVisible(false);
            menu.findItem(R.id.home_button).setVisible(false);
            menu.findItem(R.id.refresh_button).setVisible(false);
            menu.findItem(R.id.stop_selection).setVisible(true);
            menu.findItem(R.id.compress).setVisible(true);
        } else {
            menu.findItem(R.id.sdcard_button).setVisible(true);
            menu.findItem(R.id.home_button).setVisible(true);
            menu.findItem(R.id.refresh_button).setVisible(true);
            menu.findItem(R.id.stop_selection).setVisible(false);
            menu.findItem(R.id.compress).setVisible(false);
        }
        if (!StorageAccessFrameworkHelper.canSupportSDCardOnAndroidVersion()) {
            menu.findItem(R.id.sdcard_button).setVisible(false);
        }
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
        fileListAdapter = new FileListAdapter();
        fileBrowser = new FileBrowser(getContext());
        fileBrowser.setFilePickerFragment(this);
        isOutput = getArguments().getBoolean(FilePickerFragment.IS_OUTPUT_KEY, false);
        if (savedInstanceState != null) {
            fileBrowser.setCurrentDirectory(new File(savedInstanceState.getString(CURRENT_DIRECTORY_STATE_KEY),
                    fileBrowser.getHomeDirectory().getAbsolutePath()));
        }
    }

    //save the current state for a screen rotation
    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putString(CURRENT_DIRECTORY_STATE_KEY, fileBrowser.getCurrentDirectory().getAbsolutePath());
        super.onSaveInstanceState(outState);
    }

    //define behavior for the options menu
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.sdcard_button:
                sdCardMenuButtonOnClick();
                return true;
            case R.id.home_button:
                goToHomeDirectory();
                return true;
            case R.id.refresh_button:
                fileBrowser.updateFileViewer();
                return true;
            case R.id.stop_selection:
                stopMultiSelection();
                return true;
            case R.id.compress:
                tarSelectedFiles();
                return true;
        }
        return false;
    }

    /*Start selecting. Show the checkboxes in the listview items*/
    private void startMultiSelection() {
        selectingFiles=true;
        for (int i = 0; i < fileListView.getChildCount(); i++) {
            ((CheckBox)fileListView.getChildAt(i).findViewById(R.id.fileSelectCheckbox)).setVisibility(View.VISIBLE);
        }
        getActivity().invalidateOptionsMenu();
    }

    /*Stop selecting. Unmark and hide the checkboxes in the listview items*/
    private void stopMultiSelection() {
        selectingFiles=false;
        for (int i = 0; i < fileListView.getChildCount(); i++) {
            CheckBox checkBox = ((CheckBox)fileListView.getChildAt(i).findViewById(R.id.fileSelectCheckbox));
            checkBox.setChecked(false);
            checkBox.setVisibility(View.GONE);
        }
        getActivity().invalidateOptionsMenu();
    }

    private void tarSelectedFiles() {
        ArrayList<File> toBeTarballed = new ArrayList<>();
        for(int i = 0; i < fileListView.getChildCount(); i++) {
            if (((CheckBox)fileListView.getChildAt(i).findViewById(R.id.fileSelectCheckbox)).isChecked()) {
                toBeTarballed.add(fileListAdapter.getItem(i));
            }
        }

    }

    /**
     * called by the MainActivity when the back button is pressed, goes up one directory
     * if parent directory does not exist: regular back button behavior
     */
    public void onBackPressed() {
        if (!fileBrowser.goToParentDirectory()) {
            ((MainActivity) getActivity()).superOnBackPressed();
        }
    }

    //called by the FileBrowser, updates the displayed list of filenames
    public void setFileList(List<File> files) {
        updateFileArrayAdapterFileList(files);
        if (currentPathTextView != null) {
            currentPathTextView.setText(fileBrowser.getCurrentDirectory().getName());
        }
    }

    //tell the FileBrowser to change the path
    public void changeDirectory(File file) {
        fileBrowser.setCurrentDirectory(file);
    }

    /**
     * TODO. Will need to be modified when supporting multiple external storage devices.
     * TODO. fuck it. might just take it out when supporting multiple removable storage.
     * TODO. this doesn't even use the settings. it just guesses at the sd card. wtf?
     * Exists so MainActivity can call this after SAF Activity for selecting SD card completes.
     */
    public void changePathToSDCard() {
        if (StorageAccessFrameworkHelper.canSupportSDCardOnAndroidVersion()) {
            //cache the current directory to recover if changing to the sd card fails
            File currentDirectory = fileBrowser.getCurrentDirectory();
            try {
                String[] storageDirs = StorageAccessFrameworkHelper.getExternalStorageDirectories(getContext());
                if (storageDirs.length > 1) {
                    fileBrowser.setCurrentDirectory(new File(storageDirs[1]));
                }
            } catch (NullPointerException npe) {
                /*TODO this exception handler may not be necessary*/
                fileBrowser.setCurrentDirectory(currentDirectory);
                AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
                builder.setTitle(R.string.sdcard_error).setMessage(R.string.could_not_read_sdcard).setPositiveButton(R.string.ok, null);
                builder.show();
            }
        }
    }

    ///////////////////////////////////
    //PROTECTED METHODS
    ///////////////////////////////////

    //after the concrete class inflates the view and defines the callback for the list adapter getview method, do the rest of the ui initialization work
    protected final void initializeFilePickerWithViewAndFileListAdapterGetViewCallback(View view, FileListAdapterGetViewCallback fileListAdapterGetViewCallback) {
        fileListAdapter.setFileListAdapterGetViewCallback(fileListAdapterGetViewCallback);
        fileListView.setVisibility(View.VISIBLE);
        fileListView.setAdapter(fileListAdapter);
        fileListView.setVisibility(View.VISIBLE);
        fileListView.setOnItemClickListener(onItemClickListener);
        fileListView.setOnItemLongClickListener(onItemLongClickListener);
        currentPathTextView = (TextView) view.findViewById(R.id.currentPathTextView);
        currentPathTextView.setMovementMethod(new ScrollingMovementMethod());
        currentPathTextView.setText(fileBrowser.getCurrentDirectory().getName());

        fileNameInputLinearLayout = (LinearLayout) view.findViewById(R.id.fileNameInputLinearLayout);
        fileNameEditText = (EditText) view.findViewById(R.id.fileNameEditText);
        fileNameOkButton = (Button) view.findViewById(R.id.fileNameOkButton);

        if (isOutput) {
            fileNameInputLinearLayout.setVisibility(View.VISIBLE);
            fileNameEditText.setTextColor(Color.BLACK);
            fileNameOkButton.setOnClickListener(fileNameOkButtonOnClickListener);

            String defaultOutputFilename = getArguments().getString(DEFAULT_OUTPUT_FILENAME_KEY, null);
            String initialDirectory = getArguments().getString(INITIAL_DIRECTORY_KEY, null);
            if (initialDirectory != null) {
                fileBrowser.setCurrentDirectory(new File(initialDirectory));
            }
            /*if (!GlobalDocumentFileStateHolder.initialFilePickerDirectoryIsNull()) {
                fileBrowser.setCurrentDirectory(GlobalDocumentFileStateHolder.getAndClearInitialFilePickerDirectory());
            }*/
            if (defaultOutputFilename != null) {
                fileNameEditText.setText(defaultOutputFilename);
            }
        }

        /*Change text to match theme*/
        if (SettingsHelper.getUseDarkTeme(getContext())) {
            currentPathTextView.setTextColor(((MainActivity)getActivity()).getDarkThemeColor(android.R.attr.textColorPrimary));
        }
    }
    ///////////////////////////////////
    //PRIVATE METHODS
    ///////////////////////////////////

    //the button listeners are in anonymous classes. They need to pass an instance of this FilePickerFragment but for them this points to the anonymous class, they can reach this method though
    private FilePickerFragment getSelfForButtonListeners() {
        return this;
    }

    //change to current and displayed directory to the homeDirectory
    private void goToHomeDirectory() {
        fileBrowser.setCurrentDirectory(fileBrowser.getHomeDirectory());
    }

    private void updateFileArrayAdapterFileList(List<File> files) {
        fileListAdapter.clear();
        fileListAdapter.addAll(files);
        /*
        TODO Might be necessary for directory traversal, needs special treatment in sorting comparator
        if (fileBrowser.getCurrentDirectory().getParentFile() != null) {
            fileListAdapter.add(new Pair<>(FileBrowser.PARENT_FILE_NAME, true));
        }*/
        fileListAdapter.notifyDataSetChanged();
    }

    /**
     * Return the first error encountered with writing to the new file as a String.
     * return empty string if there are no errors
     */
    private String checkFileErrors(File newFileParentDirectory, String filename) {
        String error = "";
        if (filename.isEmpty()) {
            error = getString(R.string.filename_cannot_be_empty);
        } else{
            /*TODO not sure if this line needs the path seperator. Does get absolute path include sperator at end of string?*/
            File existingFile = new File(newFileParentDirectory.getAbsolutePath()+File.pathSeparator+filename);
            if (existingFile.exists() && existingFile.isDirectory()) {
                error = getString(R.string.file_is_a_directory);
            }
        }
        return error;
    }

    private void sdCardMenuButtonOnClick() {
        /*
        TODO need a reliable way to check if user needs to add Permission in SAF or if can go
            straight to the sdcard. When multiple removable storage is supported: should make a list
            of removable storage locations in a dialog that can be selected to go to with an option
            to add more.
        String sdCardUriString = SettingsHelper.getSdcardRootTreeUri(getContext());
        if (sdCardUriString == null) {
            StorageAccessFrameworkHelper.findRemovableStorageWithDialog(getActivity());
        } else {*/
            changePathToSDCard();
        /*}*/
    }

    ///////////////////////
    //INTERNAL CLASSES
    ///////////////////////
    //intialize an instance of this and pass it to the contructor for FileListAdapter to change its getView behavior
    protected abstract class FileListAdapterGetViewCallback {
        public abstract View getView(int position, View convertView, ViewGroup parent, FileListAdapter fileListAdapter);
    }

    ///the adapter for the file list view
    protected class FileListAdapter extends BaseAdapter {
        private FileListAdapterGetViewCallback fileListAdapterGetViewCallback;
        //Pair<filename, isDirectory>
        private ArrayList<File> files = new ArrayList<>();

        public FileListAdapter() {
        }

        public void add(File file) {
            files.add(file);
        }

        public void addAll(Collection<File> collection) {
            files.addAll(collection);
        }

        public void addAll(File[] files) {
            for (File file: files) {
                this.files.add(file);
            }
        }

        public void sort() {
            Collections.sort(files, fileNameAlphabeticalComparator);
        }

        public void setFileListAdapterGetViewCallback(FileListAdapterGetViewCallback fileListAdapterGetViewCallback) {
            this.fileListAdapterGetViewCallback = fileListAdapterGetViewCallback;
        }

        public void clear() {
            files.clear();
        }

        @Override
        public int getCount() {
            return files.size();
        }

        @Override
        public File getItem(int i) {
            return files.get(i);
        }

        @Override
        public long getItemId(int i) {
            return files.get(i).hashCode();
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (fileListAdapterGetViewCallback == null) {
                throw new IllegalStateException("Get view callback wasn't set before calling getView.");
            }
            return fileListAdapterGetViewCallback.getView(position, convertView, parent, this);
        }

        @Override
        public void notifyDataSetChanged() {
            sort();
            super.notifyDataSetChanged();
        }
    }
}
