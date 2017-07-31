package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileCopyOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;
import java.util.ArrayList;

/**
 * recursively copies a directories contents
 */

public class FolderCopyOperator extends FileOperator{

    private ArrayList<File> toBeCopied = new ArrayList<>();
    private ArrayList<File> dontOverwrite = new ArrayList<>();
    private int dontOverwriteIndex = 0;
    private File destination;
    private int sourceParentDirectoryCharLength = 0;

    public FolderCopyOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    protected void initMemVarFromArgs() {
        destination = new File(args.getString(FileCopyOperator.FILE_COPY_DESTINATION_ARG));
        sourceParentDirectoryCharLength = file.getParent().length();
    }

    @Override
    public void doOperation() {
        FileUtils.moveOrCopyFolder(toBeCopied, destination, dontOverwrite, FileCopyOperator.FILE_COPY_DESTINATION_ARG, FileCopyOperator.class, sourceParentDirectoryCharLength, fileModifierService);
        fileModifierService.updateNotification(100);
    }

    @Override
    protected boolean prepareAndValidate() {
        if (!FileUtils.folderMoveAndCopyValidationAndErrorToasts(file, destination, fileModifierService))
        {
            return false;
        }
        toBeCopied = FileUtils.createListWithSubdirectories(file);
        dontOverwrite = FileUtils.conflictsList(toBeCopied, destination, sourceParentDirectoryCharLength);
        return true;
    }

    @Override
    protected void getInfoFromUser() {
        if (dontOverwriteIndex<dontOverwrite.size()) {
            askYesNoRememberAnswer("Overwrite " + dontOverwrite.get(dontOverwriteIndex).getName() + "?", dontOverwrite.size() - dontOverwriteIndex, "conflict");
        } else {
            finishTakingInput();
        }
    }
    @Override
    protected void handleYesNoResponse(boolean yes) {

    }

    @Override
    protected void handleTextOrCancelResponse(String response) {

    }

    @Override
    public void handleYesNoRememberAnswerResponse(boolean yes, boolean remember) {
        if (yes) {
            dontOverwrite.remove(dontOverwriteIndex);
        } else {
            dontOverwriteIndex++;
        }
        if (remember) {
            if (yes) {
                dontOverwrite = new ArrayList<>(dontOverwrite.subList(0, dontOverwriteIndex));
            } else {
                dontOverwriteIndex = dontOverwrite.size();
            }
        }
        getInfoFromUser();
    }
}
