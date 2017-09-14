package com.dewdrop623.androidaescrypt.FileBrowsing.ui;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
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
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.DebugFileOptionsDialog;
import com.dewdrop623.androidaescrypt.FileBrowsing.ui.dialog.filedialog.FileDialog;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.R;

import java.io.File;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Set;

public class MainActivity extends AppCompatActivity {
    private static final String DIALOG_FRAGMENT_TAG = "file_dialog";
    private static final String SHARED_PREFERENCES_FAVORITE_FILE = "favorite";
    private static final String SHARED_PREFERENCES_FAVORITE_STRING_SET = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.MainActivity.SHARED_PREFERENCES_FAVORITE_STRING_SET";
    private static final int WRITE_FILE_PERMISSION_REQUEST_CODE = 654;

    //pass these into the FavoritesDrawerArrayAdapter to get titles in the list
    private static final String MOUNT_POINTS_TITLE_MAGIC_VALUE = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.MainActivity.MOUNT_POINTS_TITLE_MAGIC_VALUE.DKixODTLz\"ABSans('pCQ`udAaJFTcRl6`/*:l>X`;,oIQy75{#U+38o=qR%8)\"";
    private static final String FAVORITES_TITLE_MAGIC_VALUE = "com.dewdrop623.androidaescrypt.FileBrowsing.ui.MainActivity.FAVORITES_TITLE_MAGIC_VALUE.T6g#eG?ZvfeS]F:cFhe5[:d{zg@o$N|f)i)SV`.5@)_$z=--b8epNmaw!DWg$e;";

    private ActionBarDrawerToggle drawerToggle;
    private FavoritesDrawerArrayAdapter arrayAdapter;

    private FileViewer fileViewer;

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        checkPermissions();

        //set up the drawer layout and a ListView
        DrawerLayout drawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        ListView drawerList = (ListView) findViewById(R.id.left_drawer);
        //create an adapter, attach it to the ListView and give it data
        arrayAdapter = new FavoritesDrawerArrayAdapter(this, R.layout.favorites_list_item);
        drawerList.setAdapter(arrayAdapter);
        refreshFavoritesDrawerData();
        //click listeners
        drawerList.setOnItemClickListener(favoritesDrawerOnItemClickListener);
        drawerList.setOnItemLongClickListener(favoritesDrawerOnItemLongClickListener);
        //an object that handles the drawer opening and closing
        drawerToggle = new ActionBarDrawerToggle(this, drawerLayout, R.string.favorites, R.string.close_favorites) {
            @Override
            public void onDrawerOpened(View drawerView) {
                super.onDrawerOpened(drawerView);
                getSupportActionBar().setTitle(R.string.favorites);
                invalidateOptionsMenu();
                arrayAdapter.notifyDataSetChanged();
            }

            @Override
            public void onDrawerClosed(View drawerView) {
                super.onDrawerClosed(drawerView);
                getSupportActionBar().setTitle(R.string.app_name);
                invalidateOptionsMenu();
            }
        };
        drawerLayout.setDrawerListener(drawerToggle);

        //the action bar needs some methods called to get the drawer open button
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setHomeButtonEnabled(true);

