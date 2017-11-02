package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.support.annotation.Nullable;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;

public class MainActivity extends AppCompatActivity {

    private FloatingActionButton fab;
    private static MainActivityFragment mainActivityFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        if (savedInstanceState == null) {
            mainActivityFragment = new MainActivityFragment();
            displayFragment(mainActivityFragment, false);
        }

        fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mainActivityFragment.actionButtonPressed();
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
            //Tell mainActivityFragment to that onCreate is done and fragment is attached to activity.
            mainActivityFragment.onMainActivityPostCreate();
    }

    //Called by MainActivityFragment to change the icon when switching between encryption and decryption.
    public void setFABIcon(int drawableId) {
        fab.setImageDrawable(ResourcesCompat.getDrawable(getResources(), drawableId, null));
    }

    /*shows the dialog to help find internal storage in SAF. maybe called by MainActivity fragment or by the Help fragment*/
    public void showMissingFilesHelpDialog() {
        View missingFilesHelpLayout = getLayoutInflater().inflate(R.layout.dialogfragment_missing_files, null);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setView(missingFilesHelpLayout);
        builder.show();
    }

    /*
    * Display the about fragment
     */
    public void displayAboutFragment() {
        setFabVisible(false);
        displayFragment(new AboutFragment(), true);
    }

    public void setFabVisible(boolean visible) {
        fab.setVisibility(visible?View.VISIBLE:View.GONE);
    }

    /*
    * display the given fragment, called by returnToMainFragment and displayAboutFragment
     */
    private void displayFragment(Fragment fragment, boolean addToBackStack) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        if (addToBackStack) {
            fragmentTransaction.addToBackStack(null);
        }
        fragmentTransaction.replace(R.id.main_fragment_container, fragment);
        fragmentTransaction.commit();
    }

}
