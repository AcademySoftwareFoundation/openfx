<!-- SPDX-License-Identifier: CC-BY-4.0 -->
<!-- Copyright OpenFX and contributors to the OpenFX project. -->
# Contributing to OpenFX

Thank you for your interest in contributing to OpenFX. This document
explains our contribution process and procedures.

For a description of the roles and responsibilities of the various
members of the OpenFX community, see [GOVERNANCE](GOVERNANCE.md), and
for further details, see the project's [Technical
Charter](ASWF/charter/OpenFX-Technical-Charter.md). Briefly,
Contributors are anyone who submits content to the project, Committers
review and approve such submissions, and the Technical Steering
Committee provides general project oversight.

## Getting Information

There are several ways to connect with the OpenFX project:

* The [openfx-discussion](https://lists.aswf.io/g/openfx-discussion)
  mail list: This is our primary mailing list, intended for technical
  discussion and help implementing and using OpenFX.

* [GitHub Issues](https://github.com/ofxa/openfx/issues): GitHub
  Issues are used both to track bugs and to discuss feature requests.

* [Slack channel](https://slack.aswf.io/)

### How to Ask for Help

The best places to start if you have questions about implementing,
using, or extending OpenFX are the mailing list and our Slack.

### How to Report a Bug

OpenFX use GitHub's issue tracking system for bugs and enhancements:
https://github.com/ofxa/openfx/issues

If you are submitting a bug report, please be sure to note which
plug-in(s) and host products are involved, what OS and compilers
you're using, aany special build flags or other unusual environmental
issues). Please give a specific account of

* what you tried
* what happened
* what you expected to happen instead

with enough detail that others can reproduce the problem.

### How to Request a Standard Change

Open a GitHub issue: https://github.com/ofxa/openfx/issues.

Describe the goal and the proposed feature in as much detail as
possible. Standard change requests will almost certainly spawn a
discussion among the project community.

### How to Report a Security Vulnerability

If you think you've found a potential vulnerability in OpenFX, please
refer to [SECURITY.md] to responsibly disclose it.

### How to Contribute a Bug Fix or Change

To contribute code to the project, first read over the
[GOVERNANCE](GOVERNANCE.md) page to understand the roles involved.
You'll need:

* A good knowledge of git.

* A fork of the GitHub repo.

* An understanding of the project's development workflow.

* Legal authorization, that is, you need to have signed a Contributor
  License Agreement. See below for details.

## Legal Requirements

OpenFX is a project of the Academy Software Foundation and follows the
open source software best practice policies of the Linux Foundation.

### License

OpenFX is licensed under the [BSD-3-Clause](LICENSE.md)
license. Contributions to the library should abide by that standard
license.

### Contributor License Agreements

We do not require a CLA at this time.

### Commit Sign-Off

Every commit must be signed off.  That is, every commit log message
must include a “`Signed-off-by`” line (generated, for example, with
“`git commit --signoff`”), indicating that the committer wrote the
code and has the right to release it under the [BSD-3-Clause](LICENSE.md)
license. See https://github.com/AcademySoftwareFoundation/tac/blob/main/process/contributing.md#contribution-sign-off for more information on this requirement.

## Development Workflow

### Git Basics

Working with OpenFX requires understanding a significant amount of
Git and GitHub based terminology. If you’re unfamiliar with these
tools or their lingo, please look at the [GitHub
Glossary](https://help.github.com/articles/github-glossary/) or browse
[GitHub Help](https://help.github.com/).

To contribute, you need a GitHub account. This is needed in order to
push changes to the upstream repository, via a pull request.

You will also need Git installed on your local development machine. If
you need setup assistance, please see the official [Git
Documentation](https://git-scm.com/doc).

### Repository Structure and Commit Policy

The OpenFX repository uses a simple branching and merging strategy.

All development work is done directly on the ``main`` branch. The ``main``
branch represents the bleeding-edge of the project and most
contributions should be done on top of it.

After sufficient work is done on the ``main`` branch and the OpenFX
leadership determines that a release is due, we will bump the relevant
internal versioning and tag a commit with the corresponding version
number. Each Minor version also has its own “Release
Branch”, e.g. RB-1.1. This marks a branch of code dedicated to that
Major.Minor version, which allows upstream bug fixes to be
cherry-picked to a given version while still allowing the ``main``
branch to continue forward onto higher versions. This basic repository
structure keeps maintenance low, while remaining simple to understand.

To reiterate, the ``main`` branch represents the latest development
version, so beware that it may include untested features and is not
generally stable enough for release.  To retrieve a stable version of
the source code, use one of the release branches.

### The Git Workflow

This development workflow is sometimes referred to as
[OneFlow](https://www.endoflineblog.com/oneflow-a-git-branching-model-and-workflow). It
leads to a simple, clean, linear edit history in the repo.

### Use the Fork, Luke.

In a typical workflow, you should **fork** the OpenFX repository to
your account. This creates a copy of the repository under your user
namespace and serves as the “home base” for your development branches,
from which you will submit **pull requests** to the upstream
repository to be merged.

Once your Git environment is operational, the next step is to locally
**clone** your forked OpenFX repository, and add a **remote**
pointing to the upstream OpenFX repository. These topics are
covered in the GitHub documentation [Cloning a
repository](https://help.github.com/articles/cloning-a-repository/)
and [Configuring a remote for a
fork](https://help.github.com/articles/configuring-a-remote-for-a-fork/),
but again, if you need assistance feel free to reach out on the
openexr-dev@lists.aswf.io mail list.

### Pull Requests

Contributions should be submitted as Github pull requests. See
[Creating a pull request](https://help.github.com/articles/creating-a-pull-request/)
if you're unfamiliar with this concept.

The development cycle for a code change should follow this protocol:

1. Create a topic branch in your local repository, following the naming format
"feature/<your-feature>" or "bugfix/<your-fix>".

2. Make changes, compile, and test thoroughly. Code style should match existing
style and conventions, and changes should be focused on the topic the pull
request will be addressing. Make unrelated changes in a separate topic branch
with a separate pull request.

3. Push commits to your fork.

4. Create a Github pull request from your topic branch.

5. Pull requests will be reviewed by project Committers and Contributors,
who may discuss, offer constructive feedback, request changes, or approve
the work.

6. Upon receiving the required number of Committer approvals (as
outlined in [Required Approvals](#required-approvals)), a Committer
other than the PR contributor may merge changes into the ``main``
branch.

### Code Review and Required Approvals

Modifications of the contents of the OpenFX repository are made on a
collaborative basis. Anyone with a GitHub account may propose a
modification via pull request and it will be considered by the project
Committers.

Pull requests must meet a minimum number of Committer approvals prior
to being merged. Rather than having a hard rule for all PRs, the
requirement is based on the complexity and risk of the proposed
changes, factoring in the length of time the PR has been open to
discussion. The following guidelines outline the project's established
approval rules for merging:

* Changes to the OpenFX Standard, that is, changes to the main header
files in [include], including new suites, new properties, or incompatible
modifications to existing suites, go through a [Standard
Process](STANDARD_PROCESS.md) which involves these steps:

 * Proposal
 * Discussion
 * Implementation (by one plug-in and one host)
 * Documentation
 * Approval

For all other changes:

* Core design decisions, large new features, or anything that might be
perceived as changing the overall direction of the project should be
discussed at length in the mail list before any PR is submitted, in
order to: solicit feedback, try to get as much consensus as possible,
and alert all the stakeholders to be on the lookout for the eventual
PR when it appears.

* Small changes (bug fixes, docs, tests, cleanups) can be approved and
merged by a single Committer.

* Big changes that can alter behavior, add major features, or present
a high degree of risk should be signed off by TWO Committers, ideally
one of whom should be the "owner" for that section of the codebase (if
a specific owner has been designated). If the person submitting the PR
is him/herself the "owner" of that section of the codebase, then only
one additional Committer approval is sufficient. But in either case, a
48 hour minimum is helpful to give everybody a chance to see it,
unless it's a critical emergency fix (which would probably put it in
the previous "small fix" category, rather than a "big feature").

* Escape valve: big changes can nonetheless be merged by a single
Committer if the PR has been open for over two weeks without any
unaddressed objections from other Committers. At some point, we have
to assume that the people who know and care are monitoring the PRs and
that an extended period without objections is really assent.

Approval must be from Committers who are not authors of the change. If
one or more Committers oppose a proposed change, then the change
cannot be accepted unless:

* Discussions and/or additional changes result in no Committers
objecting to the change. Previously-objecting Committers do not
necessarily have to sign-off on the change, but they should not be
opposed to it.

* The change is escalated to the TSC and the TSC votes to approve the
change.  This should only happen if disagreements between Committers
cannot be resolved through discussion.

Committers may opt to elevate significant or controversial
modifications to the TSC by assigning the `tsc-review` label to a pull
request or issue. The TSC should serve as the final arbiter where
required.

### Test Policy

OpenFX runs an automated CI build process on github. All builds must
pass before merging a PR.

#### Copyright Notices

All new source files should begin with a copyright and license stating:

    //
    // SPDX-License-Identifier: BSD-3-Clause
    // Copyright (c) OpenFX and contributors to the OpenFX Project.
    //

Prior to becoming an ASWF project, OpenFX was controlled by The Open Effects
Association, a legal entity which had been assigned copyright in the original
contributions by a variety of companies and individuals. When the project
transferred to ASWF, these copyrights were assigned to a new legal entity, 
OpenFX a Series of LF Projects, LLC (abbreviated to "OpenFX" in the notice).
In line with ASWF principles, new contributions do not require a copyright
assignment, but our copyright notice reflects this mixed history, and is 
applied consistently across the project to avoid confusion when code is moved
between old and new files.

#### Third-party libraries

The standard itself should not depend on any third-party library or
headers. It must be pure C code.

For the C++ examples and wrappers, C++11 `std` is recommended, but no
other third-party libraries should be used except for special purposes
such as image processing for particular examples.

#### Comments and Doxygen

The standard is documented in the header files, and the comments there
are used to auto-generate the reference documentation. So correct,
concise and clear documentation is absolutely vital. Since this is a
standard and not just a library, code reviews will focus as much on
the documentation as the code itself.

To build the docs, follow the
[Documentation/README.md](Documentation/README.md) instructions.
