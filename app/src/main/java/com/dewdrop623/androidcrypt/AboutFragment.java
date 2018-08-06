package com.dewdrop623.androidcrypt;


import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * This fragment is the about page accessed via the action bar from MainActivityFragment.
 */

public class AboutFragment extends Fragment {

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_about, container, false);
        if (SettingsHelper.getUseDarkTeme(getContext())) {
            LinearLayout rootLinearLayout = (LinearLayout) view.findViewById(R.id.rootLinearLayout);
            for (int i = 0; i < rootLinearLayout.getChildCount(); i++) {
                if (rootLinearLayout.getChildAt(i) instanceof TextView) {
                    ((TextView) rootLinearLayout.getChildAt(i)).setTextColor(((MainActivity) getActivity()).getDarkThemeColor(android.R.attr.textColorPrimary));
                }
            }
        }
        return view;
    }
}
