package com.dewdrop623.androidaescrypt.FileBrowsing;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

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

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_debug_file_viewer, container, false);
        fileListView = (ListView) view.findViewById(R.id.fileListView);
        fileArrayAdapter = new FileArrayAdapter(getContext(),0);
        fileListView.setAdapter(fileArrayAdapter);
        fileListView.setOnItemClickListener(onItemClickListener);

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
            }
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

}
