package com.dewdrop623.androidcrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidcrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidcrypt.FileOperations.FileUtils;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidcrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidcrypt.R;

import java.io.File;
import java.util.ArrayList;

/**
 * renames folders
 */

public class FolderMoveOperator extends FileOperator {

    private String destinationName;
    private boolean validFilename;
    private ArrayList<File> toBeMoved = new ArrayList<>();
    private ArrayList<File> dontOverwrite = new ArrayList<>();
    private int dontOverwriteIndex = 0;
    private File destination;
    private int sourceParentDirectoryCharLength = 0;

    public FolderMoveOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    public String getOperationName() {
        return fileModifierService.getString(R.string.moving)+" "+file.getName();
    }

    @Override
    protected void initMemVarFromArgs() {
        destinationName = args.getString(FileMoveOperator.FILE_NEW_NAME_ARG, file.getName());
        destination = new File(args.getString(FileMoveOperator.FILE_MOVE_DESTINATION_ARG)+"/"+destinationName);
        sourceParentDirectoryCharLength = file.getAbsolutePath().length();
    }

    @Override
    protected void handleYesNoResponse(boolean yes) {

    }

    @Override
    protected void handleYesNoRememberAnswerResponse(boolean yes, boolean remember) {
        if (yes) {
            dontOverwrite.remove(dontOverwriteIndex);
        } else {
            dontOverwriteIndex++;
        }
        if (remember) {
            if (yes) {
                dontOverwrite = (ArrayList<File>) dontOverwrite.subList(0, dontOverwriteIndex);
            } else {
                dontOverwriteIndex = dontOverwrite.size();
            }
        }
        getInfoFromUser();
    }

    @Override
    protected void handleTextOrCancelResponse(String response) {
        if (response == null) {
            cancelOperation();
            return;
        }
        validFilename = FileUtils.validFilename(response, fileModifierService);
        if (validFilename) {
            destination = new File(args.getString(FileMoveOperator.FILE_MOVE_DESTINATION_ARG)+"/"+response);
            dontOverwrite = FileUtils.conflictsList(toBeMoved, destination, sourceParentDirectoryCharLength);
        }
        getInfoFromUser();
    }

    @Override
    public void doOperation() {
       FileUtils.moveOrCopyFolder(toBeMoved, destination, dontOverwrite, FileMoveOperator.FILE_MOVE_DESTINATION_ARG, FileMoveOperator.class, sourceParentDirectoryCharLength, fileModifierService);

        //cleanup
        for (int i = toBeMoved.size()-1; i>= 0; i--)  {
            if (toBeMoved.get(i).listFiles().length==0) {
                toBeMoved.get(i).delete();
            }
        }
        fileModifierService.updateNotification(100);
    }

    @Override
    protected boolean prepareAndValidate() {
        if (!FileUtils.folderMoveAndCopyValidationAndErrorToasts(file, destination.getParentFile(), fileModifierService)) {
            return false;
        }
        validFilename = FileUtils.validFilename(destinationName, fileModifierService);
        toBeMoved = FileUtils.createListWithSubdirectories(file);
        dontOverwrite = FileUtils.conflictsList(toBeMoved, destination, sourceParentDirectoryCharLength);
        return true;
    }

    @Override
    protected void getInfoFromUser() {
        if (!validFilename) {
            askForTextOrCancel(fileModifierService.getString(R.string.invalid_filename)+". "+fileModifierService.getString(R.string.try_again)+"?");
        }
        else if (dontOverwriteIndex<dontOverwrite.size()) {
            askYesNoRememberAnswer("Overwrite " + dontOverwrite.get(dontOverwriteIndex).getName() + "?", dontOverwrite.size() - dontOverwriteIndex, "conflict");
        } else {
            finishTakingInput();
        }
    }
}
