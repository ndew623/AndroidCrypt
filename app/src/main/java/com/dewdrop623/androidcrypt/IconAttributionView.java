package com.dewdrop623.androidcrypt;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.TypedArray;
import android.os.Build;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

/**
 * Displays the attribution text and links for icons from www.flaticon.com
 */

public class IconAttributionView extends RelativeLayout {

    private ImageView iconAttributionImageView;
    private TextView iconCreatorTextView;
    private TextView flatIconLinkTextView;

    private String iconCreatorLink;

    public IconAttributionView(Context context) {
        super(context);
        constructorTasks(context);
    }

    public IconAttributionView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        constructorTasks(context);
        attrSetUp(context, attrs);
    }

    public IconAttributionView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        constructorTasks(context);
        attrSetUp(context, attrs);
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public IconAttributionView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        constructorTasks(context);
        attrSetUp(context, attrs);
    }

    private void constructorTasks(Context context) {
        inflate(context, R.layout.view_icon_attribution, this);
        iconAttributionImageView = (ImageView) findViewById(R.id.iconAttributionImageView);
        iconCreatorTextView = (TextView) findViewById(R.id.iconCreatorTextView);
        flatIconLinkTextView = (TextView) findViewById(R.id.flatIconLinkTextView);
    }

    private void attrSetUp(Context context, AttributeSet attrs) {
        TypedArray attributesArray = context.getTheme().obtainStyledAttributes(attrs, R.styleable.IconAttributionView, 0, 0);
        try {
            iconAttributionImageView.setImageDrawable(attributesArray.getDrawable(R.styleable.IconAttributionView_icon_src));
            iconCreatorTextView.setText(attributesArray.getText(R.styleable.IconAttributionView_creator_name));
            iconCreatorLink = attributesArray.getString(R.styleable.IconAttributionView_src_link);
            setUpOnClickListeners();
            formatTextViews();
        } finally {
            attributesArray.recycle();
        }
    }

    private void setUpOnClickListeners() {
        iconCreatorTextView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                //TODO send intent to open iconCreatorLink
            }
        });
        flatIconLinkTextView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                //TODO put www.flaticon.com in a string resource and send intent here to open it
            }
        });
    }

    private void formatTextViews() {
        //TODO underline iconCreatorLink
    }
}
