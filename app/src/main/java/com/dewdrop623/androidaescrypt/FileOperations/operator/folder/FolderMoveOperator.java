package com.dewdrop623.androidaescrypt.FileOperations.operator.folder;

import android.os.Bundle;

import com.dewdrop623.androidaescrypt.FileOperations.FileModifierService;
import com.dewdrop623.androidaescrypt.FileOperations.FileUtils;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileMoveOperator;
import com.dewdrop623.androidaescrypt.FileOperations.operator.FileOperator;
import com.dewdrop623.androidaescrypt.R;

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
        destination = new File(args.getString(FileMoveOperator.FILE_MOVE_DESTINATION_ARG));
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
        int filesToBeMoved = FileUtils.countFilesInFolder(toBeMoved.get(0))-dontOverwrite.size();
        int filesMoved = 0;
        fileModifierService.updateNotification(0);
        Bundle args = new Bundle();
        for (File folder : toBeMoved) {
            File newFolder = renameToDesination(folder);
            newFolder.mkdir();
            for (File file : folder.listFiles()) {
                if (!file.isDirectory() && !dontOverwrite.contains(file)) {
                    args.putString(FileMoveOperator.FILE_MOVE_DESTINATION_ARG, newFolder.getAbsolutePath()+"/"+file.getName());
                    new FileMoveOperator(file, args, fileModifierService).doOperationWithoutThreadOrUserQuestions();
                    filesMoved++;
                    fileModifierService.updateNotification((filesMoved*100)/filesToBeMoved);
                }
            }
        }
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
        if (!file.exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+file.getName());
            cancelOperation();
            return;
        }
        if (!destination.getParentFile().exists()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.could_not_find_directory)+" "+destination.getName());
            cancelOperation();
            return;
        }
        if (!file.canRead()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_readable)+": "+file.getName());
            cancelOperation();
            return;
        }
        if (!destination.getParentFile().canWrite()) {
            fileModifierService.showToast(fileModifierService.getString(R.string.directory_not_readable)+": "+destination.getName());
            cancelOperation();
            return;
        }
        toBeMoved.add(file);
        addSubdirectoriesOfPositionToList(0);
        for(File folderToBeCopied : toBeMoved) {
            for (File file : folderToBeCopied.listFiles()) {
                File newFile = renameToDesination(file);
                if (!newFile.isDirectory() && newFile.exists()) {
                    dontOverwrite.add(file);
                }
            }
        }
    }

    @Override
    protected void getInfoFromUser() {
        if (dontOverwriteIndex<dontOverwrite.size()) {
            askYesNoRememberAnswer("Overwrite " + dontOverwrite.get(dontOverwriteIndex).getName() + "?", dontOverwrite.size() - dontOverwriteIndex, "conflict");
        } else {
            finishTakingInput();
        }
    }
    private void addSubdirectoriesOfPositionToList(int position) {
        if(toBeMoved.size()<=position) {
            return;
        }
        for(File file : toBeMoved.get(position).listFiles()) {
            if (file.isDirectory()) {
                toBeMoved.add(file);
            }
        }
        addSubdirectoriesOfPositionToList(position+1);
    }
    private File renameToDesination(File file) {
        File newFile = new File(destination.getAbsolutePath()+file.getAbsolutePath().substring(sourceParentDirectoryCharLength));
        return newFile;
    }
}
