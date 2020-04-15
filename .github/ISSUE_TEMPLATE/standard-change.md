---
name: Standard Change Proposal
about: Use this template to propose a change to the OFX Standard
labels: standard change
---
# Open Effects Proposal for Standard Change

Please read the contribution [guidelines](https://github.com/ofxa/openfx/wiki/Extending-OpenFX-Guidelines#submit-a-proposal-or-bug-report-to-review) first.

## Standard Change Workflow
- [ ] Create proposal as issue (you're doing this now!)
- [ ] Tag this issue with `standard change` tag
- [ ] Identify subcommittee: at least one plug-in vendor, and at least one host
- [ ] Discuss the idea in this issue
- [ ] Write new or updated code and doc
- [ ] Publish updates as a pull request (ideally on a `feature/PROPOSAL-NAME` branch)
   - [ ] Make sure that PR references this issue number to keep them in sync
   - [ ] Discuss and review code in the PR
   - [ ] Meet all requirements below for accepting PR
- [ ] When subcommittee signs off and other members don't have any further review comments, 
      maintainer merges PR to master which closes PR and issue

## Requirements for accepting a standard change:
- [ ] Header files updated
- [ ] Documentation updated
- [ ] Release notes added
- [ ] Compatibility review completed
- [ ] Working code demonstrated with at least one host and one plugin
- [ ] At least two members sign off
- [ ] No further changes requested from membership

## Summary

Describe the change succintly in one or two sentences.

## Motivation

Why is this change proposed?

## Problem

What problem does this proposed change solve? Be specific, concise, and clear.

## Impact

Is this a new feature (no compatibility impact), a change to an existing function suite (version
the suite to avoid compatibility issues), a change to an existing property, or a documentation
change?

How will hosts and plugins negotiate use of this change? Show how it works when a host implements 
it but not plugin and vice versa.

## Documentation Impact

What changes to the docs are needed for this change?

## Stakeholders

Who will benefit from this proposed change? Plug-ins, hosts, or both? Specific types of hosts?

## Discussion
