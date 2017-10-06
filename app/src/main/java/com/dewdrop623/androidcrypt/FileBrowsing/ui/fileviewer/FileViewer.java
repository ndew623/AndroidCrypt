package com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.content.res.ResourcesCompat;
import android.text.method.ScrollingMovementMethod;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ImageButton;
import android.widget.TextView;

import com.dewdrop623.androidcrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.MainActivity;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.DebugCreateDirectoryDialog;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.EncryptDecryptFileDialog;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.FileDialog;
import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidcrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidcrypt.R;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * FileViewer is an abstract Fragment. It implements the common logic and contains the common UI elements across all types of FileViewers
 * FileViewer:
 * -contains the references to the UI elements
 * -implements the UI logic
 * -interacts with the FileBrowser to update the display and modify files
 *
 * Concrete implementations of FileViewer must:
 * -override onCreateView and inflate a layout with all of the view ids that FileViewer needs
 * -get an AbsListView instance from the inflated layout
 * -implement a FileListAdapterGetViewCallback to define the logic of FileListAdapter's getView method
 * -call initializeFileViewerWithViewAndFileListAdapterGetViewCallback
 */

public abstract class FileViewer extends Fragment{

    ////////////////////////////////
    //MEMBER VARIABLES
    ////////////////////////////////

    //keys to remember state on rotation
    private static final String MOVE_STATE_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.MOVE_STATE_KEY";
    private static final String MOVE_COPY_FILE_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.MOVE_COPY_FILE_KEY";
    private static final String CURRENT_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.CURRENT_DIRECTORY_KEY";
    private static final String IS_SELECTING_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.IS_SELECTING_DIRECTORY_KEY";
    private static final String ENCRYPT_DECRYPT_DIALOG_STATE_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.ENCRYPT_DECRYPT_DIALOG_STATE_KEY";

    //for storing the current state
    private Bundle savedInstanceState = null;

    //represents the move and copy states
    private final int MOVE = 2;
    private final int COPY = 1;
    private final int NONE = 0;

    //represents the current state of moving or copying
    private File moveCopyFile;
    private int moveState = NONE;

    //references to the move/copy/select directory ui buttons
    private ImageButton moveCopyButton;
    private ImageButton cancelButton;
    private ImageButton selectDirectoryButton;

    //the TextView that shows the current directory under the actionbar
    private TextView currentPathTextView;

    //the list of files being displayed and the instance of FileBrowser
    private File[] fileList;
    private FileBrowser fileBrowser;

    //The list view to display the files
    protected AbsListView fileListView;
    private FileListAdapter fileListAdapter;

    //represent the selecting directory state
    private boolean isSelectingEncryptDecryptOutputDirectory = false;

    //stores information about the encryptDecryptDialog while a directory is selected so that the dialog can be recreated
    private Bundle encryptDecryptDialogStateBundle = null;

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

