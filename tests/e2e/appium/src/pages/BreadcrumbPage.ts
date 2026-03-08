/**
 * BreadcrumbPage — Page object for the breadcrumb navigation bar.
 *
 * The breadcrumb shows the file path segments above the editor.
 */

class BreadcrumbPage {
    /**
     * Check if breadcrumb content is present in the accessibility tree.
     */
    async isVisible(): Promise<boolean> {
        const source = await browser.getPageSource();
        return source.includes('breadcrumb') || source.includes('Breadcrumb');
    }

    /**
     * Get the breadcrumb text (file path) from the accessibility tree.
     */
    async getBreadcrumbTexts(): Promise<string[]> {
        const elements = await browser.$$(
            '-ios predicate string:elementType == 48'
        );
        const texts: string[] = [];
        for (const el of elements) {
            const value = await el.getAttribute('value');
            if (value && value.includes('/')) {
                texts.push(value);
            }
        }
        return texts;
    }
}

export default new BreadcrumbPage();
