package com.dewdrop623.androidcrypt;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.Intent;
import android.content.res.TypedArray;
import android.net.Uri;
import android.os.Build;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v7.content.res.AppCompatResources;
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
    private TextView srcLinkTextView;
    private TextView licenseLinkTextView;

    private String iconCreatorLink;
    private String iconCreatorName;

    private String srcName;
    private String srcLink;

    private String licenseName;
    private String licenseLink;

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
        srcLinkTextView = (TextView) findViewById(R.id.srcLinkTextView);
        licenseLinkTextView = (TextView) findViewById(R.id.licenseLinkTextView);
        /*
         * apply theme to ui
         */
        if (SettingsHelper.getUseDarkTeme(context)) {
            int color = context.getTheme().obtainStyledAttributes(R.style.AppThemeDark, new int[] {android.R.attr.textColorPrimary}).getColor(0,0);
            ((TextView) findViewById(R.id.iconMadeByTextView)).setTextColor(color);
            ((TextView) findViewById(R.id.fromTextView)).setTextColor(color);
            ((TextView) findViewById(R.id.licensedByTextView)).setTextColor(color);
        }
    }

    private void attrSetUp(Context context, AttributeSet attrs) {
        TypedArray attributesArray = context.getTheme().obtainStyledAttributes(attrs, R.styleable.IconAttributionView, 0, 0);
        try {
            iconAttributionImageView.setImageDrawable(AppCompatResources.getDrawable(getContext(), attributesArray.getResourceId(R.styleable.IconAttributionView_icon_src, R.drawable.ic_cancel)));
            iconCreatorName = attributesArray.getString(R.styleable.IconAttributionView_creator_name);
            iconCreatorLink = attributesArray.getString(R.styleable.IconAttributionView_creator_link);
            srcName = attributesArray.getString(R.styleable.IconAttributionView_src_name);
            srcLink = attributesArray.getString(R.styleable.IconAttributionView_src_link);
            licenseName = attributesArray.getString(R.styleable.IconAttributionView_license_name);
            licenseLink = attributesArray.getString(R.styleable.IconAttributionView_license_link);
            boolean tintGrey = attributesArray.getBoolean(R.styleable.IconAttributionView_tint_grey, true);
            if (tintGrey) {
                iconAttributionImageView.setColorFilter(ContextCompat.getColor(context, android.R.color.darker_gray), android.graphics.PorterDuff.Mode.SRC_IN);
            }
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
        srcLinkTextView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                openUrlWithImplicitIntent(context, srcLink);
            }
        });
        licenseLinkTextView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                openUrlWithImplicitIntent(context, licenseLink);
            }
        });
    }

    private void formatTextViews() {
        SpannableString creatorNameUnderlined = new SpannableString(iconCreatorName);
        creatorNameUnderlined.setSpan(new UnderlineSpan(), 0, creatorNameUnderlined.length(), 0);
        iconCreatorTextView.setText(creatorNameUnderlined);
        SpannableString srcNameUnderlined = new SpannableString(srcName);
        srcNameUnderlined.setSpan(new UnderlineSpan(), 0, srcNameUnderlined.length(), 0);
        srcLinkTextView.setText(srcNameUnderlined);
        SpannableString licenseNameUnderlined = new SpannableString(licenseName);
        licenseNameUnderlined.setSpan(new UnderlineSpan(), 0, licenseName.length(), 0);
        licenseLinkTextView.setText(licenseNameUnderlined);
    }

    private void openUrlWithImplicitIntent(Context context, String url) {
        Intent webIntent = new Intent(Intent.ACTION_VIEW);
        webIntent.setData(Uri.parse(url));
        context.startActivity(webIntent);
    }
}
