package com.dewdrop623.androidcrypt.FileBrowsing.ui.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.dewdrop623.androidcrypt.R;

/**
 * A custom ImageButton that has a TextView under it
 */

public class CaptionedImageButton extends RelativeLayout{
    private static final float ICON_SCALE_FACTOR = 0.6f;
    private TextView textView;
    private ImageView frontImageView;
    private ImageView backImageView;
    public CaptionedImageButton(Context context) {
        super(context);
        constructorTasks(context);
    }

    public CaptionedImageButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        constructorTasks(context);
        attrSetUp(context, attrs);
    }

    public CaptionedImageButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        constructorTasks(context);
        attrSetUp(context, attrs);
    }
    //Called by all constructors
    //inflates the layout for this view
    //defines references to child views
    private void constructorTasks(Context context) {
        inflate(context, R.layout.view_captioned_image_button, this);
        textView = (TextView) findViewById(R.id.captionedImageButtonTextView);
        frontImageView = (ImageView) findViewById(R.id.captionedImageButtonFrontImageView);
        backImageView = (ImageView) findViewById(R.id.captionedImageButtonBackImageView);
    }

    //applies attributes from attrs to view, called by constructors with an AttributeSet parameter
    private void attrSetUp(Context context, AttributeSet attrs) {
        TypedArray attrArray = context.getTheme().obtainStyledAttributes(attrs, R.styleable.CaptionedImageButton, 0, 0);
        try {
            textView.setText(attrArray.getString(R.styleable.CaptionedImageButton_caption));

            frontImageView.setImageDrawable(attrArray.getDrawable(R.styleable.CaptionedImageButton_img_src));
            frontImageView.getLayoutParams().height = (int) (attrArray.getLayoutDimension(R.styleable.CaptionedImageButton_img_height_dp, ViewGroup.LayoutParams.WRAP_CONTENT)*ICON_SCALE_FACTOR);
            frontImageView.getLayoutParams().width = (int) (attrArray.getLayoutDimension(R.styleable.CaptionedImageButton_img_width_dp, ViewGroup.LayoutParams.WRAP_CONTENT)*ICON_SCALE_FACTOR);

            backImageView.setImageDrawable(attrArray.getDrawable(R.styleable.CaptionedImageButton_img_background));
            backImageView.getLayoutParams().height = attrArray.getLayoutDimension(R.styleable.CaptionedImageButton_img_height_dp, ViewGroup.LayoutParams.WRAP_CONTENT);
            backImageView.getLayoutParams().width = attrArray.getLayoutDimension(R.styleable.CaptionedImageButton_img_width_dp, ViewGroup.LayoutParams.WRAP_CONTENT);


        } finally {
            attrArray.recycle();
        }
    }
}
