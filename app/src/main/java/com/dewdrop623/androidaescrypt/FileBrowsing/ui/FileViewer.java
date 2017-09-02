package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

import android.os.Bundle;
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
    protected enum MoveState {
        MOVE, COPY, NONE
    }
    private File moveCopyFile;
    protected MoveState moveState = MoveState.NONE;

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
            if(moveState == MoveState.NONE) {
                return;
            }
            Bundle args = new Bundle();
            int fileOperationType = -1;
            if(moveState == MoveState.MOVE) {
                args.putString(FileMoveOperator.FILE_MOVE_DESTINATION_ARG, fileBrowser.getCurrentPath().getAbsolutePath());
                fileOperationType = FileOperationType.MOVE;
            } else if (moveState == MoveState.COPY) {
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
        moveState = MoveState.MOVE;
        onMoveOrCopy(file);
    }
    public void copyFile(File file) {
        moveState = MoveState.COPY;
        onMoveOrCopy(file);
    }
    protected void onMoveOrCopy(File file) {
        moveCopyFile=file;
    }
    protected void moveCopyReset() {
        moveState = MoveState.NONE;
        moveCopyFile = null;
    }
}
