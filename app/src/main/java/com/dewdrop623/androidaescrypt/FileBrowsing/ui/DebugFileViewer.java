package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.DebugCreateDirectoryDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.DebugFileOptionsDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.FileDialog;
import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileOperationType;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidaescrypt.MainActivity;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

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
    private Button cancelMoveCopyButton;

    @Override
    public void setFileList(File[] fileList) {
        super.setFileList(fileList);
        updateFileArrayAdapterFileList();
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.file_viewer_menu, menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.home_button:
                goToHomeDirectory();
                return true;
        }
        return false;
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

        cancelMoveCopyButton = (Button) view.findViewById(R.id.cancelMoveCopyButton);
        cancelMoveCopyButton.setOnClickListener(cancelMoveCopyButtonOnClickListener);

        updateFileArrayAdapterFileList();

        return view;
    }

    @Override
    public void moveFile(File file) {
        moveCopyButton.setText(getString(R.string.move_here));
        moveState = MoveState.MOVE;
        super.moveFile(file);
    }

    @Override
    public void copyFile(File file) {
        moveCopyButton.setText(getString(R.string.copy_here));
        moveState = MoveState.COPY;
        super.copyFile(file);
    }

    @Override
    protected void onMoveOrCopy(File file) {
        super.onMoveOrCopy(file);
        moveCopyFile=file;
        moveCopyButton.setVisibility(View.VISIBLE);
        cancelMoveCopyButton.setVisibility(View.VISIBLE);
        String cancelMCBText = "";
        if (moveState == MoveState.COPY) {
            cancelMCBText = getString(R.string.cancel)+" "+getString(R.string.copy);
        } else if (moveState == MoveState.MOVE) {
            cancelMCBText = getString(R.string.cancel)+" "+getString(R.string.move);
        }
        cancelMoveCopyButton.setText(cancelMCBText);
    }

    private void updateFileArrayAdapterFileList() {
        if (fileArrayAdapter == null) { //file viewer has not been displayed yet
            return;
        }
        fileArrayAdapter.clear();
        if (!fileBrowser.getCurrentPath().equals(new File("/")) /* || ! fileBrowser.getCurrentPath().getAbsolutePath().equals(FileBrowser.topLevelInternal.getAbsolutePath())*/) { //uncomment to prevent navigating from /sdcard to root
            fileArrayAdapter.add(FileBrowser.parentDirectory);
        }
        fileArrayAdapter.addAll(fileList);
        fileArrayAdapter.notifyDataSetChanged();
    }
    private void moveCopyReset() {
        moveState = MoveState.NONE;
        moveCopyFile = null;
        moveCopyButton.setVisibility(View.GONE);
        cancelMoveCopyButton.setVisibility(View.GONE);
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
            File clickedFile = fileArrayAdapter.getItem(position);
            if (clickedFile!=FileBrowser.parentDirectory) {
                openOptionsDialog(clickedFile);
            }
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
