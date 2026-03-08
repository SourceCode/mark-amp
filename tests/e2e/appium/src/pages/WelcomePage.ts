/**
 * WelcomePage — Page object for the Welcome screen.
 *
 * The Welcome screen is shown when no workspace is open.
 * It displays version info, recent workspaces, and quick actions.
 */

import AppShellPage from './AppShellPage';

class WelcomePage {
    /**
     * Check if the Welcome screen is visible by looking for
     * "Workspaces" text or version string.
     */
    async isWelcomeVisible(): Promise<boolean> {
        const texts = await AppShellPage.getAllStaticTexts();
        return texts.some(t => t === 'Workspaces' || t.startsWith('v'));
    }

    /**
     * Get the version string from the Welcome screen.
     */
    async getVersion(): Promise<string | undefined> {
        const texts = await AppShellPage.getAllStaticTexts();
        return texts.find(t => t.startsWith('v'));
    }

    /**
     * Get all text labels on the Welcome screen.
     */
    async getAllLabels(): Promise<string[]> {
        return AppShellPage.getAllStaticTexts();
    }

    /**
     * Check if the "Workspaces" label is present.
     */
    async hasWorkspacesLabel(): Promise<boolean> {
        const texts = await AppShellPage.getAllStaticTexts();
        return texts.includes('Workspaces');
    }
}

export default new WelcomePage();
