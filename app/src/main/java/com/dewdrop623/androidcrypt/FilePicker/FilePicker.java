package com.dewdrop623.androidcrypt.FilePicker;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
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

import com.dewdrop623.androidcrypt.MainActivity;
import com.dewdrop623.androidcrypt.R;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * TODO comment on class
 */

public abstract class FilePicker extends Fragment {

    /**
     * Keys for arguments bundle
     */
    public static final String IS_OUTPUT_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePicker.IS_OUTPUT_KEY";
    public static final String INITIAL_FOLDER_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePicker.INITIAL_FOLDER_KEY";
    public static final String DEFAULT_OUTPUT_FILENAME_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePicker.DEFAULT_OUTPUT_FILENAME_KEY";

    ////////////////////////////////
    //MEMBER VARIABLES
    ///////////////////////////////

    //keys to remember state on rotation
    private static final String CURRENT_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FilePicker.FilePicker.CURRENT_DIRECTORY_KEY";

    //for storing the current state
    private Bundle savedInstanceState = null;
    private boolean isOutput;

    //the TextView that shows the current directory under the actionbar
    private TextView currentPathTextView;
    private LinearLayout fileNameInputLinearLayout;
    private EditText fileNameEditText;
    private Button fileNameOkButton;

    //the list of files being displayed and the instance of FileBrowser
    private File[] fileList;
    private FileBrowser fileBrowser;

    //The list view to display the files
    protected AbsListView fileListView;
    private FileListAdapter fileListAdapter;

    ///////////////////////////////
    //ANONYMOUS CLASSES
    /////////////////////////////////

    //a comparator to help sort file alphabetically
    private Comparator<File> fileObjectAlphabeticalComparator = new Comparator<File>() {
        @Override
        public int compare(File file, File t1) {
            return file.getName().toLowerCase().compareTo(t1.getName().toLowerCase());
        }
    };

