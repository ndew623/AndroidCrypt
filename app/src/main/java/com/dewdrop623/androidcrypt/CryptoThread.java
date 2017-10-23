package com.dewdrop623.androidcrypt;

import android.net.Uri;
import android.widget.Toast;

import java.io.File;
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

    private CryptoService cryptoService;
    private Uri inputUri;
    private Uri outputUri;
    private String password;
    private int version;
    boolean operationType;

    /**
     * Takes a cryptoService, input and output uris, the password, a version (use VERSION_X constants), and operation type (defined by the OPERATION_TYPE_X constants)
     */
    public CryptoThread(CryptoService cryptoService, Uri inputUri, Uri outputUri, String password, int version, boolean operationType) {
        this.cryptoService = cryptoService;
        this.inputUri = inputUri;
        this.outputUri = outputUri;
        this.password = password;
        this.version = version;
        this.operationType = operationType;
    }

    @Override
    public void run() {

        InputStream inputStream = null;
        //get the input stream
        try {
            inputStream = StorageAccessFrameworkHelper.getUriInputStream(cryptoService, inputUri);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            cryptoService.showToastOnGuiThread(R.string.error_could_not_get_input_file);
        }

        OutputStream outputStream = null;
        //get the output stream
        try {
            outputStream = StorageAccessFrameworkHelper.getUriOutputStream(cryptoService, outputUri);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            cryptoService.showToastOnGuiThread(R.string.error_could_not_get_output_file);
        }

        //call AESCrypt
        try {
            AESCrypt aesCrypt = new AESCrypt(password);
            if (operationType == OPERATION_TYPE_ENCRYPTION) {
                aesCrypt.encrypt(version, inputStream, outputStream);
            } else {
                aesCrypt.decrypt(new File(inputUri.getPath()).length(), inputStream, outputStream);//TODO, getting filesize this way doesn't work
            }
        } catch (GeneralSecurityException gse) {
            gse.printStackTrace();
            cryptoService.showToastOnGuiThread(R.string.error_platform_does_not_support_the_required_cryptographic_methods);
        } catch (UnsupportedEncodingException uee) {
            uee.printStackTrace();
            cryptoService.showToastOnGuiThread(R.string.error_utf16_encoding_is_not_supported);
        } catch (IOException ioe) {
            cryptoService.showToastOnGuiThread(ioe.getMessage());
        }

        //close the streams
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException ioe) {
                ioe.printStackTrace();
                //TODO, probably can't do this from the thread. Move it to somewhere else.
                cryptoService.showToastOnGuiThread(R.string.error_could_not_close_input_file);
            }
        }
        if (outputStream != null) {
            try {
                outputStream.close();
            } catch (IOException ioe) {
                //TODO, probably can't do this from the thread. Move it to somewhere else.
                cryptoService.showToastOnGuiThread(R.string.error_could_not_close_output_file);
            }
        }

        //stop the service, and remove the notification
        cryptoService.stopForeground(true);
    }
}
