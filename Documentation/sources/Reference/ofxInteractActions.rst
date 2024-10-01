.. SPDX-License-Identifier: CC-BY-4.0
.. InteractActions:

Actions Passed to an Interact
=============================

This chapter describes the actions that can be issued to an interact's
main entry point. Interact actions are also generic in character, they
could be issued to other plug-in types rather than just Image Effects,
however they are not issued directly to an effect's main entry point,
they are rather issued to separate entry points which exist on specific
'interact' objects that a plugin may create.

For nearly all the actions the ``handle`` passed to to main entry point
for an interact will be either NULL, or a value that should be cast to
an :cpp:type:`OfxInteractHandle`.

.. doxygendefine:: kOfxActionDescribeInteract

.. doxygendefine:: kOfxActionCreateInstanceInteract

.. doxygendefine:: kOfxActionDestroyInstanceInteract

.. doxygendefine:: kOfxInteractActionDraw

.. doxygendefine:: kOfxInteractActionPenMotion

.. doxygendefine:: kOfxInteractActionPenDown

.. doxygendefine:: kOfxInteractActionPenUp

.. doxygendefine:: kOfxInteractActionKeyDown

.. doxygendefine:: kOfxInteractActionKeyUp

.. doxygendefine:: kOfxInteractActionKeyRepeat

.. doxygendefine:: kOfxInteractActionGainFocus

.. doxygendefine:: kOfxInteractActionLoseFocus

