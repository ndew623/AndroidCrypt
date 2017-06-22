package com.dewdrop623.androidaescrypt;

import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Toast;

import com.dewdrop623.androidaescrypt.FileBrowsing.ui.DebugFileViewer;
import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.FileViewer;

public class MainActivity extends AppCompatActivity {
    private static final String FRAGMENT_TAG = "file_dialog";
    private static final int WRITE_FILE_PERMISSION_REQUEST_CODE = 654;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        checkPermissions();
        FileViewer fileViewer = new DebugFileViewer();
        FileBrowser fileBrowser = new FileBrowser();
        fileBrowser.setFileViewer(fileViewer);
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.add(R.id.mainActivityFrameLayout, fileViewer);
        fragmentTransaction.commit();
    }
    public void checkPermissions() {
        if(ContextCompat.checkSelfPermission(this, android.Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED ) {
            ActivityCompat.requestPermissions(this, new String[] {android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_FILE_PERMISSION_REQUEST_CODE);
        }
    }
    public void showDialogFragment(DialogFragment dialogFragment) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction ft = fragmentManager.beginTransaction();
        dialogFragment.show(ft, FRAGMENT_TAG+dialogFragment.getId());
    }
    public void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }
}
