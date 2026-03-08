/**
 * PreviewPage — Page object for the markdown preview panel.
 *
 * Selector: ~Rendered preview
 * Only available when in Split or VIEW mode with a workspace open.
 */

class PreviewPage {
    get container() {
        return $('~Rendered preview');
    }

    async waitForPreview(timeoutMs = 10000): Promise<void> {
        await this.container.waitForExist({ timeout: timeoutMs });
    }

    async isPreviewVisible(): Promise<boolean> {
        try {
            return await this.container.isExisting();
        } catch {
            return false;
        }
    }

    async getSize(): Promise<{ width: number; height: number }> {
        const size = await this.container.getSize();
        return { width: size.width, height: size.height };
    }

    /**
     * Check that the preview panel has reasonable minimum dimensions.
     */
    async hasMinimumSize(minWidth = 200, minHeight = 200): Promise<boolean> {
        const size = await this.getSize();
        return size.width >= minWidth && size.height >= minHeight;
    }
}

export default new PreviewPage();
