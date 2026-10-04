## 2024-10-04 - Assisting user with resolving merge conflicts in PR #9 in Rockflow

**Learning:** When resolving Git conflicts across repositories where the workspace restricts `git push`, the safest approach to maintain boundaries is to perform the rebase and conflict resolution in a temporary isolated clone, then output the resolved files as heredoc blocks for the user to apply manually in their local environment.

**Action:** Before executing `git` commands that push changes, confirm the repository context and remote settings to avoid violating sandbox boundaries. Provide self-contained bash blocks to the user when they need to sync state in an environment like Termux.
