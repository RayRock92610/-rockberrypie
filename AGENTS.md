# Agent Directives & Operational Rules

## Build & Test Commands
- Install: `pip install -r requirements.txt` / `npm install`
- Test: `pytest -v` / `python3 -m unittest discover tests`
- Lint: `flake8 .` / `npm run lint`

## Architecture Invariants
- Code formatting: 4-space indentation, strict typing.
- Security: Never expose raw secrets or passwords in process arguments or logs.
- Git: Branch naming convention: `fix/<issue-id>-<description>`. Never squash migrations without review.
