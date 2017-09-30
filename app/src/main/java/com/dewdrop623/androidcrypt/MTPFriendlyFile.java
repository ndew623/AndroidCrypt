package com.dewdrop623.androidcrypt;

import android.support.annotation.NonNull;

import java.io.File;

/**
 * essentially just a java file that updates the mtp cache so files appear over usb
 */

public class MTPFriendlyFile extends File {
    public MTPFriendlyFile(@NonNull String pathname) {
        super(pathname);
    }

}
