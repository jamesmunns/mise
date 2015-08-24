#Mise en place

Mise en place is a French phrase meaning "putting in place". It is commonly used in kitchens to describe the state of having all of the ingredients set up and ready to use.

In a similar vein, I am tired of assembling all the parts for a project as I go, which usually means I do a poor job hacking them in just after I need them.

To deal with this, I would like a foundation that contains all of the following as a platform to start any embedded hobby project I have in the future:

## Components

* A build system
    * Current choice: Fabricate
* A messaging system
    * Current choice: Protobufs, NanoPb, and a custom transport layer, fit for serial (UART) data between Python, C, or C++ nodes
* A unit test system
    * Current choice: CMocka for C, GoogleTest for C++
* Scripting glue to hold it all together
    * Current choice: Python 2.7.x, because I know it.

## Objectives

* Be able to start a new project, for any hardware platform, and have a working Hello World as soon as possible
* Keep development of dissimilar projects consistent, so that I can pick-up-and-go between systems, and port cool things from one to the other
* Minimize external dependancies. Each installation should be as self contained and portable as possible.

## Restrictions

* I currently only plan to support GCC or Clang compilers. Probably just GCC.
* I currently only plan to support *nix environments, including within a VM. Will be Debian based (probably Ubuntu)
