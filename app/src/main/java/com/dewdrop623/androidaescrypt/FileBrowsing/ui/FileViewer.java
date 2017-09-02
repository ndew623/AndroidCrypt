package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.DebugCreateDirectoryDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.DebugFileOptionsDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.FileDialog;
import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidaescrypt.MainActivity;

import java.io.File;

/**
 * intended to be extended by other file viewers, each with a different UI for file browsing
 */

public abstract class FileViewer extends Fragment{

    protected static final String MOVE_STATE_KEY = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.FileViewer.MOVE_STATE_KEY";
    protected static final String MOVE_COPY_FILE_KEY = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.FileViewer.MOVE_COPY_FILE_KEY";
    protected static final String CURRENT_DIRECTORY_KEY = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.FileViewer.CURRENT_DIRECTORY_KEY";

    protected Bundle savedInstanceState = null;

    protected final int MOVE = 2;
    protected final int COPY = 1;
    protected final int NONE = 0;

    private File moveCopyFile;
    protected int moveState = NONE;

    protected ImageButton moveCopyButton;
    protected ImageButton cancelMoveCopyButton;

    protected File[] fileList;
    protected FileBrowser fileBrowser;

    protected void createFolder() {
        DebugCreateDirectoryDialog debugCreateDirectoryDialog = new DebugCreateDirectoryDialog();
        Bundle args = new Bundle();
        args.putString(FileDialog.PATH_ARGUMENT, fileBrowser.getCurrentPath().getAbsolutePath());
        debugCreateDirectoryDialog.setArguments(args);
        debugCreateDirectoryDialog.setFileViewer(getSelfForButtonListeners());
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
            moveCopyReset();
        }
    };
    private View.OnClickListener cancelMoveCopyButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            moveCopyReset();
        }
    };

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FileBrowser fileBrowser = new FileBrowser();
        fileBrowser.setFileViewer(this);
        this.savedInstanceState = savedInstanceState;
        if (savedInstanceState != null) {
            fileBrowser.changePath(new File(savedInstanceState.getString(CURRENT_DIRECTORY_KEY,"/")));
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putInt(MOVE_STATE_KEY, moveState);
        outState.putString(MOVE_COPY_FILE_KEY, (moveCopyFile==null)?"":moveCopyFile.getAbsolutePath());
        outState.putString(CURRENT_DIRECTORY_KEY, fileBrowser.getCurrentPath().getAbsolutePath());
        super.onSaveInstanceState(outState);

    }

    //methods that the subclass cannot override
    final public void setFileBrowser(FileBrowser fileBrowser) {
        this.fileBrowser=fileBrowser;
    }
    final public void sendFileCommandToFileBrowser(Bundle args) {
        fileBrowser.modifyFile(args);
    }
    final protected void goToHomeDirectory () {
        fileBrowser.changePath(FileBrowser.topLevelInternal);
    }
    final protected void openOptionsDialog(File clickedFile) {
        DebugFileOptionsDialog debugFileOptionsDialog = new DebugFileOptionsDialog();
        Bundle args = new Bundle();
        args.putString(FileDialog.PATH_ARGUMENT, clickedFile.getAbsolutePath());
        debugFileOptionsDialog.setArguments(args);
        debugFileOptionsDialog.setFileViewer(getSelfForButtonListeners());
        ((MainActivity)getActivity()).showDialogFragment(debugFileOptionsDialog);
    }
    final protected void setButtonListeners() {
        moveCopyButton.setOnClickListener(moveCopyButtonOnClickListener);
        cancelMoveCopyButton.setOnClickListener(cancelMoveCopyButtonOnClickListener);
    }
    private FileViewer getSelfForButtonListeners() {
        return this;
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
    protected void onMoveOrCopy(File file) {
        moveCopyFile=file;
    }
    protected void moveCopyReset() {
        moveState = NONE;
        moveCopyFile = null;
    }

}
