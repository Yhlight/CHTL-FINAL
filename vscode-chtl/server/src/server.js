const {
    createConnection,
    TextDocuments,
    ProposedFeatures,
    InitializeParams,
    DidChangeConfigurationNotification,
    CompletionItem,
    CompletionItemKind,
    TextDocumentPositionParams,
    TextDocumentSyncKind,
    InitializeResult
} = require('vscode-languageserver/node');

const {
    TextDocument
} = require('vscode-languageserver-textdocument');

// Create a connection for the server, using Node's IPC as a transport.
// Also include all preview / proposed LSP features.
let connection = createConnection(ProposedFeatures.all);

// Create a simple text document manager.
let documents = new TextDocuments(TextDocument);

let hasConfigurationCapability = false;
let hasWorkspaceFolderCapability = false;
let hasDiagnosticRelatedInformationCapability = false;

connection.onInitialize((params) => {
    let capabilities = params.capabilities;

    // Does the client support the `workspace/configuration` request?
    // If not, we fall back using global settings.
    hasConfigurationCapability = !!(
        capabilities.workspace && !!capabilities.workspace.configuration
    );
    hasWorkspaceFolderCapability = !!(
        capabilities.workspace && !!capabilities.workspace.workspaceFolders
    );
    hasDiagnosticRelatedInformationCapability = !!(
        capabilities.textDocument &&
        capabilities.textDocument.publishDiagnostics &&
        capabilities.textDocument.publishDiagnostics.relatedInformation
    );

    const result = {
        capabilities: {
            textDocumentSync: TextDocumentSyncKind.Incremental,
            // Tell the client that this server supports code completion.
            completionProvider: {
                resolveProvider: true
            }
        }
    };
    if (hasWorkspaceFolderCapability) {
        result.capabilities.workspace = {
            workspaceFolders: {
                supported: true
            }
        };
    }
    return result;
});

// The content of a text document has changed. This event is emitted
// when the text document first opened or when its content has changed.
documents.onDidChangeContent(change => {
    // Here you could validate the document and send diagnostics to VS Code.
});

// This handler provides the initial list of the completion items.
connection.onCompletion((_textDocumentPosition) => {
    // The pass parameter contains the position of the text document in
    // which code complete got requested. For the example we ignore this
    // info and always provide the same completion items.
    return [{
            label: 'text',
            kind: CompletionItemKind.Keyword,
            data: 1
        },
        {
            label: 'style',
            kind: CompletionItemKind.Keyword,
            data: 2
        },
        {
            label: 'script',
            kind: CompletionItemKind.Keyword,
            data: 3
        },
        {
            label: '[Template]',
            kind: CompletionItemKind.Keyword,
            data: 4
        },
        {
            label: '[Custom]',
            kind: CompletionItemKind.Keyword,
            data: 5
        }
    ];
});

// This handler resolves additional information for the item selected in
// the completion list.
connection.onCompletionResolve((item) => {
    if (item.data === 1) {
        item.detail = 'Text block';
        item.documentation = 'A block of text content.';
    } else if (item.data === 2) {
        item.detail = 'Style block';
        item.documentation = 'A block for CSS styles.';
    } else if (item.data === 3) {
        item.detail = 'Script block';
        item.documentation = 'A block for JavaScript code.';
    } else if (item.data === 4) {
        item.detail = 'Template definition';
        item.documentation = 'Defines a reusable CHTL template.';
    } else if (item.data === 5) {
        item.detail = 'Custom definition';
        item.documentation = 'Defines a customizable CHTL component.';
    }
    return item;
});

// Make the text document manager listen on the connection
// for open, change and close text document events
documents.listen(connection);

// Listen on the connection
connection.listen();