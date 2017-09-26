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
 * intended to be extended by other file viewers, each with a different UI for file browsing
 */

public abstract class FileViewer extends Fragment{

    protected static final String MOVE_STATE_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.MOVE_STATE_KEY";
    protected static final String MOVE_COPY_FILE_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.MOVE_COPY_FILE_KEY";
    protected static final String CURRENT_DIRECTORY_KEY = "com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer.FileViewer.CURRENT_DIRECTORY_KEY";

    protected Bundle savedInstanceState = null;

    protected final int MOVE = 2;
    protected final int COPY = 1;
    protected final int NONE = 0;

    private File moveCopyFile;
    protected int moveState = NONE;

    protected ImageButton moveCopyButton;
    protected ImageButton cancelButton;
    protected ImageButton selectDirectoryButton;

    protected File[] fileList;
    protected FileBrowser fileBrowser;
    private Comparator<File> fileObjectAlphabeticalComparator = new Comparator<File>() {
        @Override
        public int compare(File file, File t1) {
            return file.getName().toLowerCase().compareTo(t1.getName().toLowerCase());
        }
    };
    private boolean isSelectingEncryptDecryptOutputDirectory = false;
    private Bundle encryptDecryptDialogStateBundle = null;

    protected void createFolder() {
        DebugCreateDirectoryDialog debugCreateDirectoryDialog = new DebugCreateDirectoryDialog();
        Bundle args = new Bundle();
        args.putString(FileDialog.PATH_ARGUMENT, fileBrowser.getCurrentPath().getAbsolutePath());
        debugCreateDirectoryDialog.setArguments(args);
        debugCreateDirectoryDialog.setFileViewer(this);
        ((MainActivity)getActivity()).showDialogFragment(debugCreateDirectoryDialog);
    }
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
    private View.OnClickListener cancelMoveCopyButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            resetDirectorySelectOperations();
        }
    };
    private View.OnClickListener selectDirectoryButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {

            selectDirectoryOnClick();
            resetDirectorySelectOperations();
        }
    };
    //called by the on click listener by the directory select button. needs to be in a method to reference FileViewer using "this"
    private void selectDirectoryOnClick() {
        EncryptDecryptFileDialog encryptDecryptFileDialog = new EncryptDecryptFileDialog();
        encryptDecryptDialogStateBundle.putString(EncryptDecryptFileDialog.OUTPUT_DIRECTORY_KEY, fileBrowser.getCurrentPath().getAbsolutePath());
        encryptDecryptFileDialog.setArguments(encryptDecryptDialogStateBundle);
        encryptDecryptFileDialog.setFileViewer(this);
        ((MainActivity)getActivity()).showDialogFragment(encryptDecryptFileDialog);
    }
    protected final FileViewer getSelfForButtonListeners() {return this;}
    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FileBrowser fileBrowser = new FileBrowser();
        fileBrowser.setFileViewer(this);
        this.savedInstanceState = savedInstanceState;
        if (savedInstanceState != null) {
            fileBrowser.setCurrentPath(new File(savedInstanceState.getString(CURRENT_DIRECTORY_KEY,"/")));
        }
    }

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

    //methods that the subclass cannot override
    final public void setFileBrowser(FileBrowser fileBrowser) {
        this.fileBrowser=fileBrowser;
    }
    final public void sendFileCommandToFileBrowser(Bundle args) {
        fileBrowser.modifyFile(args);
    }
    final protected void goToHomeDirectory () {
        fileBrowser.setCurrentPath(FileBrowser.topLevelInternal);
    }
    final protected void setButtonListeners() {
        moveCopyButton.setOnClickListener(moveCopyButtonOnClickListener);
        cancelButton.setOnClickListener(cancelMoveCopyButtonOnClickListener);
        selectDirectoryButton.setOnClickListener(selectDirectoryButtonOnClickListener);
    }

    //sort the fileList member variable alphabetically
    final protected void sortFileList() {
        List<File> fileListObjects=Arrays.asList(fileList);
        Collections.sort(fileListObjects, fileObjectAlphabeticalComparator);
        fileList= (File[]) fileListObjects.toArray();
    }
    //methods that sublass can override, but should call super
    public void setFileList(File[] fileList) {
        this.fileList=fileList;
    }
    public void moveFile(File file) {
        moveState = MOVE;
        onMoveOrCopy(file);
    }
    public void copyFile(File file) {
        moveState = COPY;
        onMoveOrCopy(file);
    }
    public void changePath(File newPath) {
        fileBrowser.setCurrentPath(newPath);
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

    //gets called when EncryptDecryptFileDialog asks to select an output directory. Can be overridden by child class
    protected void onSelectEncryptDecryptOutputDirectory() {

    }

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

}
