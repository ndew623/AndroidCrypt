package com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog;

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
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.GridView;
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
        }
        return false;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_debug_file_viewer, container, false);
        fileGridView = (GridView) view.findViewById(R.id.fileGridView);
        fileGridAdapter = new FileGridAdapter(getContext());
        fileGridView.setAdapter(fileGridAdapter);
        fileGridView.setOnItemClickListener(onItemClickListener);
        fileGridView.setOnItemLongClickListener(onItemLongClickListener);

        createFolderButton = (Button) view.findViewById(R.id.createFolderButton);
        moveCopyButton = (Button) view.findViewById(R.id.moveCopyButton);
        cancelMoveCopyButton = (Button) view.findViewById(R.id.cancelMoveCopyButton);


        updateFileArrayAdapterFileList();
        setButtonListeners();

        return view;
    }

    @Override
    public void moveFile(File file) {
        super.moveFile(file);
        moveCopyButton.setText(getString(R.string.move_here));
    }

    @Override
    public void copyFile(File file) {
        super.copyFile(file);
        moveCopyButton.setText(getString(R.string.copy_here));
    }

    @Override
    protected void onMoveOrCopy(File file) {
        super.onMoveOrCopy(file);

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
    protected void moveCopyReset() {
        super.moveCopyReset();
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
                File file = getItem(position);
                convertView = LayoutInflater.from(context).inflate(R.layout.view_icon, parent, false);
                ImageView fileIconImageView = (ImageView) convertView.findViewById(R.id.fileIconImageView);
                TextView fileNameTextView = (TextView) convertView.findViewById(R.id.fileNameTextView);
                //TODO set image view based on .isDirectory
                fileNameTextView.setText(file.getName());
            }
            return convertView;
        }
    }
}
