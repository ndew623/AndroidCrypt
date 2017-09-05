package com.dewdrop623.androidaescrypt;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.LayoutRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v4.util.ArraySet;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.dewdrop623.androidaescrypt.FileBrowsing.FileBrowser;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.FileViewer;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.IconFileViewer;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.DebugFileOptionsDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.FileDialog;

import java.io.File;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Set;

public class MainActivity extends AppCompatActivity {
    private static final String DIALOG_FRAGMENT_TAG = "file_dialog";
    private static final String SHARED_PREFERENCES_FAVORITE_FILE = "favorite";
    private static final String SHARED_PREFERENCES_FAVORITE_STRING_SET = "com.dewdrop623.androidaescrypt.MainActivity.SHARED_PREFERENCES_FAVORITE_STRING_SET";
    private static final int WRITE_FILE_PERMISSION_REQUEST_CODE = 654;

    /*TODO more stuff from favorites drawer to clean*/
    private ActionBarDrawerToggle drawerToggle;
    private FavoritesDrawerArrayAdapter arrayAdapter;
    /*-----------------------------------------------*/
    private FileViewer fileViewer;

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //TODO put sdcard directory somewhere in ui
        ///first item in array is /storage/emulated (no 0), other items are sdcard/usb/stuff
        //for (File file : getExternalFilesDirs(null)) {
        //    Log.e("EFD", FileUtils.getMountPointContainingFile(file).getAbsolutePath());
        //}

        setContentView(R.layout.activity_main);
        checkPermissions();

        /*TODO gonna need to probably refactor the shit out of this mess*/
        DrawerLayout drawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        ListView drawerList = (ListView) findViewById(R.id.left_drawer);
        arrayAdapter = new FavoritesDrawerArrayAdapter(this, R.layout.favorites_list_item);
        refreshFavoritesDrawerData();
        drawerList.setAdapter(arrayAdapter);