        //create the fragment manager
        FragmentManager fragmentManager = getSupportFragmentManager();
        if (savedInstanceState == null) {
            //the activity is new and we need to instantiate the fragment that is the main gui and add it to the stack
            fileViewer = new IconFileViewer();
            FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
            fragmentTransaction.add(R.id.mainActivityFrameLayout, fileViewer);
            fragmentTransaction.commit();
        } else if (fileViewer == null) {
            //the fragment is not new. the screen probably just rotated. find the old fragment from the stack from the id in its layout and update our member variable reference to it
            refreshFileViewerReferenceWithLayoutID(fragmentManager);
        }
    }

    /*
    * ANONYMOUS CLASSES
    * */
    ListView.OnItemClickListener favoritesDrawerOnItemClickListener = new ListView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
            String item = arrayAdapter.getItem(i);
            if (!(item.equals(MOUNT_POINTS_TITLE_MAGIC_VALUE) || item.equals(FAVORITES_TITLE_MAGIC_VALUE))) {
                File file = new File(item);
                if (file.isDirectory()) {
                    fileViewer.changePath(file);
                } else {
                    openOptionsDialog(file, fileViewer);
                }
            }
        }
    };
    ListView.OnItemLongClickListener favoritesDrawerOnItemLongClickListener = new ListView.OnItemLongClickListener() {
        @Override
        public boolean onItemLongClick(AdapterView<?> adapterView, View view, int i, long l) {
            String item = arrayAdapter.getItem(i);
            if (!(item.equals(MOUNT_POINTS_TITLE_MAGIC_VALUE) || item.equals(FAVORITES_TITLE_MAGIC_VALUE))) {
                File file = new File(item);
                openOptionsDialog(file, fileViewer);
            }
            return true;
        }
    };

    /*
    * PUBLIC METHODS
    * */
    //documention about the drawer had a code example and this was in there. it probably does something useful
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        drawerToggle.onConfigurationChanged(newConfig);
    }

    //not currently being used. fragments handle options menus. but this is here if options menu options are ever added to the favorites drawer
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (drawerToggle.onOptionsItemSelected(item)) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    //check for the necessary permissions. destroy and recreate the activity if permissions are asked for so that the files (which couldn't be seen previously) will be displayed
    public void checkPermissions() {
        if(ContextCompat.checkSelfPermission(this, android.Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED ) {
            ActivityCompat.requestPermissions(this, new String[] {android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_FILE_PERMISSION_REQUEST_CODE);
            recreate();
        }
    }

    //return a reference to the current FileViewer fragment
    public FileViewer getFileViewer() {
        //check if fileView is null. if it is, try to get it with its layout id from fragment manager (usually used after a screen rotation)
        if (fileViewer == null) {
            refreshFileViewerReferenceWithLayoutID(getSupportFragmentManager());
        }
        return fileViewer;
    }

    //show the given dialog fragment
    public void showDialogFragment(DialogFragment dialogFragment) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction ft = fragmentManager.beginTransaction();
        dialogFragment.show(ft, DIALOG_FRAGMENT_TAG +dialogFragment.getId());
    }

    //show a toast displaying a given string
    public void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    //add a path to the favorites list
    public void addFavorite(String path) {
        SharedPreferences sharedPreferences = getSharedPreferencesFavoritesFile();
        Set<String> favorites = getFavoritesSet(sharedPreferences);
        favorites.add(path);
        setFavoritesStringSet(favorites, sharedPreferences);
    }

    //remove a path from the favorites list
    public void removeFavorite(String path) {
        SharedPreferences sharedPreferences = getSharedPreferencesFavoritesFile();
        Set<String> favorites = getFavoritesSet(sharedPreferences);
        favorites.remove(path);
        setFavoritesStringSet(favorites, sharedPreferences);
    }

    //get the set of paths that are favorites in Shared Preferences
    public Set<String> getFavoritesSet(SharedPreferences sharedPreferences) {
        Set<String> oldFavorites = sharedPreferences.getStringSet(SHARED_PREFERENCES_FAVORITE_STRING_SET, new ArraySet<String>());
        ArraySet<String> favorites = new ArraySet<>();
        favorites.addAll(oldFavorites);
        return favorites;
    }

    //check if a directory is a favorite
    public boolean isInFavorites(File file) {
        Set<String> favorites = getFavoritesSet(getSharedPreferencesFavoritesFile());
        return favorites.contains(file.getAbsolutePath());
    }

    //open the dialog for file options
    public void openOptionsDialog(File clickedFile, FileViewer fileViewer) {
        DebugFileOptionsDialog debugFileOptionsDialog = new DebugFileOptionsDialog();
        Bundle args = new Bundle();
        args.putString(FileDialog.PATH_ARGUMENT, clickedFile.getAbsolutePath());
        debugFileOptionsDialog.setArguments(args);
        debugFileOptionsDialog.setFileViewer(fileViewer);
        showDialogFragment(debugFileOptionsDialog);
    }

    /*
    * PROTECTED METHODS
    * */
    //documention about the favorites drawer had a code example and this was in there. it probably does something useful
    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        drawerToggle.syncState();
    }

    /*
     * PRIVATE METHODS
     * */


    //reset the fileViewer member variable (e.g. after a screen rotation)
    private void refreshFileViewerReferenceWithLayoutID(FragmentManager fragmentManager) {
        fileViewer = (FileViewer) fragmentManager.findFragmentById(R.id.mainActivityFrameLayout);
    }

    //refresh the favorites drawer to display the current favorites
    private void refreshFavoritesDrawerData() {
        ArrayList<String> mountpoints = FileUtils.getMountExternalStorageMountPoints(this);
        String[] favorites = getFavoritesSet(getSharedPreferencesFavoritesFile()).toArray(new String[]{});

        arrayAdapter.clear();
        if (!mountpoints.isEmpty()) {
            arrayAdapter.add(MOUNT_POINTS_TITLE_MAGIC_VALUE);
            arrayAdapter.addAll(mountpoints);
            arrayAdapter.add(FAVORITES_TITLE_MAGIC_VALUE);
        }
        arrayAdapter.addAll(favorites);
        arrayAdapter.notifyDataSetChanged();
    }

    //get the sharedpreferences file with the favorites in it
    private SharedPreferences getSharedPreferencesFavoritesFile() {
        return getSharedPreferences(SHARED_PREFERENCES_FAVORITE_FILE, 0);
    }

    //put a new set of paths into sharedPreferences
    private void setFavoritesStringSet(Set<String> favorites, SharedPreferences sharedPreferences) {
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putStringSet(SHARED_PREFERENCES_FAVORITE_STRING_SET, favorites);
        editor.apply();
        refreshFavoritesDrawerData();
    }

    /*
    * OTHER CLASSES
    * */
    //Adapts file path strings into list items for the favorites drawer
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

            ///Magic value passed in, return a title in the list
            String item = getItem(position);
            if (item.equals(MOUNT_POINTS_TITLE_MAGIC_VALUE) || item.equals(FAVORITES_TITLE_MAGIC_VALUE)) {
                TextView title = new TextView(context);
                if (item.equals(MOUNT_POINTS_TITLE_MAGIC_VALUE)) {
                    title.setText(R.string.mountpoints);
                } else if (item.equals(FAVORITES_TITLE_MAGIC_VALUE)) {
                    title.setText(R.string.favorites);
                }
                return title;
            }

            if (convertView == null || convertView instanceof TextView) {
                convertView = ((LayoutInflater)context.getSystemService(LAYOUT_INFLATER_SERVICE)).inflate(listItemResource, parent, false);
            }

            File file = new File(item);
            ImageView favoritesListItemImageView = (ImageView) convertView.findViewById(R.id.favoritesListItemImageView);
            TextView favoritesListItemTextView = (TextView) convertView.findViewById(R.id.favoritesListItemTextView);

            if (!file.exists()) {
                favoritesListItemTextView.setText(file.getName()+" "+getString(R.string.preappend_filename_does_not_exist));
            } else {
                favoritesListItemTextView.setText(file.getName());
            }

            if (file.isDirectory()) {
                favoritesListItemImageView.setImageDrawable(getDrawable(R.drawable.ic_folder));
            } else if (!file.exists()) {
                favoritesListItemImageView.setImageDrawable(getDrawable(R.drawable.ic_error));
            } else {
                favoritesListItemImageView.setImageDrawable(getDrawable(R.drawable.ic_file));
            }

            return convertView;
        }
        private Drawable getDrawable(int id) {
            return ResourcesCompat.getDrawable(getResources(), id, null);
        }
    }

}
