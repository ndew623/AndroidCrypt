package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.net.Uri;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;

/**
 * does the crypto operations. meant to  be run in CryptoService
 */

public class CryptoThread extends Thread {
    public static final boolean OPERATION_TYPE_ENCRYPTION = true;
    public static final boolean OPERATION_TYPE_DECRYPTION = false;

    private Context context;
    private Uri inputUri;
    private Uri outputUri;

    /**
     * Takes a context, input and output uris, and and operation type (defined by the OPERATION_TYPE_X constants)
     */
    public CryptoThread(Context context, Uri inputUri, Uri outputUri, boolean operationType) {
        this.context = context;
        this.inputUri = inputUri;
        this.outputUri = outputUri;
    }

    @Override
    public void run() {

        InputStream inputStream = null;
        //get the input stream
        try {
            inputStream = StorageAccessFrameworkHelper.getUriInputStream(context, inputUri);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            //TODO, probably can't do this from the thread. Move it to somewhere else.
            Toast.makeText(context, R.string.error_could_not_get_input_file, Toast.LENGTH_SHORT);
        }

        //get the output stream

        //call AESCrypt

        //close the streams
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException ioe) {
                ioe.printStackTrace();
                //TODO, probably can't do this from the thread. Move it to somewhere else.
                Toast.makeText(context, R.string.error_could_close_input_file, Toast.LENGTH_SHORT);
            }

        }
    }
}
