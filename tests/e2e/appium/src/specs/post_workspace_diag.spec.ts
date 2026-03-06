/**
 * post_workspace_diag.spec.ts
 *
 * Opens a workspace (triggering showEditor → refresh_accessibility_tree)
 * and then checks whether child accessibility identifiers are visible.
 */
import { openFixtureWorkspace } from '../support/folder_open';

describe('Post-Workspace Accessibility Diagnostic', () => {
    before(async () => {
        // Open a workspace to trigger showEditor() → refresh_accessibility_tree()
        await openFixtureWorkspace('sample');
        await browser.pause(3000); // Let the AX tree settle
    });

    it('should find child ma.* identifiers after workspace is opened', async () => {
        const source = await browser.getPageSource();
        console.log(`Page source length after workspace open: ${source.length}`);

        // Search for all ma.* identifiers
        const maPattern = /identifier="(ma\.[^"]+)"/g;
        const found = new Set<string>();
        let match = maPattern.exec(source);
        while (match !== null) {
            found.add(match[1]);
            match = maPattern.exec(source);
        }

        console.log(`=== ma.* identifiers found after workspace open ===`);
        console.log(`Total unique: ${found.size}`);
        for (const id of found) {
            console.log(`  ✅ ${id}`);
        }

        // Check specifically for child identifiers
        const expected = [
            'ma.shell.main_frame',
            'ma.activitybar',
            'ma.editor.panel',
            'ma.filetree.ctrl',
            'ma.statusbar',
        ];

        for (const id of expected) {
            const inSource = source.includes(id);
            console.log(`  ${inSource ? '✅' : '❌'} ${id} → ${inSource ? 'FOUND' : 'NOT FOUND'}`);
        }

        // The main frame should always be there
        expect(source).toContain('ma.shell.main_frame');
    });

    it('should try finding each identifier via selector', async () => {
        const ids = [
            'ma.shell.main_frame',
            'ma.activitybar',
            'ma.editor.panel',
            'ma.filetree.ctrl',
            'ma.statusbar',
        ];

        for (const id of ids) {
            try {
                const el = await browser.$(`~${id}`);
                const exists = await el.isExisting();
                console.log(`  ${exists ? '✅' : '❌'} ~${id} → ${exists ? 'EXISTS' : 'NOT FOUND'}`);
            } catch {
                console.log(`  ❌ ~${id} → EXCEPTION`);
            }
        }
    });
});
