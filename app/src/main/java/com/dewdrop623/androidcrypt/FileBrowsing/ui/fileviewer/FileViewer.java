package com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.View;
import android.widget.ImageButton;

import com.dewdrop623.androidcrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.MainActivity;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.DebugCreateDirectoryDialog;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.EncryptDecryptFileDialog;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.dialog.filedialog.FileDialog;
import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidcrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileMoveOperator;

import java.io.File;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * FileViewer is an abstract Fragment. It implements the common logic and contains the common UI elements across all types of FileViewers
 * FileViewer:
 * -contains the references to buttons for moving/copying files, selecting a directory, and canceling those operations (button appearance is left to the concrete subclass)
 * -assigns and implements the onclicklisteners for the previously mentioned buttons
 * -interacts with the FileBrowser instance when updating the display or modifiying files
 * -tracks the current state of whether a file is being moved/copied or a directory is being selected
 * -provides other common logic, such as the back button going up a directory, returning to the home directory, initiating the process of copy/move/select directory
 */

public abstract class FileViewer extends Fragment{

    ////////////////////////////////
    //MEMBER VARIABLES
    ////////////////////////////////

    //keys to remember states on rotation
    protected static final String MOVE_STATE_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.MOVE_STATE_KEY";
    protected static final String MOVE_COPY_FILE_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.MOVE_COPY_FILE_KEY";
    protected static final String CURRENT_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.CURRENT_DIRECTORY_KEY";

    //for storing the current state
    protected Bundle savedInstanceState = null;

    //represents the move and copy states
    protected final int MOVE = 2;
    protected final int COPY = 1;
    protected final int NONE = 0;

    //represents the current state of moving or copying
    private File moveCopyFile;
    protected int moveState = NONE;

    //references to the move/copy/select directory ui buttons
    protected ImageButton moveCopyButton;
    protected ImageButton cancelButton;
    protected ImageButton selectDirectoryButton;

    //the list of files being displayed and the instance of FileBrowser
    protected File[] fileList;
    protected FileBrowser fileBrowser;

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
    *the button listener for both the move/copy button
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

    /////////////////////////
    //PUBLIC METHODS
    /////////////////////////

    //create a FileBrowser and assign it to the member variable
    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fileBrowser = new FileBrowser(getContext());
        fileBrowser.setFileViewer(this);
        this.savedInstanceState = savedInstanceState;
        if (savedInstanceState != null) {
            fileBrowser.setCurrentPath(new File(savedInstanceState.getString(CURRENT_DIRECTORY_KEY,"/")));
        }
    }

    //save the current state for a screen rotation
    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putInt(MOVE_STATE_KEY, moveState);
        outState.putString(MOVE_COPY_FILE_KEY, (moveCopyFile==null)?"":moveCopyFile.getAbsolutePath());
        outState.putString(CURRENT_DIRECTORY_KEY, fileBrowser.getCurrentPath().getAbsolutePath());
        super.onSaveInstanceState(outState);

    }

    //called by the MainActivity when the back button is pressed, goes up one directory if not already at root
    public void onBackPressed() {
        if (!fileBrowser.getCurrentPath().equals(fileBrowser.root)) {
            fileBrowser.setCurrentPath(fileBrowser.parentDirectory);
        }
    }

    //meant to be called by the FileBrowser, this changes the list of files to be displayed
    public void setFileList(File[] fileList) {
        this.fileList=fileList;
    }

    //initiate a move file operation
    public void moveFile(File file) {
        moveState = MOVE;
        onMoveOrCopy(file);
    }

    //initiate a copy file operation
    public void copyFile(File file) {
        moveState = COPY;
        onMoveOrCopy(file);
    }

    //tell the FileBrowser to change the path
    public void changePath(File newPath) {
        fileBrowser.setCurrentPath(newPath);
    }

    //set the FileBrowser for this FileViewer
    public final void setFileBrowser(FileBrowser fileBrowser) {
        this.fileBrowser=fileBrowser;
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
        encryptDecryptDialogStateBundle = dialogState;
        isSelectingEncryptDecryptOutputDirectory = true;
        onSelectEncryptDecryptOutputDirectory();
    }

    ///////////////////////////////////
    //PROTECTED METHODS
    ///////////////////////////////////

    //open a dialog to create a new folder
    protected void createFolder() {
        DebugCreateDirectoryDialog debugCreateDirectoryDialog = new DebugCreateDirectoryDialog();
        Bundle args = new Bundle();
        args.putString(FileDialog.PATH_ARGUMENT, fileBrowser.getCurrentPath().getAbsolutePath());
        debugCreateDirectoryDialog.setArguments(args);
        debugCreateDirectoryDialog.setFileViewer(this);
        ((MainActivity)getActivity()).showDialogFragment(debugCreateDirectoryDialog);
    }

    //the button listeners are in anonymous classes. They need to pass an instance of this FileViewer but for them this points to the anonymous class, they can reach this method though
    protected final FileViewer getSelfForButtonListeners() {return this;}

    //gets called when EncryptDecryptFileDialog asks to select an output directory. Can be overridden by child class
    protected void onSelectEncryptDecryptOutputDirectory() {

    }
    //actions taken when a file copy/move operation is initiated by this.copyFile or this.moveFile, intended to be overrode by the concrete class but still call super()
    protected void onMoveOrCopy(File file) {
        moveCopyFile=file;
    }

    /*
      called by the click listener for the cancel button. can be overidden by child classes
      should reset the state for move/copy and encrypt/decrypt directory select operations
     */
    protected void resetDirectorySelectOperations() {
        moveState = NONE;
        moveCopyFile = null;
        encryptDecryptDialogStateBundle = null;
        isSelectingEncryptDecryptOutputDirectory = false;
    }

    //change to current and displayed directory to the homeDirectory
    protected final void goToHomeDirectory () {
        fileBrowser.setCurrentPath(FileBrowser.homeDirectory);
    }
    //after the concrete class assigns the button references, this method should be called to assign their onClick behavior
    protected final void setButtonListeners() {
        moveCopyButton.setOnClickListener(moveCopyButtonOnClickListener);
        cancelButton.setOnClickListener(cancelMoveCopyButtonOnClickListener);
        selectDirectoryButton.setOnClickListener(selectDirectoryButtonOnClickListener);
    }

    //sort the fileList member variable alphabetically
    protected final void sortFileList() {
        List<File> fileListObjects=Arrays.asList(fileList);
        Collections.sort(fileListObjects, fileObjectAlphabeticalComparator);
        fileList= (File[]) fileListObjects.toArray();
    }
}
