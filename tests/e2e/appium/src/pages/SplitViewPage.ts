/**
 * SplitViewPage — Page object for the SplitView component.
 *
 * Named elements:
 *   - "Split view divider"
 *   - "Markdown editor"
 *   - "Rendered preview"
 */

class SplitViewPage {
    get divider()  { return $('~Split view divider'); }
    get editor()   { return $('~Markdown editor'); }
    get preview()  { return $('~Rendered preview'); }

    async waitForSplitView(timeoutMs = 10000): Promise<void> {
        await this.editor.waitForExist({ timeout: timeoutMs });
    }

    async isSplitMode(): Promise<boolean> {
        const editorExists = await this.editor.isExisting();
        const previewExists = await this.preview.isExisting();
        return editorExists && previewExists;
    }

    async isEditorOnlyMode(): Promise<boolean> {
        const editorExists = await this.editor.isExisting();
        const previewExists = await this.preview.isExisting();
        return editorExists && !previewExists;
    }

    async isPreviewOnlyMode(): Promise<boolean> {
        const editorExists = await this.editor.isExisting();
        const previewExists = await this.preview.isExisting();
        return !editorExists && previewExists;
    }

    async isDividerVisible(): Promise<boolean> {
        return this.divider.isExisting();
    }

    async getEditorSize(): Promise<{ width: number; height: number }> {
        const size = await this.editor.getSize();
        return { width: size.width, height: size.height };
    }

    async getPreviewSize(): Promise<{ width: number; height: number }> {
        const size = await this.preview.getSize();
        return { width: size.width, height: size.height };
    }
}

export default new SplitViewPage();
