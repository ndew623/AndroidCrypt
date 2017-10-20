package com.dewdrop623.androidcrypt;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;

public class MainActivity extends AppCompatActivity {

    private FloatingActionButton fab;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivityFragment mainActivityFragment = getMainActivityFragment();
                mainActivityFragment.actionButtonPressed();
            }
        });
    }

    public void setFABIcon(int drawableId) {
        fab.setImageDrawable(ResourcesCompat.getDrawable(getResources(), drawableId, null));
    }

    private MainActivityFragment getMainActivityFragment() {
        return (MainActivityFragment) getSupportFragmentManager().findFragmentById(R.id.main_fragment);
    }

}
