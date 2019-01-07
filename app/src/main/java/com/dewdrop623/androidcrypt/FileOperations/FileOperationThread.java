package com.dewdrop623.androidcrypt.FileOperations;

import android.support.v4.provider.DocumentFile;

import com.dewdrop623.androidcrypt.CryptoThread;
import com.dewdrop623.androidcrypt.GlobalDocumentFileStateHolder;
import com.dewdrop623.androidcrypt.R;

import java.io.File;
import java.util.HashMap;

public abstract class FileOperationThread extends Thread {

    public static boolean operationInProgress = false;

    public enum FILE_OP_TYPE  {ENCRYPTION_OP, DECRYPTION_OP, TAR_OP}

    private static HashMap<String, CryptoThread.ProgressDisplayer> progressDiplayers = new HashMap<>();

    public interface ProgressDisplayer {
        //[minutes|seconds]ToCompletion=-1 => unknown
        void update(FileOperationThread.FILE_OP_TYPE operationType, int progress, int completedMessageStringId, int minutesToCompletion, int secondsToCompletion);
    }

    private static final long updateIntervalInBytes = 550000;

    private static long timeOperationStarted = 0;
    private static long lastUpdateAtByteNumber = 0;
    private static long totalBytesRead = 0;
    private static long fileSize = 0;

    protected static FILE_OP_TYPE fileOpType;

    protected FileOperationService service;

    protected File inputFile;
    protected File outputFile;
    protected boolean deleteSource;
    protected static int completedMessageStringId = R.string.done;

    /**
     * deleteSource currently has no effect in tar operation
     */
    public FileOperationThread(FileOperationService service, File inputFile, File outputFile, boolean deleteSource) {
        this.service = service;
        this.inputFile = inputFile;
        this.outputFile = outputFile;
        this.deleteSource = deleteSource;
        setOpTypeAndCompletionString();
    }

    protected void initializeProgressTrackingValues() {
        lastUpdateAtByteNumber = 0;
        totalBytesRead = 0;
        timeOperationStarted = System.currentTimeMillis();
        //Send out an initial update for 0 progress.
        int [] timeToCompletionSetinalValue = {-1,-1};
        updateProgressDisplayers(0, 1, timeToCompletionSetinalValue);
    }
    protected void oneHundredPercentProgressUpdate() {
        int[] timeToCompletionSetinalValue = {0,0};
        updateProgressDisplayers(fileSize, fileSize, timeToCompletionSetinalValue);
    }
    protected abstract void setOpTypeAndCompletionString();
    public abstract void run();

    public static void updateProgressOnInterval(long bytesRead) {
        totalBytesRead += bytesRead;
        if (totalBytesRead - lastUpdateAtByteNumber > updateIntervalInBytes) {
            int [] timeToCompletion = getTimeToCompletion();
            lastUpdateAtByteNumber = totalBytesRead;
            updateProgressDisplayers(totalBytesRead, fileSize, timeToCompletion);
        }
    }
    //for each progress displayer: if not null: update, else remove it from progressDisplayers because it is null.
    public static void updateProgressDisplayers(long workDone, long totalWork, int [] timeToCompletion) {
        int progress = 100;
        if (totalWork != 0) {
            progress = (int) ((workDone * 100) / totalWork);
        }
        for (HashMap.Entry<String, CryptoThread.ProgressDisplayer> progressDisplayer : progressDiplayers.entrySet()) {
            if (progressDisplayer.getValue() != null) {
                progressDisplayer.getValue().update(fileOpType, progress, completedMessageStringId, timeToCompletion[0], timeToCompletion[1]);
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

    /**
     * TODO going to need a method in SAF to do this
     * @return
     */
    protected boolean deleteInputFile() {
        DocumentFile inputFile = GlobalDocumentFileStateHolder.getInputFileParentDirectory().findFile(inputFileName);
        if (inputFile != null) {
            return inputFile.delete();
        } else {
            return false;
        }
    }

    public static void registerForProgressUpdate(String id, FileOperationThread.ProgressDisplayer progressDisplayer) {
        progressDiplayers.put(id, progressDisplayer);
    }

    //Called by the cancel button in MainActivityFragment.
    public static void cancel() {
        if (fileOpType == FILE_OP_TYPE.ENCRYPTION_OP) {
            completedMessageStringId = R.string.encryption_canceled;
        } else if (fileOpType == FILE_OP_TYPE.DECRYPTION_OP) {
            completedMessageStringId = R.string.decryption_canceled;
        } else if (fileOpType == FILE_OP_TYPE.TAR_OP) {
            completedMessageStringId = R.string.archiving_canceled;
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
            progress = (int) ((totalBytesRead * 100) / fileSize);
        }
        return progress;
    }

    public static FILE_OP_TYPE getCurrentOperationType() {
        return fileOpType;
    }

    public static int getCompletedMessageStringId() {
        return completedMessageStringId;
    }
}
