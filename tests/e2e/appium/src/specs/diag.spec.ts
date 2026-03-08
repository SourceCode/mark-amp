/**
 * Deep diagnostic spec — searches full accessibility tree for ma.* identifiers.
 * Run with: npx wdio run wdio.conf.ts --spec src/specs/diag.spec.ts
 */
import * as fs from 'fs';
import * as path from 'path';

describe('Deep Accessibility Tree Diagnostic', () => {
    it('should dump full page source to file and search for ma.* identifiers', async () => {
        await browser.pause(3000); // wait for all deferred accessibility calls
        const source = await browser.getPageSource();

        // Write full XML to file for inspection
        const dumpPath = path.resolve(__dirname, '../../artifacts/ax_tree_dump.xml');
        fs.mkdirSync(path.dirname(dumpPath), { recursive: true });
        fs.writeFileSync(dumpPath, source, 'utf-8');
        console.log(`Full page source written to: ${dumpPath} (${source.length} chars)`);

        // Search for ALL occurrences of "ma." anywhere in attributes
        const allMaOccurrences = source.match(/ma\.[a-z._]+/g);
        console.log('=== ALL "ma.*" OCCURRENCES IN XML ===');
        if (allMaOccurrences) {
            const unique = [...new Set(allMaOccurrences)];
            console.log(`Found ${allMaOccurrences.length} total, ${unique.length} unique:`);
            unique.forEach((m: string) => console.log(`  ${m}`));
        } else {
            console.log('NO "ma.*" occurrences found anywhere in the XML!');
        }

        // Count total elements in the tree
        const elementCount = (source.match(/<XCUI/g) || []).length;
        console.log(`Total XCUIElementType elements in tree: ${elementCount}`);

        // Show what element types are present
        const typeMatches = source.match(/XCUIElementType\w+/g);
        if (typeMatches) {
            const typeCounts: Record<string, number> = {};
            typeMatches.forEach((t: string) => {
                typeCounts[t] = (typeCounts[t] || 0) + 1;
            });
            console.log('=== ELEMENT TYPE BREAKDOWN ===');
            Object.entries(typeCounts)
                .sort(([, a], [, b]) => b - a)
                .forEach(([type, count]) => {
                    console.log(`  ${type}: ${count}`);
                });
        }

        expect(source.length).toBeGreaterThan(0);
    });

    it('should try finding each ma.* identifier', async () => {
        const identifiers = [
            'ma.shell.main_frame',
            'ma.activitybar',
            'ma.editor.panel',
            'ma.filetree.ctrl',
            'ma.statusbar',
            'ma.commandpalette',
            'ma.settings.panel',
        ];

        console.log('=== IDENTIFIER LOOKUP ===');
        for (const id of identifiers) {
            try {
                const el = await browser.$(`~${id}`);
                const exists = await el.isExisting();
                if (exists) {
                    const label = await el.getAttribute('label');
                    const identifier = await el.getAttribute('identifier');
                    const elementType = await el.getAttribute('elementType');
                    console.log(`  ✅ ~${id} → label="${label}" identifier="${identifier}" elementType="${elementType}"`);
                } else {
                    console.log(`  ❌ ~${id} → not found`);
                }
            } catch {
                console.log(`  ❌ ~${id} → error`);
            }
        }
    });

    it('should search by predicate for any identifier containing "ma."', async () => {
        try {
            const els = await browser.$$(`-ios predicate string:identifier CONTAINS "ma."`);
            console.log(`\nFound ${els.length} elements with identifier containing "ma.":`);
            for (const el of els) {
                const identifier = await el.getAttribute('identifier');
                const label = await el.getAttribute('label');
                const elementType = await el.getAttribute('elementType');
                const frame = await el.getAttribute('frame');
                console.log(`  identifier="${identifier}" label="${label}" type="${elementType}" frame=${frame}`);
            }
        } catch (e) {
            console.log('Predicate search failed:', e instanceof Error ? e.message : String(e));
        }
    });
});
