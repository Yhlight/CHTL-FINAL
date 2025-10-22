const { workspace, ExtensionContext } = require('vscode');
const { LanguageClient, LanguageClientOptions, ServerOptions, TransportKind } = require('vscode-languageclient/node');
const path = require('path');

let client;

function activate(context) {
    // The server is implemented in C++
    const serverExecutable = context.asAbsolutePath(path.join('..', '..', 'CHTL', 'build', 'chtl-ls'));

    // Server options tell the client how to start the server
    const serverOptions = {
        command: serverExecutable,
        transport: TransportKind.stdio
    };

    // Client options define the language features the client supports
    const clientOptions = {
        documentSelector: [{ scheme: 'file', language: 'chtl' }],
        synchronize: {
            // Notify the server about file changes to '.clientrc files contained in the workspace
            fileEvents: workspace.createFileSystemWatcher('**/.clientrc')
        }
    };

    // Create the language client and start the client.
    client = new LanguageClient(
        'chtlLanguageServer',
        'CHTL Language Server',
        serverOptions,
        clientOptions
    );

    // Start the client. This will also launch the server
    client.start();
}

function deactivate() {
    if (!client) {
        return undefined;
    }
    return client.stop();
}

module.exports = {
    activate,
    deactivate
};
