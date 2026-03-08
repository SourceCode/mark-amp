/**
 * SourceControlPage — Page object for the SourceControlPanel accessibility elements.
 *
 * Covers 7 named elements: Branch Selector, Commit Message Input,
 * Template Button, Commit Button, Staged/Unstaged/Merge/Timeline Lists.
 */

class SourceControlPage {
    get branchSelector()    { return $('~Branch Selector'); }
    get commitInput()       { return $('~Commit Message Input'); }
    get templateButton()    { return $('~Template Button'); }
    get commitButton()      { return $('~Commit Button'); }
    get stagedList()        { return $('~Staged Changes List'); }
    get unstagedList()      { return $('~Unstaged Changes List'); }
    get mergeList()         { return $('~Merge Conflicts List'); }
    get timelineList()      { return $('~Timeline List'); }

    async waitForPanel(timeoutMs = 10000): Promise<void> {
        await this.branchSelector.waitForExist({ timeout: timeoutMs });
    }

    async isPanelLoaded(): Promise<boolean> {
        const branch = await this.branchSelector.isExisting();
        const commit = await this.commitButton.isExisting();
        return branch && commit;
    }

    async getCommitInputText(): Promise<string> {
        return this.commitInput.getValue();
    }

    async isCommitButtonEnabled(): Promise<boolean> {
        return this.commitButton.isEnabled();
    }
}

export default new SourceControlPage();
