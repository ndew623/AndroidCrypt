package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.DebugCreateDirectoryDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.DebugFileOptionsDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.FileDialog;
import com.dewdrop623.androidaescrypt.FileOperations.FileCommand;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidaescrypt.MainActivity;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;
import java.util.HashMap;

/**
 * a simple fragment for displaying files and test functionality
 */

public class DebugFileViewer extends FileViewer {

    private enum MoveState {
        MOVE, COPY, NONE
    }
    private File moveCopyFile;
    private MoveState moveState = MoveState.NONE;

    private ListView fileListView;
    private FileArrayAdapter fileArrayAdapter;
    private Button createFolderButton;
    private Button moveCopyButton;

    @Override
    public void setFileList(File[] fileList) {
        super.setFileList(fileList);
        updateFileArrayAdapterFileList();
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_debug_file_viewer, container, false);
        fileListView = (ListView) view.findViewById(R.id.fileListView);
        fileArrayAdapter = new FileArrayAdapter(getContext(),0);
        fileListView.setAdapter(fileArrayAdapter);
        fileListView.setOnItemClickListener(onItemClickListener);
        fileListView.setOnItemLongClickListener(onItemLongClickListener);

        createFolderButton = (Button) view.findViewById(R.id.createFolderButton);
        createFolderButton.setOnClickListener(createFolderButtonOnClickListener);

        moveCopyButton = (Button) view.findViewById(R.id.moveCopyButton);
        moveCopyButton.setOnClickListener(moveCopyButtonOnClickListener);

        updateFileArrayAdapterFileList();

        return view;
    }

    @Override
    public void moveFile(File file) {
        super.moveFile(file);
        moveCopyButton.setText(getString(R.string.move_here));
        moveState = MoveState.MOVE;
    }

    @Override
    public void copyFile(File file) {
        super.copyFile(file);
        moveCopyButton.setText(getString(R.string.copy_here));
        moveState = MoveState.COPY;
    }

    @Override
    protected void onMoveOrCopy(File file) {
        super.onMoveOrCopy(file);
        moveCopyFile=file;
        moveCopyButton.setVisibility(View.VISIBLE);
    }

    private void updateFileArrayAdapterFileList() {
        if (fileArrayAdapter == null) { //file viewer has not been displayed yet
            return;
        }
        fileArrayAdapter.clear();
        if (! fileBrowser.getCurrentPath().getAbsolutePath().equals(FileBrowser.topLevelInternal.getAbsolutePath())) {
            fileArrayAdapter.add(FileBrowser.parentDirectory);
        }
        fileArrayAdapter.addAll(fileList);
        fileArrayAdapter.notifyDataSetChanged();
    }
    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileArrayAdapter.getItem(position);
            if (clickedFile.isDirectory()) {
                fileBrowser.changePath(clickedFile);
            } else {
                openOptionsDialog(clickedFile);
            }
        }
    };
    private AdapterView.OnItemLongClickListener onItemLongClickListener = new AdapterView.OnItemLongClickListener() {
        @Override
        public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
            openOptionsDialog(fileArrayAdapter.getItem(position));
            return true;
        }
    };
    private View.OnClickListener createFolderButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            DebugCreateDirectoryDialog debugCreateDirectoryDialog = new DebugCreateDirectoryDialog();
            Bundle args = new Bundle();
            args.putString(FileDialog.PATH_ARGUMENT, fileBrowser.getCurrentPath().getAbsolutePath());
            debugCreateDirectoryDialog.setArguments(args);
            debugCreateDirectoryDialog.setFileViewer(getSelfForButtonListeners());
            ((MainActivity)getActivity()).showDialogFragment(debugCreateDirectoryDialog);
        }
    };
    private View.OnClickListener moveCopyButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if(moveState == MoveState.NONE) {
                return;
            }
            HashMap<String,String> args = new HashMap<>();
            FileCommand fileCommand = null;
            if(moveState == MoveState.MOVE) {
                args.put(FileMoveOperator.FILE_MOVE_DESTINATION_ARG, fileBrowser.getCurrentPath().getAbsolutePath());
                fileCommand = new FileCommand(moveCopyFile, FileOperationType.MOVE, args);
            } else if (moveState == MoveState.COPY) {
                args.put(FileCopyOperator.FILE_COPY_DESTINATION_ARG, fileBrowser.getCurrentPath().getAbsolutePath());
                fileCommand = new FileCommand(moveCopyFile, FileOperationType.COPY, args);
            }
            sendFileCommandToFileBrowser(fileCommand);
            moveCopyButton.setVisibility(View.GONE);
            moveState = MoveState.NONE;
            moveCopyFile = null;
        }
    };
    private class FileArrayAdapter extends ArrayAdapter<File> {
        public FileArrayAdapter(Context context, int resource) {
            super(context, resource);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = new TextView(getContext());
            }
            File file = getItem(position);
            String displayString = file.getAbsolutePath();
            if(file.isDirectory()) {
                displayString=displayString.concat("/");
            }
            ((TextView)convertView).setText(displayString);
            return convertView;
        }
    }
    private FileViewer getSelfForButtonListeners() {
        return this;
    }

    private void openOptionsDialog(File clickedFile) {
        DebugFileOptionsDialog debugFileOptionsDialog = new DebugFileOptionsDialog();
        Bundle args = new Bundle();
        args.putString(FileDialog.PATH_ARGUMENT, clickedFile.getAbsolutePath());
        debugFileOptionsDialog.setArguments(args);
        debugFileOptionsDialog.setFileViewer(getSelfForButtonListeners());
        ((MainActivity)getActivity()).showDialogFragment(debugFileOptionsDialog);
    }
}
