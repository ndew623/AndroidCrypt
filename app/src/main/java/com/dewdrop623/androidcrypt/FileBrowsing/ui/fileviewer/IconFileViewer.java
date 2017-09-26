package com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.content.res.ResourcesCompat;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.dewdrop623.androidcrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidcrypt.FileBrowsing.ui.MainActivity;
import com.dewdrop623.androidcrypt.R;

import java.io.File;
import java.util.ArrayList;
import java.util.Collection;

/**
 * A subclass of FileViewer that displays filesystem with icons in a grid
 */

public class IconFileViewer extends FileViewer {
    private GridView fileGridView;
    private FileGridAdapter fileGridAdapter;


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
            case R.id.createFolderButton:
                createFolder();
                return true;
        }
        return false;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_icon_file_viewer, container, false);
        fileGridView = (GridView) view.findViewById(R.id.fileGridView);
        fileGridAdapter = new FileGridAdapter(getContext());
        fileGridView.setAdapter(fileGridAdapter);
        fileGridView.setOnItemClickListener(onItemClickListener);
        fileGridView.setOnItemLongClickListener(onItemLongClickListener);

        moveCopyButton = (ImageButton) view.findViewById(R.id.moveCopyButton);
        cancelButton = (ImageButton) view.findViewById(R.id.cancelMoveCopyButton);
        selectDirectoryButton = (ImageButton) view.findViewById(R.id.selectDirectoryButton);


        updateFileArrayAdapterFileList();
        setButtonListeners();

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
        }

        return view;
    }

    @Override
    public void moveFile(File file) {
        super.moveFile(file);
        moveCopyButton.setImageDrawable(ResourcesCompat.getDrawable(getResources(),R.drawable.ic_move,null));
    }

    @Override
    public void copyFile(File file) {
        super.copyFile(file);
        moveCopyButton.setImageDrawable(ResourcesCompat.getDrawable(getResources(),R.drawable.ic_copy,null));
    }



    @Override
    protected void onMoveOrCopy(File file) {
        super.onMoveOrCopy(file);
        moveCopyButton.setVisibility(View.VISIBLE);
        cancelButton.setVisibility(View.VISIBLE);
    }

    @Override
    protected void onSelectEncryptDecryptOutputDirectory() {
        super.onSelectEncryptDecryptOutputDirectory();
        selectDirectoryButton.setVisibility(View.VISIBLE);
        cancelButton.setVisibility(View.VISIBLE);
    }

    private void updateFileArrayAdapterFileList() {
        if (fileGridAdapter == null) { //file viewer has not been displayed yet
            return;
        }
        fileGridAdapter.clear();
        if (!fileBrowser.getCurrentPath().equals(fileBrowser.root)) {
            fileGridAdapter.add(FileBrowser.parentDirectory);
        }
        sortFileList();
        fileGridAdapter.addAll(fileList);
        fileGridAdapter.notifyDataSetChanged();
    }

    protected void resetDirectorySelectOperations() {
        super.resetDirectorySelectOperations();
        moveCopyButton.setVisibility(View.GONE);
        cancelButton.setVisibility(View.GONE);
        selectDirectoryButton.setVisibility(View.GONE);
    }

    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileGridAdapter.getItem(position);
            if (clickedFile.isDirectory()) {
                fileBrowser.setCurrentPath(clickedFile);
            } else {
                ((MainActivity)getActivity()).openOptionsDialog(clickedFile, getSelfForButtonListeners());
            }
        }
    };
    private AdapterView.OnItemLongClickListener onItemLongClickListener = new AdapterView.OnItemLongClickListener() {
        @Override
        public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileGridAdapter.getItem(position);
            if (clickedFile != FileBrowser.parentDirectory) {
                ((MainActivity)getActivity()).openOptionsDialog(clickedFile, getSelfForButtonListeners());
            }
            return true;
        }
    };

    private class FileGridAdapter extends BaseAdapter {
        private Context context;
        private ArrayList<File> files = new ArrayList();

        public FileGridAdapter(Context context) {
            this.context = context;
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
            if (convertView == null) {
                convertView = LayoutInflater.from(context).inflate(R.layout.view_icon, parent, false);
            }
            File file = getItem(position);
            ImageView fileIconImageView = (ImageView) convertView.findViewById(R.id.fileIconImageView);
            TextView fileNameTextView = (TextView) convertView.findViewById(R.id.fileNameTextView);
            if (file.isDirectory()) {
                fileIconImageView.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.ic_folder, null));
            } else {
                fileIconImageView.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.ic_file, null));
            }
            fileNameTextView.setText(file.getName());
            return convertView;
        }
    }
}
