package com.dewdrop623.androidcrypt;

import java.io.File;
import java.io.FileInputStream;
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
        void update(boolean operationType, int progress, int completedMessageStringId);
    }

    private static final long updateIntervalInBytes = 4096;

    private static long lastUpdateAtByteNumber = 0;
    private static long totalBytesReadForProgress = 0;
    private static long fileSize = 0;

    private CryptoService cryptoService;
    private static boolean operationType;

    private String inputFileName;
    private String outputFileName;
    private String password;
    private int version;
    private static int completedMessageStringId = R.string.done;

    /**
     * Takes a cryptoService, input and output uris, the password, a version (use VERSION_X constants), and operation type (defined by the OPERATION_TYPE_X constants)
     */
    public CryptoThread(CryptoService cryptoService, String inputFileName, String outputFileName, String password, int version, boolean operationType) {
        this.cryptoService = cryptoService;
        this.inputFileName = inputFileName;
        this.outputFileName = outputFileName;
        this.password = password;
        this.version = version;
        this.operationType = operationType;
    }

    @Override
    public void run() {
        operationInProgress = true;
        lastUpdateAtByteNumber = 0;
        totalBytesReadForProgress = 0;

        if (operationType == OPERATION_TYPE_ENCRYPTION) {
            completedMessageStringId = R.string.encryption_completed;
        } else {
            completedMessageStringId = R.string.decryption_completed;
        }

        //Send out an initial update for 0 progress.
        updateProgressDisplayers(0, 1);
        InputStream inputStream = null;
        OutputStream outputStream = null;
        //get the input stream
        try {
            inputStream = StorageAccessFrameworkHelper.getFileInputStream(cryptoService, inputFileName);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            cryptoService.showToastOnGuiThread(R.string.error_could_not_get_input_file);
        }

        //get the output stream
        try {
            outputStream = StorageAccessFrameworkHelper.getFileOutputStream(cryptoService, outputFileName);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            cryptoService.showToastOnGuiThread(ioe.getMessage());
        }

        if (inputStream != null && outputStream != null) {
            //call AESCrypt
            try {
                fileSize = inputStream.available();
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
        }

        //close the streams
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException ioe) {
                ioe.printStackTrace();
                cryptoService.showToastOnGuiThread(R.string.error_could_not_close_input_file);
            }
        }
        if (outputStream != null) {
            try {
                outputStream.close();
            } catch (IOException ioe) {
                cryptoService.showToastOnGuiThread(R.string.error_could_not_close_output_file);
            }
        }

        //Send out one last progress update. It is important that ProgressDisplayers get the final update at 100%. Even if the operation was canceled.
        updateProgressDisplayers(fileSize, fileSize);

        //stop the service
        cryptoService.stopForeground(false);
        operationInProgress = false;
    }

    public static void updateProgressOnInterval(long bytesRead) {
        totalBytesReadForProgress += bytesRead;
        if (totalBytesReadForProgress - lastUpdateAtByteNumber > updateIntervalInBytes) {
            lastUpdateAtByteNumber = totalBytesReadForProgress;
            updateProgressDisplayers(totalBytesReadForProgress, fileSize);
        }
    }

    //for each progress displayer: if not null: update, else remove it from progressDisplayers because it is null.
    private static void updateProgressDisplayers(long workDone, long totalWork) {
        int progress = 100;
        if (totalWork != 0) {
            progress = (int) ((workDone * 100) / totalWork);
        }
        for (HashMap.Entry<String, ProgressDisplayer> progressDisplayer : progressDiplayers.entrySet()) {
            if (progressDisplayer.getValue() != null) {
                progressDisplayer.getValue().update(operationType, progress, completedMessageStringId);
            } else {
                progressDiplayers.remove(progressDisplayer.getKey());
            }
        }
    }

    public static void registerForProgressUpdate(String id, ProgressDisplayer progressDisplayer) {
        progressDiplayers.put(id, progressDisplayer);
    }

    //Called by the cancel button in MainActivityFragment.
    public static void cancel() {
        if (operationType == OPERATION_TYPE_ENCRYPTION) {
            completedMessageStringId = R.string.encryption_canceled;
        } else {
            completedMessageStringId = R.string.decryption_canceled;
        }
        operationInProgress = false;
    }

    //Called by MainActivityFragment on initialization if CryptoThread.operationInProgress == true
    //otherwise the progress bar won't appear until an update is sent out, which is not guaranteed to be quickly
    public static int getProgressUpdate() {
        int progress = 0;
        if (fileSize == 0) {
            progress = 100;
        } else if (operationInProgress) {
            progress = (int) ((totalBytesReadForProgress * 100) / fileSize);
        }
        return progress;
    }

    public static boolean getCurrentOperationType() {
        return operationType;
    }

    public static int getCompletedMessageStringId() {
        return completedMessageStringId;
    }
}
