# OpenFX image processing plug-in standard

The authoritative source for information about OFX is http://openeffects.org/

* [OFX Programming Guide By Example](https://github.com/ofxa/openfx/tree/master/Guide)
* [OFX API v. 1.4 Reference](http://openeffects.org/documentation/api_doc)
* [OFX API Programming Guide](http://openeffects.org/documentation/guide)
* [OFX API Programming Reference](http://openeffects.org/documentation/reference)
* [OFX Discussion Google Group](https://groups.google.com/forum/#!forum/ofx-discussion)

## Why a Standard?
VFX plug-in vendors were frustrated for years because host application vendors created proprietary plug-in interfaces. As a result, each plug-in vendor had to port their plug-ins to all the different hosts and hosts couldn't use each other's plug-ins, limiting the selection of effects available to artists. The need for a standard interface was clear, so Bruno Nicoletti of The Foundry led the effort to develop a standard. That standard is OFX.

OFX is a win for artists because there is no waiting for plug-in vendors to port their cool effects to your application. Once a host compositing or editing application adopts OFX, all OFX plug-ins on the market instantly become available on that host.

And OFX is a win for plug-in vendors because they can concentrate on what they do best: making cool effects

## OFX Terminology
### Host
A video compositing or editing application, such as The Foundry Nuke, Assimilate Scratch, Sony Vegas, or FilmLight Baselight
### Plug-in
Video software, such as GenArts Sapphire or RE:Vison Effects which adds a wider variety of effects to a host application.
### Open Effects
A standardized software interface between VFX host applications and plug-ins (also known as OpenFX and OFX).
### Editor
An application which allows you to manipulate a video timeline by adding, removing, and changing the in and out points of video clips. Effects, Generators, Transition, Compositors and Retiming effects are commonly used in editors.
### Compositor
An application which allows you build a video clip by layering video clips, still images, and effects.

## About the Open Effects Association

The Open Effects Association (OFX), a non-profit organization, develops and promotes open standards across the visual effects community. The founding members come from Assimilate, Autodesk, Digieffects, FilmLight, The Foundry, GenArts and RE:Vision FX. These are companies which have helped artists create ground-breaking VFX shots on nearly every blockbuster movie.

The Association's initial focus is to improve the OpenFX image processing plug-in standard. This goal of this standard is to reduce development effort and support needed for plug-ins across different compositing and editing host platforms.
