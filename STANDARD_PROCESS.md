# OpenFX Standard Update Process

Changes to the OpenFX Standard, that is, changes to the main header
files, including new suites, new properties, or incompatible
modifications to existing suites, must follow a carefully designed
process to ensure wide community adoption, compatibility and
extensibility.

The process is comprised of the following steps:

## Proposal

Use the
[Standard Change Proposal](https://github.com/ofxa/openfx/issues/new?assignees=&labels=standard+change&template=standard-change.md)
github issue template to file a Standard Change Proposal.
This template has a number of helpful prompts to guide you in creating
a useful proposal.

## Discussion

The proposal will be discussed by the TSC and the community. It is
very helpful if the proposer(s) can attend a TSC meeting to discuss
the proposal and how it can benefit the community.

## Implementation (by one plug-in and one host)

In order to make sure a standard change is implementable and useful,
before being approved the header file(s) must be written and a github
branch and PR created, and it must be implemented by at least one
plug-in and at least one host product and a demo most be shown to the
community, ideally presented at a TSC meeting.

The implementation does not have to be in a fully released version of
the two sides, but they must be using the exact header files in the
github PR. The plug-in and host should be from different source trees
or organizations, to better demonstrate the utility of the change for
the entire community.

## Documentation

All changes to the standard must include full documentation, including
compatibility implications, feature negotiation, and release notes.

## Approval

Once all the steps in the Standard Change Proposal are complete, the
TSC will vote to approve the change and then it can be merged into the main branch
for release.
