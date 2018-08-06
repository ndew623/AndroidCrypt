package com.dewdrop623.androidcrypt.FilePicker;

import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.provider.DocumentFile;
import android.support.v7.app.AlertDialog;
import android.text.method.ScrollingMovementMethod;
import android.util.Pair;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.dewdrop623.androidcrypt.GlobalDocumentFileStateHolder;
import com.dewdrop623.androidcrypt.MainActivity;
import com.dewdrop623.androidcrypt.R;
import com.dewdrop623.androidcrypt.SettingsHelper;
import com.dewdrop623.androidcrypt.StorageAccessFrameworkHelper;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * FilePicker fragment allows users to select a file.
 * Child fragments determine what implementation of android's AbsListView to use.
 * Child fragments define the behavior for the listviewadapter's getView method.
 */

public abstract class FilePicker extends Fragment {

    /**
     * Keys for arguments bundle
     */
    public static final String IS_OUTPUT_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePicker.IS_OUTPUT_KEY";
    public static final String DEFAULT_OUTPUT_FILENAME_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePicker.DEFAULT_OUTPUT_FILENAME_KEY";

    ////////////////////////////////
    //MEMBER VARIABLES
    ///////////////////////////////

    //for storing the current state
    private boolean isOutput;

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
    private Comparator<Pair<String, Boolean>> fileNameAlphabeticalComparator = new Comparator<Pair<String, Boolean>>() {
        @Override
        public int compare(Pair<String, Boolean> file1, Pair<String, Boolean> file2) {
            int result;
            if (file1.first.equals(FileBrowser.PARENT_FILE_NAME)) {
                result = -1;
            } else if (file2.first.equals(FileBrowser.PARENT_FILE_NAME)) {
                result = 1;
            } else {
                result = file1.first.toLowerCase().compareTo(file2.first.toLowerCase());
            }
            return result;
        }
    };

