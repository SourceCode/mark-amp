/**
 * SettingsPage — Page object for settings panel interactions.
 *
 * Uses selector: ma.settings.panel
 */

class SettingsPage {
    get panel() {
        return $('~ma.settings.panel');
    }

    async waitForSettings(timeoutMs = 10000): Promise<void> {
        await this.panel.waitForExist({ timeout: timeoutMs });
    }

    async isSettingsVisible(): Promise<boolean> {
        return this.panel.isExisting();
    }
}

export default new SettingsPage();
