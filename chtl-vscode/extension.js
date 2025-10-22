const vscode = require('vscode');
const cp = require('child_process');
const path = require('path');

function activate(context) {
    let compileCommand = vscode.commands.registerCommand('chtl.compile', () => {
        const editor = vscode.window.activeTextEditor;
        if (editor) {
            const filePath = editor.document.fileName;
            const workspaceFolder = vscode.workspace.getWorkspaceFolder(editor.document.uri);
            if (workspaceFolder) {
                const rootPath = workspaceFolder.uri.fsPath;
                const buildScriptPath = path.join(rootPath, 'build.py');
                const command = `python ${buildScriptPath} --file ${filePath}`;

                cp.exec(command, { cwd: rootPath }, (err, stdout, stderr) => {
                    if (err) {
                        console.error(err);
                        vscode.window.showErrorMessage('CHTL compilation failed.');
                        return;
                    }
                    vscode.window.showInformationMessage('CHTL compiled successfully.');
                    console.log(stdout);
                });
            }
        }
    });

    let previewCommand = vscode.commands.registerCommand('chtl.preview', () => {
        vscode.window.showInformationMessage('CHTL preview is not yet implemented.');
    });

    context.subscriptions.push(compileCommand);
    context.subscriptions.push(previewCommand);
}

function deactivate() {}

module.exports = {
    activate,
    deactivate
};
