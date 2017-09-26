package com.dewdrop623.androidcrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidcrypt.R;

import java.io.File;
import java.util.Stack;

/**
 * interface for classes that do work on files, implements runnable: to be done on seperate thread
 */

public abstract class FileOperator{
    protected File file;
    protected Bundle args;
    protected boolean silent = false;
    protected FileModifierService fileModifierService;
    private static Stack<FileOperator> waitingForYesNoResponse = new Stack<>();
    private static Stack<FileOperator> waitingForYesNoRememberAnswerResponse = new Stack<>();
    private static Stack<FileOperator> waitingForTextOrCancelResponse = new Stack<>();
    private Thread operationThread;
    public FileOperator(File file, Bundle args, FileModifierService fileModifierService) {
        this.file = file;
        this.args = args;
        this.fileModifierService = fileModifierService;
    }

    public String getOperationName() {
        return fileModifierService.getString(R.string.file_operation);
    }

    public void run() {
        initMemVarFromArgs();
        if (prepareAndValidate()) {
            getInfoFromUser();
        } else {
            cancelOperation();
        }
    }

    public void cancelOperation() {
        if (operationThread != null) {
            operationThread.interrupt();
            operationThread = null;
        }
        waitingForYesNoResponse.remove(this);
        waitingForYesNoRememberAnswerResponse.remove(this);
        waitingForTextOrCancelResponse.remove(this);
        fileModifierService.stopSelf();
    }

    public void runSilentNoThread() {
        //optional override
        silent = true;
    }

    protected abstract void initMemVarFromArgs();
    protected abstract boolean prepareAndValidate();
    protected abstract void getInfoFromUser();
    protected abstract void doOperation();

    protected abstract void handleYesNoResponse(boolean yes);
    protected abstract void handleYesNoRememberAnswerResponse(boolean yes, boolean remember);
    protected abstract void handleTextOrCancelResponse(String response);


    public static void userResponse(boolean yes) {
        waitingForYesNoResponse.pop().handleYesNoResponse(yes);
    }
    public static void userResponse(boolean yes, boolean remember) {
        waitingForYesNoRememberAnswerResponse.pop().handleYesNoRememberAnswerResponse(yes, remember);
    }
    public static void userResponse(String response) {
        waitingForTextOrCancelResponse.pop().handleTextOrCancelResponse(response);
    }
    protected void askYesNo(String question) {
        waitingForYesNoResponse.add(this);
        fileModifierService.askYesNo(question);
    }
    protected void askYesNoRememberAnswer(String question, int numberOfEvents, String typeOfEventString) {
        waitingForYesNoRememberAnswerResponse.add(this);
        fileModifierService.askYesNoRememberAnswer(question, numberOfEvents, typeOfEventString);
    }
    protected void askForTextOrCancel(String question) { //if response null then cancel
        waitingForTextOrCancelResponse.add(this);
        fileModifierService.askForTextOrCancel(question);
    }
    protected void finishTakingInput() {
        operationThread = new Thread(new Runnable() {
            @Override
            public void run() {
                doOperation();
                fileModifierService.stopSelf();
            }
        });
        operationThread.start();
    }
}
