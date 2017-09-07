package com.dewdrop623.androidaescrypt.FileBrowsing.ui.view;

import android.content.Context;
import android.util.AttributeSet;
import android.support.v7.widget.AppCompatImageButton;

/**
 * A custom ImageButton that has a TextView under it
 */
//TODO set content to be layout/view_captioned_image_button
//TODO set up button using values from attrs
public class CaptionedImageButton extends  AppCompatImageButton{
    public CaptionedImageButton(Context context) {
        super(context);
    }

    public CaptionedImageButton(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public CaptionedImageButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }
}
