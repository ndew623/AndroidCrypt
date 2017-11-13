package com.dewdrop623.androidcrypt;

import android.net.Uri;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;
import java.util.HashMap;

import es.vocali.util.AESCrypt;

/**
 * CryptoThread handles the usage of AESCrypt to do crypto operations.
 * CryptoThread is intended to be initialized and started by a CryptoService instance.
 */

public class CryptoThread extends Thread {

    //Do not do more than one operation at once.
    public static boolean operationInProgress = false;

    /*
    * Constants.
     */
    public static final boolean OPERATION_TYPE_ENCRYPTION = true;
    public static final boolean OPERATION_TYPE_DECRYPTION = false;
    public static final int VERSION_1 = 1;
    public static final int VERSION_2 = 2;

    private static HashMap<String, ProgressDisplayer> progressDiplayers = new HashMap<>();
    public interface ProgressDisplayer {
        void update(boolean operationType, int progress);
    }

    private static long twoPercentOfFileSize = 0;
    private static long lastUpdateAtByteNumber = 0;
    private static long totalBytesReadForProgress = 0;
    private static long fileSize = 0;

    private CryptoService cryptoService;
    private Uri inputUri;
    private Uri outputUri;
    private String password;
    private int version;
    private boolean operationType;

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
        operationInProgress = true;
        InputStream inputStream = null;
        OutputStream outputStream = null;
        //get the input stream
        try {
            inputStream = StorageAccessFrameworkHelper.getUriInputStream(cryptoService, inputUri);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            cryptoService.showToastOnGuiThread(R.string.error_could_not_get_input_file);
        }

        //get the output stream
        try {
            outputStream = StorageAccessFrameworkHelper.getUriOutputStream(cryptoService, outputUri);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            cryptoService.showToastOnGuiThread(R.string.error_could_not_get_output_file);
        }

        if (inputStream != null && outputStream != null) {
            //call AESCrypt
            try {
                fileSize = StorageAccessFrameworkHelper.getFileSizeFromUri(inputUri, cryptoService);
                if (fileSize == 0) {
                    fileSize = inputStream.available();
                }
                twoPercentOfFileSize = (long) (fileSize*.02f);
                AESCrypt aesCrypt = new AESCrypt(password);
                if (operationType == OPERATION_TYPE_ENCRYPTION) {
                    //Encrypt
                    aesCrypt.encrypt(version, inputStream, outputStream);
                } else {
                    //Decrypt
                    aesCrypt.decrypt(fileSize, inputStream, outputStream);
                }
            } catch (GeneralSecurityException gse) {
                gse.printStackTrace();
                cryptoService.showToastOnGuiThread(R.string.error_platform_does_not_support_the_required_cryptographic_methods);
            } catch (UnsupportedEncodingException uee) {
                uee.printStackTrace();
                cryptoService.showToastOnGuiThread(R.string.error_utf16_encoding_is_not_supported);
            } catch (IOException ioe) {
                cryptoService.showToastOnGuiThread(ioe.getMessage());
            } catch (NullPointerException npe) {
                cryptoService.showToastOnGuiThread(npe.getMessage());
            }

            //close the streams
            try {
                inputStream.close();
            } catch (IOException ioe) {
                ioe.printStackTrace();
                cryptoService.showToastOnGuiThread(R.string.error_could_not_close_input_file);
            }
            try {
                outputStream.close();
            } catch (IOException ioe) {
                cryptoService.showToastOnGuiThread(R.string.error_could_not_close_output_file);
            }
        }

        //stop the service, and remove the notification
        cryptoService.stopForeground(true);
        cryptoService.stopSelf();
        operationInProgress = false;
    }

    public static void updateProgress(boolean operationType, long bytesRead) {
        totalBytesReadForProgress += bytesRead;
        if (totalBytesReadForProgress - lastUpdateAtByteNumber > twoPercentOfFileSize) {
            lastUpdateAtByteNumber = totalBytesReadForProgress;
            updateProgressDisplayers(operationType, totalBytesReadForProgress, fileSize);
        }
    }

    //for each progress displayer: if not null: update, else remove it from progressDisplayers because it is null.
    private static void updateProgressDisplayers(boolean operationType, long workDone, long totalWork) {
        int progress = (int) ((workDone*100)/totalWork);
        for(HashMap.Entry<String, ProgressDisplayer> progressDisplayer : progressDiplayers.entrySet()) {
            if (progressDisplayer.getValue() != null) {
                progressDisplayer.getValue().update(operationType, progress);
            } else {
                progressDiplayers.remove(progressDisplayer.getKey());
            }
        }
    }

    public static void registerForProgressUpdate(String id, ProgressDisplayer progressDisplayer) {
        progressDiplayers.put(id, progressDisplayer);
    }
}
