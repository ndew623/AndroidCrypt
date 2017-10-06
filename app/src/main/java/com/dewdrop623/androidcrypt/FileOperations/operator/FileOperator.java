package com.dewdrop623.androidcrypt.FileOperations.operator;

import android.os.Bundle;

import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;

import java.io.File;
import java.util.Stack;

/**
 * FileOperator is the common interface for classes that define the work that is done when doing an operation on the FileSystem.
 * FileOperator:
 * -has references to the common member variables of all FileOperations (e.g. file and args)
 * -defines as abstract methods the common parts of a FileOperation that need to be implemented
 * -creates and ends a background thread
 * -creates a common flow of logic with its public final run() method
 * -provides the global methods and collections for FileOperators to interact with the user via UI
 */

public abstract class FileOperator{

    //the file being operated on
    protected File file;

    //the arguments to control the operation
    protected Bundle args;

    /*silent is used when a FileOperator like FolderCopyOperator uses another operator (like FileCopyOperator)
    in its work to keep the second operator from showing updates/messages*/
    protected boolean silent = false;

    //the service that is the FileOperators context
    protected FileModifierService fileModifierService;

    //these stacks allow FileOperators to ask the user questions and wait for a certain type of response
    private static Stack<FileOperator> waitingForYesNoResponse = new Stack<>();
    private static Stack<FileOperator> waitingForYesNoRememberAnswerResponse = new Stack<>();
    private static Stack<FileOperator> waitingForTextOrCancelResponse = new Stack<>();

    //the background thread for the operation
    private Thread operationThread;

    /////////////////////
    //PUBLIC METHODS
    /////////////////////

    public FileOperator(File file, Bundle args, FileModifierService fileModifierService) {
        this.file = file;
        this.args = args;
        this.fileModifierService = fileModifierService;
    }

    //overrode by implementing classes so that a notification/dialog can display what type of operation is happening
    public abstract String getOperationName();

    //call this to start the file operation normally
    public final void run() {
        initMemVarFromArgs();
        if (prepareAndValidate()) {
            getInfoFromUser();
        } else {
            cancelOperation();
        }
    }

    //stop the operation now. interrupt its thread, stop waiting for user questions, and stop the service
    public final void cancelOperation() {
        if (operationThread != null) {
            operationThread.interrupt();
            operationThread = null;
        }
        waitingForYesNoResponse.remove(this);
        waitingForYesNoRememberAnswerResponse.remove(this);
        waitingForTextOrCancelResponse.remove(this);
        fileModifierService.stopSelf();
    }

    //run without a thread, errors, validation/questions, or progress updates. the implementing class must define this method further to decide how exactly to do that.
    public void runSilentNoThread() {
        //optional override
        silent = true;
    }

    /*public static void userResponse is called by the Question dialogs.
    * The last FileOperation to ask for that type of response (the one at the top of its respective stack)
     * gets the answer.
    * */
    public static void userResponse(boolean yes) {
        if (!waitingForYesNoResponse.isEmpty()) {
            waitingForYesNoResponse.pop().handleYesNoResponse(yes);
        }
    }
    public static void userResponse(boolean yes, boolean remember) {
        if (!waitingForYesNoRememberAnswerResponse.isEmpty()) {
            waitingForYesNoRememberAnswerResponse.pop().handleYesNoRememberAnswerResponse(yes, remember);
        }
    }
    public static void userResponse(String response) {
        if (!waitingForTextOrCancelResponse.isEmpty()) {
            waitingForTextOrCancelResponse.pop().handleTextOrCancelResponse(response);
        }
    }

    /////////////////////
    //PROTECTED METHODS
    /////////////////////

    //initialize relevant member variable from the Bundle args
    protected abstract void initMemVarFromArgs();

    //prepare to do the operation and do any necessary validation checks
    protected abstract boolean prepareAndValidate();

    //if any validation checks (e.g. invalid filename) can be resolved, ask the user questions
    protected abstract void getInfoFromUser();

    //do the file operation
    protected abstract void doOperation();

    //called when the user answers a yes no question
    protected abstract void handleYesNoResponse(boolean yes);

    //called when the user answers a yes no question with a checkbox to remember the answer
    protected abstract void handleYesNoRememberAnswerResponse(boolean yes, boolean remember);

    //called when the user answers a question with text input
    protected abstract void handleTextOrCancelResponse(String response);

    //validation checks are done and we can stop waiting for the user to fix problems, start the thread and do the operation
    protected final void finishTakingInput() {
        operationThread = new Thread(new Runnable() {
            @Override
            public void run() {
                doOperation();
                fileModifierService.stopSelf();
            }
        });
        operationThread.start();
    }

    //display a dialog to user asking yes or no
    protected final void askYesNo(String question) {
        waitingForYesNoResponse.add(this);
        fileModifierService.askYesNo(question);
    }

    //display a dialog to the user asking yes or no and whether to remember the answer
    protected final void askYesNoRememberAnswer(String question, int numberOfEvents, String typeOfEventString) {
        waitingForYesNoRememberAnswerResponse.add(this);
        fileModifierService.askYesNoRememberAnswer(question, numberOfEvents, typeOfEventString);
    }

    //display a dialog to the user asking for a text response
    protected final void askForTextOrCancel(String question) { //if response null then cancel
        waitingForTextOrCancelResponse.add(this);
        fileModifierService.askForTextOrCancel(question);
    }
}
