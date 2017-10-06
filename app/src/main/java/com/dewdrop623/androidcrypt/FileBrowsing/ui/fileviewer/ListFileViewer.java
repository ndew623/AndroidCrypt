package com.dewdrop623.androidcrypt.FileBrowsing.ui.fileviewer;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.content.res.ResourcesCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.dewdrop623.androidcrypt.R;

import java.io.File;

/**
 * ListFileViewer is concrete implementation of FileViewer
 * ListFileViewer:
 * -inflates the FileViewer's layout
 * -gets an AbsListView instance from that view
 * -creates an instance of FileViewer.FileListAdapterGetViewCallback where it defines the getView behavior for the adapter for the AbsListView
 * -calls the FileViewer method that finishes the set up and passes to it: the view and getView callback
 */

public class ListFileViewer extends FileViewer {
    /*inflates a layout, gets its listView
 * passes the listView and a callback for the adapter to the parent class
  */
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_file_viewer, container, false);
        fileListView = (ListView) view.findViewById(R.id.fileListView);
        initializeFileViewerWithViewAndFileListAdapterGetViewCallback(view, fileListAdapterGetViewCallback);
        return view;
    }

    //implementation of FileViewer.FileListAdapterGetViewCallback for IconFileViewer
    private FileListAdapterGetViewCallback fileListAdapterGetViewCallback = new FileListAdapterGetViewCallback() {
        @Override
        public View getView(int position, View convertView, ViewGroup parent, FileViewer.FileListAdapter fileListAdapter) {
            if (convertView == null) {
                convertView = LayoutInflater.from(getContext()).inflate(R.layout.listitem_listfileviewer_item, parent, false);
            }
            File file = fileListAdapter.getItem(position);
            ImageView listItemImageView = (ImageView) convertView.findViewById(R.id.listItemImageView);
            TextView listItemTextView = (TextView) convertView.findViewById(R.id.listItemTextView);
            if (file.isDirectory()) {
                listItemImageView.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.ic_folder, null));
            } else {
                listItemImageView.setImageDrawable(ResourcesCompat.getDrawable(getResources(), R.drawable.ic_file, null));
            }
            listItemTextView.setText(file.getName());
            return convertView;
        }
    };
}
