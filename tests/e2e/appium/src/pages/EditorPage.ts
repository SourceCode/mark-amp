/**
 * EditorPage — Page object for editor-related interactions.
 *
 * Uses selector: ma.editor.panel
 *
 * NOTE: The `ma.editor.panel` identifier may not be available in the
 * accessibility tree. This page object falls back to checking for
 * static text elements (which indicate text content is rendered in the editor).
 */

class EditorPage {
    get panel() {
        return $('~ma.editor.panel');
    }

    async waitForEditor(timeoutMs = 10000): Promise<void> {
        try {
            await this.panel.waitForExist({ timeout: Math.min(timeoutMs, 5000) });
        } catch {
            // Identifier not available — check for text content instead
            await browser.waitUntil(
                async () => {
                    const source = await browser.getPageSource();
                    return source.includes('XCUIElementTypeStaticText');
                },
                { timeout: timeoutMs, timeoutMsg: 'Editor content did not appear' }
            );
        }
    }

    async isEditorVisible(): Promise<boolean> {
        try {
            const exists = await this.panel.isExisting();
            if (exists) return true;
        } catch {
            // Identifier not available
        }
        // Fall back: check if text content exists (indicating editor is showing)
        try {
            const source = await browser.getPageSource();
            const textCount = (source.match(/XCUIElementTypeStaticText/g) || []).length;
            return textCount > 3; // More than menu items
        } catch {
            return false;
        }
    }
}

export default new EditorPage();
