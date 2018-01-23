package com.dewdrop623.androidcrypt.FilePicker;

import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.provider.DocumentFile;
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

    //the list of files being displayed and the instance of FileBrowser
    private List<DocumentFile> fileList;
    private FileBrowser fileBrowser;

    //The list view to display the files
    protected AbsListView fileListView;
    private FileListAdapter fileListAdapter;

    ///////////////////////////////
    //ANONYMOUS CLASSES
    /////////////////////////////////

    //a comparator to help sort file alphabetically
    private Comparator<DocumentFile> documentFileAlphabeticalComparator = new Comparator<DocumentFile>() {
        @Override
        public int compare(DocumentFile file1, DocumentFile file2) {
            return file1.getName().toLowerCase().compareTo(file2.getName().toLowerCase());
        }
    };

    // the onClickListener for file items in the fileListView
    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            DocumentFile clickedFile = fileListAdapter.getItem(position);
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

    //called by the FileBrowser, updates the displayed list of files
    public void setFileList(List<DocumentFile> fileList) {
        this.fileList = fileList;
        updateFileArrayAdapterFileList();
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
            changeDirectory(DocumentFile.fromTreeUri(getContext(), Uri.parse(sdCardUriString)));
        }
    }

    ///////////////////////////////////
    //PROTECTED METHODS
    ///////////////////////////////////

    /**
     * Get the name for the document file to display in the GUI.
     * May be different than the actual directory name.
     * e.g. "Internal Storage" instead of "0" or ".." instead of the name of the parent folder.
     */
    protected String getGUINameForDocumentFile(DocumentFile documentFile) {
        String result;
        if (documentFile.equals(fileBrowser.getCurrentDirectory().getParentFile())) {
            result = "..";
        } else {
            result = documentFile.getName();
        }
        return result;
    }

    //after the concrete class inflates the view and defines the callback for the list adapter getview method, do the rest of the ui initialization work
    protected final void initializeFilePickerWithViewAndFileListAdapterGetViewCallback(View view, FileListAdapterGetViewCallback fileListAdapterGetViewCallback) {
        fileListAdapter = new FileListAdapter(fileListAdapterGetViewCallback);
        fileListView.setVisibility(View.VISIBLE);
        fileListView.setAdapter(fileListAdapter);
        fileListView.setVisibility(View.VISIBLE);
        fileListView.setOnItemClickListener(onItemClickListener);
        currentPathTextView = (TextView) view.findViewById(R.id.currentPathTextView);
        currentPathTextView.setMovementMethod(new ScrollingMovementMethod());
        currentPathTextView.setText(fileBrowser.getCurrentPathName());
        updateFileArrayAdapterFileList();

        fileNameInputLinearLayout = (LinearLayout) view.findViewById(R.id.fileNameInputLinearLayout);
        fileNameEditText = (EditText) view.findViewById(R.id.fileNameEditText);
        fileNameOkButton = (Button) view.findViewById(R.id.fileNameOkButton);

        if (isOutput) {
            fileNameInputLinearLayout.setVisibility(View.VISIBLE);
            fileNameOkButton.setOnClickListener(fileNameOkButtonOnClickListener);
        }

        String defaultOutputFilename = getArguments().getString(DEFAULT_OUTPUT_FILENAME_KEY, null);
        if (!GlobalDocumentFileStateHolder.initialFilePickerDirectoryIsNull()) {
            changeDirectory(GlobalDocumentFileStateHolder.getAndClearInitialFilePickerDirectory());
        }
        if (defaultOutputFilename != null) {
            fileNameEditText.setText(defaultOutputFilename);
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

    //sort the fileList member variable alphabetically
    private void sortFileList() {
        Collections.sort(fileList, documentFileAlphabeticalComparator);
    }


    private void updateFileArrayAdapterFileList() {
        if (fileListView == null) { //file viewer has not been displayed yet
            return;
        }
        fileListAdapter.clear();
        sortFileList();
        fileListAdapter.addAll(fileList);
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
        private ArrayList<DocumentFile> files = new ArrayList<>();

        public FileListAdapter(FileListAdapterGetViewCallback fileListAdapterGetViewCallback) {
            this.fileListAdapterGetViewCallback = fileListAdapterGetViewCallback;
        }

        public void add(DocumentFile file) {
            files.add(file);
        }

        public void addAll(Collection<DocumentFile> collection) {
            files.addAll(collection);
        }

        public void addAll(DocumentFile[] fileArray) {
            for (DocumentFile file : fileArray) {
                files.add(file);
            }
        }

        public void clear() {
            files.clear();
        }

        @Override
        public int getCount() {
            return files.size();
        }

        @Override
        public DocumentFile getItem(int i) {
            return files.get(i);
        }

        @Override
        public long getItemId(int i) {
            return files.get(i).hashCode();
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            return fileListAdapterGetViewCallback.getView(position, convertView, parent, this);
        }
    }
}
