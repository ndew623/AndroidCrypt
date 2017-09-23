package com.dewdrop623.androidaescrypt.FileBrowsing.ui.fileviewer;

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
import android.widget.ListView;
import android.widget.TextView;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;

/**
 * a simple fragment for displaying files and test functionality
 */

public class DebugFileViewer extends FileViewer {



    private ListView fileListView;
    private FileArrayAdapter fileArrayAdapter;


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

        //createFolderButton = (Button) view.findViewById(R.id.createFolderButton);
      //  moveCopyButton = (Button) view.findViewById(R.id.moveCopyButton);
      //  cancelButton = (Button) view.findViewById(R.id.cancelButton);


        updateFileArrayAdapterFileList();
        setButtonListeners();

        return view;
    }

    @Override
    public void moveFile(File file) {
        super.moveFile(file);
       // moveCopyButton.setText(getString(R.string.move_here));
    }

    @Override
    public void copyFile(File file) {
        super.copyFile(file);
       // moveCopyButton.setText(getString(R.string.copy_here));
    }

    @Override
    protected void onMoveOrCopy(File file) {
        super.onMoveOrCopy(file);

        moveCopyButton.setVisibility(View.VISIBLE);
        cancelButton.setVisibility(View.VISIBLE);
        String cancelMCBText = "";
        if (moveState == COPY) {
            cancelMCBText = getString(R.string.cancel)+" "+getString(R.string.copy);
        } else if (moveState == MOVE) {
            cancelMCBText = getString(R.string.cancel)+" "+getString(R.string.move);
        }
       // cancelButton.setText(cancelMCBText);
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
    protected void resetDirectorySelectOperations() {
        super.resetDirectorySelectOperations();
        moveCopyButton.setVisibility(View.GONE);
        cancelButton.setVisibility(View.GONE);
    }
    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileArrayAdapter.getItem(position);
            if (clickedFile.isDirectory()) {
                fileBrowser.changePath(clickedFile);
            } else {
        //        openOptionsDialog(clickedFile);
            }
        }
    };
    private AdapterView.OnItemLongClickListener onItemLongClickListener = new AdapterView.OnItemLongClickListener() {
        @Override
        public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileArrayAdapter.getItem(position);
            if (clickedFile!=FileBrowser.parentDirectory) {
           //     openOptionsDialog(clickedFile);
            }
            return true;
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



}
