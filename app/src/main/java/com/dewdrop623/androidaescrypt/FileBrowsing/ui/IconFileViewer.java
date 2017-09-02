package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

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
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.FileViewer;
import com.dewdrop623.androidaescrypt.R;

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
        cancelMoveCopyButton = (ImageButton) view.findViewById(R.id.cancelMoveCopyButton);


        updateFileArrayAdapterFileList();
        setButtonListeners();

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
        cancelMoveCopyButton.setVisibility(View.VISIBLE);
    }

    private void updateFileArrayAdapterFileList() {
        if (fileGridAdapter == null) { //file viewer has not been displayed yet
            return;
        }
        fileGridAdapter.clear();
        if (!fileBrowser.getCurrentPath().equals(new File("/")) /* || ! fileBrowser.getCurrentPath().getAbsolutePath().equals(FileBrowser.topLevelInternal.getAbsolutePath())*/) { //uncomment to prevent navigating from /sdcard to root
            fileGridAdapter.add(FileBrowser.parentDirectory);
        }
        fileGridAdapter.addAll(fileList);
        fileGridAdapter.notifyDataSetChanged();
    }

    protected void moveCopyReset() {
        super.moveCopyReset();
        moveCopyButton.setVisibility(View.GONE);
        cancelMoveCopyButton.setVisibility(View.GONE);
    }

    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            File clickedFile = fileGridAdapter.getItem(position);
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
            File clickedFile = fileGridAdapter.getItem(position);
            if (clickedFile != FileBrowser.parentDirectory) {
                openOptionsDialog(clickedFile);
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