    // the onClickListener for file items in the fileListView
    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            String clickedFileName = fileListAdapter.getItem(position).first;
            DocumentFile clickedFile;
            if (clickedFileName.equals(FileBrowser.PARENT_FILE_NAME)) {
                clickedFile = fileBrowser.getCurrentDirectory().getParentFile();
            } else {
                clickedFile = fileBrowser.getCurrentDirectory().findFile(clickedFileName);
            }
            if (clickedFile.isDirectory()) {
                fileBrowser.setCurrentDirectory(clickedFile);
            } else {
                if (isOutput) {
                    fileNameEditText.setText(clickedFile.getName());
                } else {
                    ((MainActivity) getActivity()).filePicked(clickedFile.getParentFile(), clickedFile.getName(), isOutput);
                    ((MainActivity) getActivity()).superOnBackPressed();
                }
            }
        }
    };

    private View.OnClickListener fileNameOkButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            String fileName = fileNameEditText.getText().toString();
            DocumentFile newFileParentDirectory = fileBrowser.getCurrentDirectory();
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
        fileBrowser.setFilePicker(this);
        isOutput = getArguments().getBoolean(FilePicker.IS_OUTPUT_KEY, false);
        if (!GlobalDocumentFileStateHolder.savedCurrentDirectoryForRotateIsNull()) {
            fileBrowser.setCurrentDirectory(GlobalDocumentFileStateHolder.getAndClearSavedCurrentDirectoryForRotate());
        }
    }

    //save the current state for a screen rotation
    @Override
    public void onSaveInstanceState(Bundle outState) {
        GlobalDocumentFileStateHolder.setSavedCurrentDirectoryForRotate(fileBrowser.getCurrentDirectory());
        super.onSaveInstanceState(outState);
    }

    //define behavior for the options menu
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.sdcard_button:
                sdCardMenuButtonOnClick();
                break;
            case R.id.home_button:
                goToHomeDirectory();
                return true;
            case R.id.refresh_button:
                fileBrowser.updateFileViewer();
                return true;
        }
        return false;
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
    public void setFileList(List<DocumentFile> documentFiles) {
        updateFileArrayAdapterFileList(documentFiles);
        if (currentPathTextView != null) {
            currentPathTextView.setText(fileBrowser.getCurrentPathName());
        }
    }

    //tell the FileBrowser to change the path
    public void changeDirectory(DocumentFile newDirectory) {
        fileBrowser.setCurrentDirectory(newDirectory);
    }

    /**
     * Exists so MainActivity can call this after SAF Activity for selecting SD card completes.
     */
    public void changePathToSDCard() {
        String sdCardUriString = SettingsHelper.getSdcardRoot(getContext());
        if (StorageAccessFrameworkHelper.canSupportSDCardOnAndroidVersion() && sdCardUriString != null) {
            //cache the current directory to recover if changing to the sd card fails
            DocumentFile currentDirectory = fileBrowser.getCurrentDirectory();
            try {
                changeDirectory(DocumentFile.fromTreeUri(getContext(), Uri.parse(sdCardUriString)));
            } catch (NullPointerException npe) {
                changeDirectory(currentDirectory);
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
        currentPathTextView = (TextView) view.findViewById(R.id.currentPathTextView);
        currentPathTextView.setMovementMethod(new ScrollingMovementMethod());
        currentPathTextView.setText(fileBrowser.getCurrentPathName());

        fileNameInputLinearLayout = (LinearLayout) view.findViewById(R.id.fileNameInputLinearLayout);
        fileNameEditText = (EditText) view.findViewById(R.id.fileNameEditText);
        fileNameOkButton = (Button) view.findViewById(R.id.fileNameOkButton);

        if (isOutput) {
            fileNameInputLinearLayout.setVisibility(View.VISIBLE);
            fileNameEditText.setTextColor(Color.BLACK);
            fileNameOkButton.setOnClickListener(fileNameOkButtonOnClickListener);

            String defaultOutputFilename = getArguments().getString(DEFAULT_OUTPUT_FILENAME_KEY, null);
            if (!GlobalDocumentFileStateHolder.initialFilePickerDirectoryIsNull()) {
                changeDirectory(GlobalDocumentFileStateHolder.getAndClearInitialFilePickerDirectory());
            }
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

    //the button listeners are in anonymous classes. They need to pass an instance of this FilePicker but for them this points to the anonymous class, they can reach this method though
    private FilePicker getSelfForButtonListeners() {
        return this;
    }

    //change to current and displayed directory to the homeDirectory
    private void goToHomeDirectory() {
        fileBrowser.setCurrentDirectory(FileBrowser.internalStorageHome);
    }

    private void updateFileArrayAdapterFileList(List<DocumentFile> documentFiles) {
        fileListAdapter.clear();
        for(DocumentFile documentFile : documentFiles) {
            fileListAdapter.add(new Pair<>(documentFile.getName(), documentFile.isDirectory()));
        }
        if (fileBrowser.getCurrentDirectory().getParentFile() != null) {
            fileListAdapter.add(new Pair<>(FileBrowser.PARENT_FILE_NAME, true));
        }
        fileListAdapter.notifyDataSetChanged();
    }

    /**
     * Return the first error encountered with writing to the new file as a String.
     * return empty string if there are no errors
     */
    private String checkFileErrors(DocumentFile newFileParentDirectory, String filename) {
        String error = "";
        if (filename.isEmpty()) {
            error = getString(R.string.filename_cannot_be_empty);
        } else{
            DocumentFile existingFile = newFileParentDirectory.findFile(filename);
            if (existingFile != null && existingFile.isDirectory()) {
                error = getString(R.string.file_is_a_directory);
            }
        }
        return error;
    }

    private void sdCardMenuButtonOnClick() {
        String sdCardUriString = SettingsHelper.getSdcardRoot(getContext());
        if (sdCardUriString == null) {
            StorageAccessFrameworkHelper.findSDCardWithDialog(getActivity());
        } else {
            changePathToSDCard();
        }
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
        private ArrayList<Pair<String, Boolean>> filenames = new ArrayList<>();

        public FileListAdapter() {
        }

        public void add(Pair<String, Boolean> filename) {
            filenames.add(filename);
        }

        public void addAll(Collection<Pair<String, Boolean>> collection) {
            filenames.addAll(collection);
        }

        public void addAll(Pair<String, Boolean>[] fileNameArray) {
            for (Pair<String, Boolean> filename : fileNameArray) {
                filenames.add(filename);
            }
        }

        public void sort() {
            Collections.sort(filenames, fileNameAlphabeticalComparator);
        }

        public void setFileListAdapterGetViewCallback(FileListAdapterGetViewCallback fileListAdapterGetViewCallback) {
            this.fileListAdapterGetViewCallback = fileListAdapterGetViewCallback;
        }

        public void clear() {
            filenames.clear();
        }

        @Override
        public int getCount() {
            return filenames.size();
        }

        @Override
        public Pair<String, Boolean> getItem(int i) {
            return filenames.get(i);
        }

        @Override
        public long getItemId(int i) {
            return filenames.get(i).hashCode();
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