    // the onClickListener for file items in the fileListView
    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileListAdapter.getItem(position);
            if (clickedFile.isDirectory()) {
                fileBrowser.setCurrentPath(clickedFile);
            } else {
                if (isOutput) {
                    fileNameEditText.setText(clickedFile.getName());
                } else {
                    ((MainActivity) getActivity()).filePicked(clickedFile, isOutput);
                    ((MainActivity) getActivity()).superOnBackPressed();
                }
            }
        }
    };

    private View.OnClickListener fileNameOkButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            String fileName = fileNameEditText.getText().toString();
            File newFile = new File(fileBrowser.getCurrentPath().getAbsolutePath().concat(File.separator).concat(fileName));
            String error = "";//TODO fix checkFileErrors to be compatible with SAF approach checkFileErrors(newFile);
            if (error.isEmpty()) {
                ((MainActivity) getActivity()).filePicked(newFile, isOutput);
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
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
        fileBrowser = new FileBrowser(getContext());
        fileBrowser.setFilePicker(this);
        isOutput = getArguments().getBoolean(FilePicker.IS_OUTPUT_KEY, false);
        this.savedInstanceState = savedInstanceState;
        if (savedInstanceState != null) {
            fileBrowser.setCurrentPath(new File(savedInstanceState.getString(CURRENT_DIRECTORY_KEY, "/")));
        }
    }

    //save the current state for a screen rotation
    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putString(CURRENT_DIRECTORY_KEY, fileBrowser.getCurrentPath().getAbsolutePath());
        super.onSaveInstanceState(outState);
    }

    //define behavior for the options menu
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.sdcard_button:
                //TODO something
                break;
            case R.id.home_button:
                goToHomeDirectory();
                return true;
            case R.id.refresh_button:
                //setting the fileBrowser to its current path will cause it to refresh FilePicker
                fileBrowser.setCurrentPath(fileBrowser.getCurrentPath());
                return true;
        }
        return false;
    }

    /**
     * called by the MainActivity when the back button is pressed, goes up one directory if not already at root
     * if at root: regular back button behavior
     */
    public void onBackPressed() {
        if (!fileBrowser.getCurrentPath().equals(FileBrowser.root)) {
            fileBrowser.setCurrentPath(FileBrowser.parentDirectory);
        } else {
            ((MainActivity) getActivity()).superOnBackPressed();
        }
    }

    //called by the FileBrowser, updates the displayed list of files
    public void setFileList(File[] fileList) {
        this.fileList = fileList;
        updateFileArrayAdapterFileList();
    }

    //tell the FileBrowser to change the path
    public void changePath(File newPath) {
        fileBrowser.setCurrentPath(newPath);
    }

    ///////////////////////////////////
    //PROTECTED METHODS
    ///////////////////////////////////

    //after the concrete class inflates the view and defines the callback for the list adapter getview method, do the rest of the ui initialization work
    protected final void initializeFilePickerWithViewAndFileListAdapterGetViewCallback(View view, FileListAdapterGetViewCallback fileListAdapterGetViewCallback) {
        fileListAdapter = new FileListAdapter(fileListAdapterGetViewCallback);
        fileListView.setVisibility(View.VISIBLE);
        fileListView.setAdapter(fileListAdapter);
        fileListView.setVisibility(View.VISIBLE);
        fileListView.setOnItemClickListener(onItemClickListener);
        currentPathTextView = (TextView) view.findViewById(R.id.currentPathTextView);
        currentPathTextView.setMovementMethod(new ScrollingMovementMethod());
        updateFileArrayAdapterFileList();

        fileNameInputLinearLayout = (LinearLayout) view.findViewById(R.id.fileNameInputLinearLayout);
        fileNameEditText = (EditText)  view.findViewById(R.id.fileNameEditText);
        fileNameOkButton = (Button) view.findViewById(R.id.fileNameOkButton);

        if (isOutput) {
            fileNameInputLinearLayout.setVisibility(View.VISIBLE);
            fileNameOkButton.setOnClickListener(fileNameOkButtonOnClickListener);
        }

        String initialFolder = getArguments().getString(INITIAL_FOLDER_KEY, null);
        String defaultOutputFilename = getArguments().getString(DEFAULT_OUTPUT_FILENAME_KEY, null);
        if (initialFolder != null) {
            fileBrowser.setCurrentPath(new File(initialFolder));
        }
        if (defaultOutputFilename!=null) {
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
        fileBrowser.setCurrentPath(FileBrowser.homeDirectory);
    }

    //sort the fileList member variable alphabetically
    private void sortFileList() {
        List<File> fileListObjects = Arrays.asList(fileList);
        Collections.sort(fileListObjects, fileObjectAlphabeticalComparator);
        fileList = (File[]) fileListObjects.toArray();
    }


    private void updateFileArrayAdapterFileList() {
        if (fileListView == null) { //file viewer has not been displayed yet
            return;
        }
        fileListAdapter.clear();
        if (!fileBrowser.getCurrentPath().equals(FileBrowser.root)) {
            fileListAdapter.add(FileBrowser.parentDirectory);
        }
        sortFileList();
        fileListAdapter.addAll(fileList);
        fileListAdapter.notifyDataSetChanged();

        currentPathTextView.setText(fileBrowser.getCurrentPath().getAbsolutePath());
    }

    /**
     * Return the first error encountered with writing to the new file as a String.
     * return empty string if there are no errors
     */
    private String checkFileErrors(File newFile) {
        String error = "";
        if (newFile.getAbsolutePath().equals(fileBrowser.getCurrentPath().getAbsolutePath())) {
            error = getString(R.string.filename_cannot_be_empty);
        } else if (newFile.isDirectory()) {
            error = getString(R.string.file_is_a_directory);
        } else if (!newFile.exists()) {
            boolean created = false;
            try {
                created = newFile.createNewFile();
            } catch (IOException ioe) {
                error = ioe.getMessage();
            }
            if (created) {
                newFile.delete();
            }
        }
        return error;
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
        private ArrayList<File> files = new ArrayList();

        public FileListAdapter(FileListAdapterGetViewCallback fileListAdapterGetViewCallback) {
            this.fileListAdapterGetViewCallback = fileListAdapterGetViewCallback;
        }

        public void add(File file) {
            files.add(file);
        }

        public void addAll(Collection<File> collection) {
            files.addAll(collection);
        }

        public void addAll(File[] fileArray) {
            for (File file : fileArray) {
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
        public File getItem(int i) {
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
