package com.dewdrop623.androidcrypt;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.Intent;
import android.content.res.TypedArray;
import android.net.Uri;
import android.os.Build;
import android.support.annotation.Nullable;
import android.text.SpannableString;
import android.text.style.UnderlineSpan;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

/**
 * Used in the about page. Displays the attribution text and links for icons from www.flaticon.com.
 */
public class IconAttributionView extends RelativeLayout {

    private ImageView iconAttributionImageView;
    private TextView iconCreatorTextView;
    private TextView flatIconLinkTextView;
    private TextView cc30LinkTextView;

    private String iconCreatorLink;
    private String iconCreatorName;

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
        cc30LinkTextView = (TextView) findViewById(R.id.cc30LinkTextView);
    }

    private void attrSetUp(Context context, AttributeSet attrs) {
        TypedArray attributesArray = context.getTheme().obtainStyledAttributes(attrs, R.styleable.IconAttributionView, 0, 0);
        try {
            iconAttributionImageView.setImageDrawable(attributesArray.getDrawable(R.styleable.IconAttributionView_icon_src));
            iconCreatorTextView.setText(attributesArray.getText(R.styleable.IconAttributionView_creator_name));
            iconCreatorLink = attributesArray.getString(R.styleable.IconAttributionView_creator_link);
            iconCreatorName = attributesArray.getString(R.styleable.IconAttributionView_creator_name);
            setUpOnClickListeners(context);
            formatTextViews();
        } finally {
            attributesArray.recycle();
        }
    }

    private void setUpOnClickListeners(final Context context) {
        iconCreatorTextView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                openUrlWithImplicitIntent(context, iconCreatorLink);
            }
        });
        flatIconLinkTextView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                openUrlWithImplicitIntent(context, context.getString(R.string.flaticon_link));
            }
        });
        cc30LinkTextView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                openUrlWithImplicitIntent(context, context.getString(R.string.cc30_link));
            }
        });
    }

    private void formatTextViews() {
        SpannableString creatorNameUnderlined = new SpannableString(iconCreatorName);
        creatorNameUnderlined.setSpan(new UnderlineSpan(), 0, creatorNameUnderlined.length(), 0);
        iconCreatorTextView.setText(creatorNameUnderlined);
    }

    private void openUrlWithImplicitIntent(Context context, String url) {
        Intent webIntent = new Intent(Intent.ACTION_VIEW);
        webIntent.setData(Uri.parse(url));
        context.startActivity(webIntent);
    }
}
