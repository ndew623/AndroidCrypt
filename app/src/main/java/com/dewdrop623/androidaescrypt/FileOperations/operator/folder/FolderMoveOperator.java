package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;

import java.io.File;
import java.util.ArrayList;

/**
 * renames folders
 */

public class FolderMoveOperator extends FileOperator {

    private ArrayList<File> toBeMoved = new ArrayList<>();
    private ArrayList<File> dontOverwrite = new ArrayList<>();
    private int dontOverwriteIndex = 0;
    private File destination;
    private int sourceParentDirectoryCharLength = 0;

    public FolderMoveOperator(File file, Bundle args, FileModifierService fileModifierService) {
        super(file, args, fileModifierService);
    }

    @Override
    protected void initMemVarFromArgs() {
        destination = new File(args.getString(FileMoveOperator.FILE_MOVE_DESTINATION_ARG)+"/"+args.getString(FileMoveOperator.FILE_NEW_NAME_ARG, file.getName()));
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
    protected void prepareAndValidate() {
        if (!FileUtils.folderMoveAndCopyValidationAndErrorToasts(file, destination.getParentFile(), fileModifierService)) {
            cancelOperation();
        }
        toBeMoved = FileUtils.createListWithSubdirectories(file);
        dontOverwrite = FileUtils.conflictsList(toBeMoved, destination, sourceParentDirectoryCharLength);
    }

    @Override
    protected void getInfoFromUser() {
        if (dontOverwriteIndex<dontOverwrite.size()) {
            askYesNoRememberAnswer("Overwrite " + dontOverwrite.get(dontOverwriteIndex).getName() + "?", dontOverwrite.size() - dontOverwriteIndex, "conflict");
        } else {
            finishTakingInput();
        }
    }
}
