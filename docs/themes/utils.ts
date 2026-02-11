import { EditorGeneralColors, EditorTheme, JavaScriptColors, JsonColors, LanguageDefaults, PythonColors, SqlColors, TypeScriptColors } from '@/types/index';

type SimplePalette = {
    background: string;
    caret: string;
    comment: string;
    gutter: string;
    keyword: string;
    line: string;
    number: string;
    selection: string;
    selectionText: string;
    string: string;
    text: string;
};

export const createDefaultEditorTheme = (palette: SimplePalette): EditorTheme => {
    const general: EditorGeneralColors = {
        background: palette.background,
        caret: palette.caret,
        caretRow: palette.selection, // Using selection color for caret row for now, or maybe a lighter version
        foldedText: palette.comment,
        gutterIcon: palette.line,
        indentGuide: palette.line,
        lineNumber: palette.line,
        lineNumberActive: palette.text,
        selectionBackground: palette.selection,
        selectionText: palette.selectionText,
        text: palette.text,
        whitespace: palette.line,
    };

    const languageDefaults: LanguageDefaults = {
        annotation: palette.keyword,
        badCharacter: '#ff0000',
        boolean: palette.keyword,
        braces: palette.text,
        brackets: palette.text,
        comma: palette.text,
        commentBlock: palette.comment,
        commentDoc: palette.comment,
        commentLine: palette.comment,
        commentTag: palette.comment,
        identifier: palette.text,
        invalidEscape: '#ff0000',
        keyword: palette.keyword,
        null: palette.keyword,
        number: palette.number,
        operator: palette.text,
        parentheses: palette.text,
        semicolon: palette.text,
        string: palette.string,
    };

    const json: JsonColors = {
        badCharacter: '#ff0000',
        boolean: palette.keyword,
        braces: palette.text,
        brackets: palette.text,
        colon: palette.text,
        comma: palette.text,
        null: palette.keyword,
        number: palette.number,
        propertyKey: palette.keyword,
        string: palette.string,
    };

    const javascript: JavaScriptColors = {
        boolean: palette.keyword,
        class: palette.text,
        commentBlock: palette.comment,
        commentDoc: palette.comment,
        commentLine: palette.comment,
        constant: palette.text,
        decorator: palette.keyword,
        functionCall: palette.text,
        functionDeclaration: palette.text,
        interface: palette.text,
        jsxAttribute: palette.text,
        jsxTag: palette.keyword,
        jsxText: palette.text,
        keyword: palette.keyword,
        methodCall: palette.text,
        methodDeclaration: palette.text,
        null: palette.keyword,
        number: palette.number,
        parameter: palette.text,
        property: palette.text,
        regexp: palette.string,
        string: palette.string,
        templateExpression: palette.text,
        templateString: palette.string,
        variable: palette.text,
    };

    const typescript: TypeScriptColors = {
        ...javascript,
        enum: palette.text,
        enumMember: palette.text,
        interface: palette.text,
        readonlyProperty: palette.text,
        tupleType: palette.text,
        typeAlias: palette.text,
        typeParameter: palette.text,
        typeReference: palette.text,
        unionType: palette.text,
    };

    const python: PythonColors = {
        boolean: palette.keyword,
        builtinFunction: palette.text,
        builtinVariable: palette.text,
        class: palette.text,
        commentLine: palette.comment,
        decorator: palette.keyword,
        docstring: palette.string,
        docTag: palette.comment,
        fStringExpression: palette.text,
        fStringText: palette.string,
        function: palette.text,
        keyword: palette.keyword,
        magicMethod: palette.keyword,
        method: palette.text,
        none: palette.keyword,
        number: palette.number,
        parameter: palette.text,
        string: palette.string,
        variable: palette.text,
    };

    const sql: SqlColors = {
        alias: palette.text,
        column: palette.text,
        commentBlock: palette.comment,
        commentLine: palette.comment,
        constraint: palette.text,
        datatype: palette.keyword,
        function: palette.text,
        index: palette.text,
        keyword: palette.keyword,
        number: palette.number,
        operator: palette.text,
        parameter: palette.text,
        schema: palette.text,
        string: palette.string,
        table: palette.text,
    };

    return {
        general,
        languageDefaults,
        languages: {
            javascript,
            json,
            python,
            sql,
            typescript,
        },
    };
};

import React from 'react';

import { Theme, TypographyStyle } from '@/types/index';

// ... (keep getTypographyVariables)

export const getMotionVariables = (theme: Theme): React.CSSProperties => {
    const variables: React.CSSProperties = {};
    if (!theme.motion) return variables;

    // Durations
    Object.entries(theme.motion.duration).forEach(([key, value]) => {
        const kebabKey = key.replace(/([A-Z])/g, '-$1').toLowerCase();
        (variables as Record<string, number | string>)[`--os-duration-${kebabKey}`] = value;
    });

    // Easings
    Object.entries(theme.motion.easing).forEach(([key, value]) => {
        const kebabKey = key.replace(/([A-Z])/g, '-$1').toLowerCase();
        (variables as Record<string, number | string>)[`--os-easing-${kebabKey}`] = value;
    });

    return variables;
};

export const getTypographyVariables = (theme: Theme): React.CSSProperties => {
    const variables: React.CSSProperties = {};
    const typographyKeys = [
        'displayLarge', 'displayMedium', 'displaySmall',
        'headlineLarge', 'headlineMedium', 'headlineSmall',
        'titleLarge', 'titleMedium', 'titleSmall',
        'bodyLarge', 'bodyMedium', 'bodySmall',
        'labelLarge', 'labelMedium', 'labelSmall'
    ] as const;

    typographyKeys.forEach(key => {
        const style = theme.typography[key] as TypographyStyle | undefined;
        if (style) {
            // Helper to convert key "displayLarge" -> "display-large"
            const kebabKey = key.replace(/([A-Z])/g, '-$1').toLowerCase();
            (variables as Record<string, number | string>)[`--os-type-${kebabKey}-size`] = style.size;
            (variables as Record<string, number | string>)[`--os-type-${kebabKey}-line-height`] = style.lineHeight;
            (variables as Record<string, number | string>)[`--os-type-${kebabKey}-weight`] = style.weight;
            (variables as Record<string, number | string>)[`--os-type-${kebabKey}-tracking`] = style.tracking;
            // Also simpler shorthand for generic usage? optional.
        }
    });

    return variables;
};

export const toOpaque = (color: string): string => {
    if (color.startsWith('rgba')) {
        return color.replace(/[\d.]+\)$/, '1)');
    }
    if (color.startsWith('#') && color.length === 9) {
        return color.slice(0, 7); // Strip alpha from hex
    }
    return color;
};
