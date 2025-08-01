package com.dewdrop623.androidcrypt;

import static java.util.Map.entry;

import android.net.Uri;
import android.support.v4.provider.DocumentFile;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.security.GeneralSecurityException;
import java.util.HashMap;
import java.util.Map;

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

    private static final HashMap<String, ProgressDisplayer> progressDiplayers = new HashMap<>();

    public interface ProgressDisplayer {
        //[minutes|seconds]ToCompletion=-1 => unknown
        void update(boolean operationType, int progress, int completedMessageStringId, int minutesToCompletion, int secondsToCompletion);
    }

    private static final long updateIntervalInBytes = 550000;

    private static long timeOperationStarted = 0;
    private static long lastUpdateAtByteNumber = 0;
    private static long totalBytesRead = 0;
    private static long fileSize = 0;

    private final CryptoService cryptoService;
    private static boolean operationType;

    private final Uri inputFile;
    private final Uri outputFile;
    private final String password;
    private boolean deleteInputFile = false;
    private static int completedMessageStringId = R.string.done;

    private Map<Integer, Integer> encryptResultStatusMessages = new HashMap<>();
    private Map<Integer, Integer> decryptResultStatusMessages = new HashMap<>();

    /**
     * Takes a cryptoService, input and output uris, the password, a version (use VERSION_X constants), and operation type (defined by the OPERATION_TYPE_X constants)
     */
    public CryptoThread(CryptoService cryptoService, Uri inputFile, Uri outputFile, String password, boolean operationType, boolean deleteInputFile) {
        this.cryptoService = cryptoService;
        this.inputFile = inputFile;
        this.outputFile = outputFile;
        this.password = password;
        this.deleteInputFile = deleteInputFile;
        CryptoThread.operationType = operationType;

        encryptResultStatusMessages.put(1, R.string.encryption_completed);
        encryptResultStatusMessages.put(2, R.string.io_error);
        encryptResultStatusMessages.put(3, R.string.invalid_extension_error);
        encryptResultStatusMessages.put(4, R.string.invalid_password_error);
        encryptResultStatusMessages.put(5, R.string.invalid_iterations_error);
        encryptResultStatusMessages.put(6, R.string.already_encrypting);
        encryptResultStatusMessages.put(7, R.string.encryption_canceled);
        encryptResultStatusMessages.put(8, R.string.aescrypt_internal_error);

        decryptResultStatusMessages.put(101, R.string.decryption_completed);
        decryptResultStatusMessages.put(102, R.string.invalid_aescrypt_stream_error);
        decryptResultStatusMessages.put(103, R.string.unsupported_aescrypt_version_error);
        decryptResultStatusMessages.put(104, R.string.io_error);
        decryptResultStatusMessages.put(105, R.string.invalid_password_error);
        decryptResultStatusMessages.put(106, R.string.invalid_iterations_error);
        decryptResultStatusMessages.put(107, R.string.altered_message_error);
        decryptResultStatusMessages.put(108, R.string.already_decrypting);
        decryptResultStatusMessages.put(109, R.string.decryption_canceled);
        decryptResultStatusMessages.put(110, R.string.aescrypt_internal_error);
    }


    @Override
    public void run() {
        boolean successful = true;
        operationInProgress = true;
        lastUpdateAtByteNumber = 0;
        totalBytesRead = 0;
        timeOperationStarted = 0;

        if (operationType == OPERATION_TYPE_ENCRYPTION) {
            completedMessageStringId = R.string.encryption_completed;
        } else {
            completedMessageStringId = R.string.decryption_completed;
        }

        //Send out an initial update for 0 progress.
        int [] timeToCompletion = {-1,-1};
        updateProgressDisplayers(0, 1, timeToCompletion);
        InputStream inputStream = null;
        OutputStream outputStream = null;
        //get the input stream
        try {
            inputStream = cryptoService.getContentResolver().openInputStream(inputFile);
            fileSize = inputStream.available();
        } catch (IOException ioe) {
            successful = false;
            ioe.printStackTrace();
            completedMessageStringId = R.string.error_could_not_get_input_file;
        }

        //get the output stream
        try {
            outputStream = cryptoService.getContentResolver().openOutputStream(outputFile);
        } catch (IOException ioe) {
            successful = false;
            ioe.printStackTrace();
            completedMessageStringId = R.string.error_could_not_open_output_file;
        }

        if (inputStream != null && outputStream != null) {
            //call AESCrypt
            LogStream logStream = new LogStream("JNI Execution");
            long progressFrequencyBytes = Math.max((long)(fileSize*0.01), 100l);
            if (operationType == OPERATION_TYPE_ENCRYPTION) {
                successful = JNIInterface.encrypt(password, inputStream, outputStream, jniCallbackInterface, logStream, progressFrequencyBytes);
            } else if (operationType == OPERATION_TYPE_DECRYPTION) {
                successful = JNIInterface.decrypt(password, inputStream, outputStream, jniCallbackInterface, logStream, progressFrequencyBytes);
            }
        }

        //close the streams
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException ioe) {
                successful = false;
                ioe.printStackTrace();
                completedMessageStringId = R.string.error_could_not_close_input_file;
            }
        }
        if (outputStream != null) {
            try {
                outputStream.close();
            } catch (IOException ioe) {
                successful = false;
                ioe.printStackTrace();
                completedMessageStringId = R.string.error_could_not_close_output_file;
            }
        }

        //Send out one last progress update. It is important that ProgressDisplayers get the final update at 100%. Even if the operation was canceled.
        timeToCompletion[0]=0; timeToCompletion[1]=0;
        updateProgressDisplayers(fileSize, fileSize, timeToCompletion);

        /*
        if operation didn't encounter errors and was not canceled (i.e. successful == true),
        and user asked (deleteInputFile == true):
        delete the input file
         */
        if (successful && deleteInputFile) {
            boolean successfullyDeleted = deleteInputFile();
            if (!successfullyDeleted) {
                cryptoService.showToastOnGuiThread(R.string.failed_to_delete_input_file);
            }
        }

        //stop the service
        cryptoService.stopForeground(false);
        operationInProgress = false;
    }

    public static void updateProgressOnIntervalTotal(long totalBytesRead) {
        if (timeOperationStarted == 0) {
            //set time operation started on first progress update,
            //because version 3 encryption with jni takes awhile to start
            //and that throws off the estimate to completion
            //maybe it has to load the binary each time or something
            timeOperationStarted = System.currentTimeMillis();
        }
        CryptoThread.totalBytesRead = totalBytesRead;
        if (totalBytesRead - lastUpdateAtByteNumber > updateIntervalInBytes) {
            int [] timeToCompletion = getTimeToCompletion();
            lastUpdateAtByteNumber = totalBytesRead;
            updateProgressDisplayers(totalBytesRead, fileSize, timeToCompletion);
        }
    }

    public static void updateProgressOnInterval(long bytesRead) {
        totalBytesRead += bytesRead;
        updateProgressOnIntervalTotal(totalBytesRead);
    }

    //for each progress displayer: if not null: update, else remove it from progressDisplayers because it is null.
    private static void updateProgressDisplayers(long workDone, long totalWork, int [] timeToCompletion) {
        int progress = 100;
        if (totalWork != 0) {
            progress = (int) ((workDone * 100) / totalWork);
        }
        for (HashMap.Entry<String, ProgressDisplayer> progressDisplayer : progressDiplayers.entrySet()) {
            if (progressDisplayer.getValue() != null) {
                progressDisplayer.getValue().update(operationType, progress, completedMessageStringId, timeToCompletion[0], timeToCompletion[1]);
            } else {
                progressDiplayers.remove(progressDisplayer.getKey());
            }
        }
    }

    /**
     * Calculate time until operation finishes using the file size, bytes since last update, and time since last update.
     * return int array [minutes, seconds]
     */
    private static int[] getTimeToCompletion() {
        int[] timeToCompletion = {0, 0};
        long bytesPerMillisecond = totalBytesRead/(System.currentTimeMillis()-timeOperationStarted);
        long bytesPerSecond = bytesPerMillisecond*1000;
        if (bytesPerSecond != 0) {
            int secondsToCompletion = (int) ((fileSize - totalBytesRead) / bytesPerSecond);
            timeToCompletion[0] = secondsToCompletion / 60; timeToCompletion[1] = secondsToCompletion % 60;
        }
        return timeToCompletion;
    }

    private boolean deleteInputFile() {
        if (inputFile != null) {
            return DocumentFile.fromSingleUri(cryptoService, inputFile).delete();
        } else {
            return false;
        }
    }

    public static void registerForProgressUpdate(String id, ProgressDisplayer progressDisplayer) {
        progressDiplayers.put(id, progressDisplayer);
    }

    //Called by the cancel button in MainActivityFragment.
    public static void cancel() {
        JNIInterface.cancel();
        operationInProgress = false;
    }

    //Called by MainActivityFragment on initialization if CryptoThread.operationInProgress == true
    //otherwise the progress bar won't appear until an update is sent out, which is not guaranteed to be quickly
    public static int getProgressUpdate() {
        int progress = 0;
        if (fileSize == 0) {
            progress = 100;
        } else if (operationInProgress) {
            progress = (int) ((totalBytesRead * 100) / fileSize);
        }
        return progress;
    }

    public static boolean getCurrentOperationType() {
        return operationType;
    }

    public static int getCompletedMessageStringId() {
        return completedMessageStringId;
    }
    private JNICallbackInterface jniCallbackInterface = new JNICallbackInterface() {
        @Override
        public void progressCallback(long totalBytes) {
            updateProgressOnIntervalTotal(totalBytes);
        }
        @Override
        public void completedCallback(int status) {
            int message_string_id = R.string.unknown_status;
            if (getCurrentOperationType() == OPERATION_TYPE_ENCRYPTION && encryptResultStatusMessages.containsKey(status)) {
                message_string_id = encryptResultStatusMessages.get(status);
            } else if (decryptResultStatusMessages.containsKey(status)){
                message_string_id = decryptResultStatusMessages.get(status);
            }
            completedMessageStringId = message_string_id;
            cryptoService.showToastOnGuiThread(message_string_id);
        }
    };
}
