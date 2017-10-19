package com.dewdrop623.androidcrypt;

import android.content.Context;
import android.net.Uri;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;

import es.vocali.util.AESCrypt;

/**
 * does the crypto operations. meant to  be run in CryptoService
 */

public class CryptoThread extends Thread {
    public static final boolean OPERATION_TYPE_ENCRYPTION = true;
    public static final boolean OPERATION_TYPE_DECRYPTION = false;

    public static final int VERSION_1 = 1;
    public static final int VERSION_2 = 2;

    private Context context;
    private Uri inputUri;
    private Uri outputUri;
    private String password;
    private int version;

    /**
     * Takes a context, input and output uris, the password, a version (use VERSION_X constants), and operation type (defined by the OPERATION_TYPE_X constants)
     */
    public CryptoThread(Context context, Uri inputUri, Uri outputUri, String password, int version, boolean operationType) {
        this.context = context;
        this.inputUri = inputUri;
        this.outputUri = outputUri;
        this.password = password;
        this.version = version;
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

        OutputStream outputStream = null;
        //get the output stream
        try {
            outputStream = StorageAccessFrameworkHelper.getUriOutputStream(context, outputUri);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            //TODO, probably can't do this from the thread. Move it to somewhere else.
            Toast.makeText(context, R.string.error_could_not_get_output_file, Toast.LENGTH_SHORT);
        }

        //call AESCrypt
        try {
            AESCrypt aesCrypt = new AESCrypt(password);
            aesCrypt.encrypt(version, inputStream, outputStream);
        } catch (GeneralSecurityException gse) {
            gse.printStackTrace();
            //TODO, probably can't do this from the thread. Move it to somewhere else.
            Toast.makeText(context, R.string.error_platform_does_not_support_the_required_cryptographic_methods, Toast.LENGTH_SHORT);
        } catch (UnsupportedEncodingException uee) {
            uee.printStackTrace();
            //TODO, probably can't do this from the thread. Move it to somewhere else.
            Toast.makeText(context, R.string.error_utf16_encoding_is_not_supported, Toast.LENGTH_SHORT);
        } catch (IOException ioe) {
            //TODO, probably can't do this from the thread. Move it to somewhere else.
            Toast.makeText(context, R.string.error_cryto_operation_encountered_ioexception, Toast.LENGTH_SHORT);
        }

        //close the streams
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException ioe) {
                ioe.printStackTrace();
                //TODO, probably can't do this from the thread. Move it to somewhere else.
                Toast.makeText(context, R.string.error_could_not_close_input_file, Toast.LENGTH_SHORT);
            }
        }
        if (outputStream != null) {
            try {
                outputStream.close();
            } catch (IOException ioe) {
                //TODO, probably can't do this from the thread. Move it to somewhere else.
                Toast.makeText(context, R.string.error_could_not_close_output_file, Toast.LENGTH_SHORT);
            }
        }
    }
}
