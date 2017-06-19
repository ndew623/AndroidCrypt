package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

import android.support.v4.app.DialogFragment;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
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
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.FileOptionsDialog;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * a simple fragment for displaying files and test functionality
 */

public class DebugFileViewer extends FileViewer {
    private static final String FRAGMENT_TAG = "debug_file_viewer_dialog";

    private ListView fileListView;
    private FileArrayAdapter fileArrayAdapter;
    private Button createFolderButton;

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

        createFolderButton = (Button) view.findViewById(R.id.createFolderButton);
        createFolderButton.setOnClickListener(createFolderButtonOnClickListener);

        updateFileArrayAdapterFileList();

        return view;
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
                DebugFileOptionsDialog debugFileOptionsDialog = new DebugFileOptionsDialog();
                Bundle args = new Bundle();
                args.putString(FileOptionsDialog.FILE_PATH_ARGUMENT, clickedFile.getAbsolutePath());
                debugFileOptionsDialog.setArguments(args);
                debugFileOptionsDialog.setFileBrowser(fileBrowser);
                showDialogFragment(debugFileOptionsDialog);
            }
        }
    };
    private View.OnClickListener createFolderButtonOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            DebugCreateDirectoryDialog debugCreateDirectoryDialog = new DebugCreateDirectoryDialog();
            Bundle args = new Bundle();
            args.putString(DebugCreateDirectoryDialog.NEW_DIRECTORY_PATH_ARGUMENT, fileBrowser.getCurrentPath().getAbsolutePath());
            debugCreateDirectoryDialog.setArguments(args);
            debugCreateDirectoryDialog.setFileBrowser(fileBrowser);
            showDialogFragment(debugCreateDirectoryDialog);
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
            ((TextView)convertView).setText(getItem(position).getPath());
            return super.getView(position, convertView, parent);
        }
    }
    private void showDialogFragment(DialogFragment dialogFragment) {
        FragmentManager fragmentManager = getFragmentManager();
        FragmentTransaction ft = fragmentManager.beginTransaction();
        Fragment prev = fragmentManager.findFragmentByTag(FRAGMENT_TAG);
        if (prev != null) {
            ft.remove(prev);
        }
        ft.addToBackStack(null);
        dialogFragment.show(ft, FRAGMENT_TAG);
    }
}