        drawerList.setOnItemClickListener(new ListView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                //TODO stuff happens
                File file = new File(arrayAdapter.getItem(i));
                if (file.isDirectory()) {
                    fileViewer.changePath(file);
                } else {
                    openOptionsDialog(file, fileViewer);
                }
            }
        });
        drawerList.setOnItemLongClickListener(new ListView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> adapterView, View view, int i, long l) {
                File file = new File(arrayAdapter.getItem(i));
                openOptionsDialog(file, fileViewer);
                return true;
            }
        });

        drawerToggle = new ActionBarDrawerToggle(this, drawerLayout, R.string.favorites, R.string.close_favorites) {
            @Override
            public void onDrawerOpened(View drawerView) {
                super.onDrawerOpened(drawerView);
                getSupportActionBar().setTitle(R.string.favorites);
                //invalidateOptionsMenu();
            }

            @Override
            public void onDrawerClosed(View drawerView) {
                super.onDrawerClosed(drawerView);
                getSupportActionBar().setTitle(R.string.app_name);
                //invalidateOptionsMenu();
            }
        };
        drawerLayout.setDrawerListener(drawerToggle);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setHomeButtonEnabled(true);
        /*end of shit*/


        FragmentManager fragmentManager = getSupportFragmentManager();
        if (savedInstanceState == null) {
            fileViewer = new IconFileViewer();
            FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
            fragmentTransaction.add(R.id.mainActivityFrameLayout, fileViewer);
            fragmentTransaction.commit();
        } else {
            fileViewer = (FileViewer) fragmentManager.findFragmentById(R.id.mainActivityFrameLayout);
        }
    }

    /*TODO cleanup*/

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        drawerToggle.syncState();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        drawerToggle.onConfigurationChanged(newConfig);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (drawerToggle.onOptionsItemSelected(item)) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
    private void refreshFavoritesDrawerData() {
        String[] favorites = getFavoritesSet(getSharedPreferencesFavoritesFile()).toArray(new String[]{});
        arrayAdapter.clear();
        arrayAdapter.addAll(favorites);
        arrayAdapter.notifyDataSetChanged();
    }
    /*------------------*/

    public void checkPermissions() {
        if(ContextCompat.checkSelfPermission(this, android.Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED ) {
            ActivityCompat.requestPermissions(this, new String[] {android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_FILE_PERMISSION_REQUEST_CODE);
            recreate();
        }
    }
    public void showDialogFragment(DialogFragment dialogFragment) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction ft = fragmentManager.beginTransaction();
        dialogFragment.show(ft, DIALOG_FRAGMENT_TAG +dialogFragment.getId());
    }
    public void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }
    public void addFavorite(String path) {
        SharedPreferences sharedPreferences = getSharedPreferencesFavoritesFile();
        Set<String> favorites = getFavoritesSet(sharedPreferences);
        favorites.add(path);
        setFavoritesStringSet(favorites, sharedPreferences);
    }
    public void removeFavorite(String path) {
        SharedPreferences sharedPreferences = getSharedPreferencesFavoritesFile();
        Set<String> favorites = getFavoritesSet(sharedPreferences);
        favorites.remove(path);
        setFavoritesStringSet(favorites, sharedPreferences);
    }
    public Set<String> getFavoritesSet(SharedPreferences sharedPreferences) {
        Set<String> oldFavorites = sharedPreferences.getStringSet(SHARED_PREFERENCES_FAVORITE_STRING_SET, new ArraySet<String>());
        ArraySet<String> favorites = new ArraySet<>();
        favorites.addAll(oldFavorites);
        return favorites;
    }
    public boolean isInFavorites(File file) {
        Set<String> favorites = getFavoritesSet(getSharedPreferencesFavoritesFile());
        return favorites.contains(file.getAbsolutePath());
    }
    public void openOptionsDialog(File clickedFile, FileViewer fileViewer) {
        DebugFileOptionsDialog debugFileOptionsDialog = new DebugFileOptionsDialog();
        Bundle args = new Bundle();
        args.putString(FileDialog.PATH_ARGUMENT, clickedFile.getAbsolutePath());
        debugFileOptionsDialog.setArguments(args);
        debugFileOptionsDialog.setFileViewer(fileViewer);
        showDialogFragment(debugFileOptionsDialog);
    }
    private SharedPreferences getSharedPreferencesFavoritesFile() {
        return getSharedPreferences(SHARED_PREFERENCES_FAVORITE_FILE, 0);
    }
    private void setFavoritesStringSet(Set<String> favorites, SharedPreferences sharedPreferences) {
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putStringSet(SHARED_PREFERENCES_FAVORITE_STRING_SET, favorites);
        editor.commit();
        refreshFavoritesDrawerData();
    }
    private class FavoritesDrawerArrayAdapter extends ArrayAdapter<String> {
        private Context context;
        private int listItemResource;
        private ArrayList<String> files = new ArrayList<>();

        public FavoritesDrawerArrayAdapter(Context context, int resource) {
            super(context, resource);
            this.context = context;
            listItemResource = resource;
        }

        @Override
        public void add(@Nullable String object) {
            files.add(object);
        }

        @Override
        public void addAll(@NonNull Collection<? extends String> collection) {
            files.addAll(collection);
        }

        @Override
        public void addAll(String... items) {
            for (String s : items) {
                files.add(s);
            }
        }

        @Override
        public void remove(@Nullable String object) {
            files.remove(object);
        }

        public void remove(int i) {
            files.remove(i);
        }

        @Override
        public void clear() {
            files.clear();
        }

        @Override
        public int getCount() {
            return files.size();
        }

        @Nullable
        @Override
        public String getItem(int position) {
            return files.get(position);
        }

        @NonNull
        @Override
        public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
            if (convertView == null) {
                convertView = ((LayoutInflater)context.getSystemService(LAYOUT_INFLATER_SERVICE)).inflate(listItemResource, parent, false);
            }

            File file = new File(getItem(position));
            ImageView favoritesListItemImageView = (ImageView) convertView.findViewById(R.id.favoritesListItemImageView);
            TextView favoritesListItemTextView = (TextView) convertView.findViewById(R.id.favoritesListItemTextView);
            Drawable fileDrawable = ResourcesCompat.getDrawable(getResources(), R.drawable.ic_file, null);
            Drawable folderDrawable = ResourcesCompat.getDrawable(getResources(), R.drawable.ic_folder, null);


            if (!file.exists()) {
                favoritesListItemTextView.setText(R.string.file_does_not_exist);
            } else {
                favoritesListItemTextView.setText(file.getName());
            }

            if (file.isDirectory()) {
                favoritesListItemImageView.setImageDrawable(folderDrawable);
            } else {
                favoritesListItemImageView.setImageDrawable(fileDrawable);
            }

            return convertView;
        }
    }

}
