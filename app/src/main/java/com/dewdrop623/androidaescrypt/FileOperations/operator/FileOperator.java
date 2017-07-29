package com.dewdrop623.androidaescrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;

import java.io.File;
import java.io.IOException;
import java.util.Stack;

/**
 * interface for classes that do work on files, implements runnable: to be done on seperate thread
 */

public abstract class FileOperator{
    protected File file;
    protected Bundle args;
    protected FileModifierService fileModifierService;
    private static Stack<FileOperator> waitingForYesNoResponse = new Stack<>();
    private static Stack<FileOperator> waitingForYesNoRememberAnswerResponse = new Stack<>();
    private static Stack<FileOperator> waitingForTextOrCancelResponse = new Stack<>();
    private Thread operationThread;
    private static final char[] ILLEGAL_CHARACTERS = { '/', '\n', '\r', '\t', '\0', '\f'};
    public FileOperator(File file, Bundle args, FileModifierService fileModifierService) {
        this.file = file;
        this.args = args;
        this.fileModifierService = fileModifierService;
    }

    public void run() {
        initMemVarFromArgs();
        prepareAndValidate();
        getInfoFromUser();
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

    protected abstract void initMemVarFromArgs();
    protected abstract void prepareAndValidate();
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

    protected boolean validFilename(String name) {
        if (name==null || name.getBytes().length>100 || name.length()==0 || name.equals("..") || name.equals(".")) {
            return false;
        }
        for (char c : ILLEGAL_CHARACTERS) {
            for (char name_c : name.toCharArray()) {
                if (c == name_c) {
                    return false;
                }
            }
        }
       File testFile = new File(fileModifierService.getApplicationInfo().dataDir+"/"+name);
        try {
            if (testFile.createNewFile()) {
                testFile.delete();
            }
        } catch (IOException ioe) {
            return false;
        }
        return true;
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