    /*
    *the button listener for the move/copy button
     */
    private View.OnClickListener moveCopyButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if(moveState == NONE) {
                return;
            }
            Bundle args = new Bundle();
            int fileOperationType = -1;
            if(moveState == MOVE) {
                args.putString(FileMoveOperator.FILE_MOVE_DESTINATION_ARG, fileBrowser.getCurrentPath().getAbsolutePath());
                fileOperationType = FileOperationType.MOVE;
            } else if (moveState == COPY) {
                args.putString(FileCopyOperator.FILE_COPY_DESTINATION_ARG, fileBrowser.getCurrentPath().getAbsolutePath());
                fileOperationType = FileOperationType.COPY;
            }
            args.putInt(FileModifierService.FILEMODIFIERSERVICE_OPERATIONTYPE, fileOperationType);
            args.putString(FileModifierService.FILEMODIFIERSERVICE_FILE, moveCopyFile.getAbsolutePath());
            sendFileCommandToFileBrowser(args);
            resetDirectorySelectOperations();
        }
    };

    //the listener for the cancel button
    private View.OnClickListener cancelMoveCopyButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            resetDirectorySelectOperations();
        }
    };

    /*
    *the listener for the selectDirectoryButton
    *reopens the encryptDecrypt dialog with the selected output directory
    */
    private View.OnClickListener selectDirectoryButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            EncryptDecryptFileDialog encryptDecryptFileDialog = new EncryptDecryptFileDialog();
            encryptDecryptDialogStateBundle.putString(EncryptDecryptFileDialog.OUTPUT_DIRECTORY_KEY, fileBrowser.getCurrentPath().getAbsolutePath());
            encryptDecryptFileDialog.setArguments(encryptDecryptDialogStateBundle);
            encryptDecryptFileDialog.setFileViewer(getSelfForButtonListeners());
            ((MainActivity)getActivity()).showDialogFragment(encryptDecryptFileDialog);
            resetDirectorySelectOperations();
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
                ((MainActivity) getActivity()).openOptionsDialog(clickedFile, getSelfForButtonListeners());
            }
        }
    };

    //the onLongClickListener for file items in the fileListView
    private AdapterView.OnItemLongClickListener onItemLongClickListener = new AdapterView.OnItemLongClickListener() {
        @Override
        public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileListAdapter.getItem(position);
            if (clickedFile != FileBrowser.parentDirectory) {
                ((MainActivity) getActivity()).openOptionsDialog(clickedFile, getSelfForButtonListeners());
            }
            return true;
        }
    };

    /////////////////////////
    //PUBLIC METHODS
    /////////////////////////

    /*create a FileBrowser and assign it to the member variable
    * get the savedInstanceState and put it in a member variable
    * */
    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
        fileBrowser = new FileBrowser(getContext());
        fileBrowser.setFileViewer(this);
        this.savedInstanceState = savedInstanceState;
        if (savedInstanceState != null) {
            fileBrowser.setCurrentPath(new File(savedInstanceState.getString(CURRENT_DIRECTORY_KEY,"/")));
        }
    }

    //create the options menu
    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.file_viewer_menu, menu);
    }

    //save the current state for a screen rotation
    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putInt(MOVE_STATE_KEY, moveState);
        outState.putString(MOVE_COPY_FILE_KEY, (moveCopyFile==null)?"":moveCopyFile.getAbsolutePath());
        outState.putString(CURRENT_DIRECTORY_KEY, fileBrowser.getCurrentPath().getAbsolutePath());
        outState.putBoolean(IS_SELECTING_DIRECTORY_KEY, isSelectingEncryptDecryptOutputDirectory);
        outState.putBundle(ENCRYPT_DECRYPT_DIALOG_STATE_KEY, encryptDecryptDialogStateBundle);
        super.onSaveInstanceState(outState);

    }

    //define behavior for the options menu
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.home_button:
                goToHomeDirectory();
                return true;
            case R.id.refresh_button:
                //setting the fileBrowser to its current path will cause it to refresh FileViewer
                fileBrowser.setCurrentPath(fileBrowser.getCurrentPath());
                return true;
            case R.id.create_folder_button:
                createFolder();
                return true;
        }
        return false;
    }

    //called by the MainActivity when the back button is pressed, goes up one directory if not already at root
    public void onBackPressed() {
        if (!fileBrowser.getCurrentPath().equals(FileBrowser.root)) {
            fileBrowser.setCurrentPath(FileBrowser.parentDirectory);
        }
    }

    //meant to be called by the FileBrowser, updates the displayed list of files
    public void setFileList(File[] fileList) {
        this.fileList=fileList;
        updateFileArrayAdapterFileList();
    }

    //initiate a move file operation
    public void moveFile(File file) {
        resetDirectorySelectOperations();
        moveState = MOVE;
        moveCopyButton.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.ic_move, null));
        onMoveOrCopy(file);
    }

    //initiate a copy file operation
    public void copyFile(File file) {
        resetDirectorySelectOperations();
        moveState = COPY;
        moveCopyButton.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.ic_copy, null));
        onMoveOrCopy(file);
    }

    //tell the FileBrowser to change the path
    public void changePath(File newPath) {
        fileBrowser.setCurrentPath(newPath);
    }

    //send a Bundle with the necessary arguments to FileBrowser's modifyFile to be executed as a command
    public final void sendFileCommandToFileBrowser(Bundle args) {
        fileBrowser.modifyFile(args);
    }

    /*
        allows user to select a directory to output from the encrypt/decrypt operation. puts that directory into dialogState.
        EncryptDecryptFileDialog must put all necessary state information into the bundle
        it will be sent to a new EncryptDecryptFileDialog as arguments with a true boolean argument to indicate that there is state information in the bundle
     */
    public final void selectEncryptDecryptOutputDirectory(Bundle dialogState) {
        resetDirectorySelectOperations();
        encryptDecryptDialogStateBundle = dialogState;
        isSelectingEncryptDecryptOutputDirectory = true;
        selectDirectoryButton.setVisibility(View.VISIBLE);
        cancelButton.setVisibility(View.VISIBLE);
    }

    ///////////////////////////////////
    //PROTECTED METHODS
    ///////////////////////////////////

    //after the concrete class inflates the view and defines the callback for the list adapter getview method, do the rest of the ui initialization work
    protected final void initializeFileViewerWithViewAndFileListAdapterGetViewCallback(View view, FileListAdapterGetViewCallback fileListAdapterGetViewCallback) {
        fileListAdapter = new FileListAdapter(fileListAdapterGetViewCallback);
        fileListView.setVisibility(View.VISIBLE);
        fileListView.setAdapter(fileListAdapter);
        fileListView.setVisibility(View.VISIBLE);
        fileListView.setOnItemClickListener(onItemClickListener);
        fileListView.setOnItemLongClickListener(onItemLongClickListener);
        currentPathTextView = (TextView) view.findViewById(R.id.currentPathTextView);
        moveCopyButton = (ImageButton) view.findViewById(R.id.moveCopyButton);
        cancelButton = (ImageButton) view.findViewById(R.id.cancelMoveCopyButton);
        selectDirectoryButton = (ImageButton) view.findViewById(R.id.selectDirectoryButton);
        currentPathTextView.setMovementMethod(new ScrollingMovementMethod());

        moveCopyButton.setOnClickListener(moveCopyButtonOnClickListener);
        cancelButton.setOnClickListener(cancelMoveCopyButtonOnClickListener);
        selectDirectoryButton.setOnClickListener(selectDirectoryButtonOnClickListener);

        updateFileArrayAdapterFileList();

        if (savedInstanceState != null) {
            int prevMoveState = savedInstanceState.getInt(MOVE_STATE_KEY, 0);
            if (prevMoveState != 0) {
                String prevMoveCopyFile = savedInstanceState.getString(MOVE_COPY_FILE_KEY, MOVE_COPY_FILE_KEY);
                if (!prevMoveCopyFile.equals(MOVE_COPY_FILE_KEY)) {
                    if (prevMoveState == COPY) {
                        copyFile(new File(prevMoveCopyFile));
                    } else if (prevMoveState == MOVE) {
                        moveFile(new File(prevMoveCopyFile));
                    }
                }
            }
            isSelectingEncryptDecryptOutputDirectory = savedInstanceState.getBoolean(IS_SELECTING_DIRECTORY_KEY, false);
            if (isSelectingEncryptDecryptOutputDirectory) {
                selectEncryptDecryptOutputDirectory(savedInstanceState.getBundle(ENCRYPT_DECRYPT_DIALOG_STATE_KEY));
            }
        }
    }
    ///////////////////////////////////
    //PRIVATE METHODS
    ///////////////////////////////////

    //open a dialog to create a new folder
    private void createFolder() {
        DebugCreateDirectoryDialog debugCreateDirectoryDialog = new DebugCreateDirectoryDialog();
        Bundle args = new Bundle();
        args.putString(FileDialog.PATH_ARGUMENT, fileBrowser.getCurrentPath().getAbsolutePath());
        debugCreateDirectoryDialog.setArguments(args);
        debugCreateDirectoryDialog.setFileViewer(this);
        ((MainActivity)getActivity()).showDialogFragment(debugCreateDirectoryDialog);
    }

    //the button listeners are in anonymous classes. They need to pass an instance of this FileViewer but for them this points to the anonymous class, they can reach this method though
    private FileViewer getSelfForButtonListeners() {return this;}

    //actions taken when a file copy/move operation is initiated by this.copyFile or this.moveFile
    private void onMoveOrCopy(File file) {
        moveCopyFile=file;
        moveCopyButton.setVisibility(View.VISIBLE);
        cancelButton.setVisibility(View.VISIBLE);
    }

    /*
      called by the click listener for the cancel button.
      should reset the state for move/copy and encrypt/decrypt directory select operations
     */
    private void resetDirectorySelectOperations() {
        moveState = NONE;
        moveCopyFile = null;
        encryptDecryptDialogStateBundle = null;
        isSelectingEncryptDecryptOutputDirectory = false;
        moveCopyButton.setVisibility(View.GONE);
        cancelButton.setVisibility(View.GONE);
        selectDirectoryButton.setVisibility(View.GONE);
    }

    //change to current and displayed directory to the homeDirectory
    private void goToHomeDirectory () {
        fileBrowser.setCurrentPath(FileBrowser.homeDirectory);
    }

    //sort the fileList member variable alphabetically
    private void sortFileList() {
        List<File> fileListObjects=Arrays.asList(fileList);
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
