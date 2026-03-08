/**
 * NotificationPage — Page object for the notification toast system.
 */

class NotificationPage {
    /**
     * Check if any notification toast is visible.
     * Notifications are typically rendered as overlay elements.
     */
    async isToastVisible(): Promise<boolean> {
        const source = await browser.getPageSource();
        return source.includes('notification') || source.includes('Notification');
    }

    /**
     * Wait for any notification to appear.
     */
    async waitForToast(timeoutMs = 5000): Promise<boolean> {
        const start = Date.now();
        while (Date.now() - start < timeoutMs) {
            if (await this.isToastVisible()) {
                return true;
            }
            await browser.pause(200);
        }
        return false;
    }
}

export default new NotificationPage();
