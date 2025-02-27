#
# MIT License
#
# Copyright(c) 2011-2020 The Maintainers of Nanvix
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

#
# Default make rule.
#
.DEFAULT_GOAL := all

#===============================================================================
# Build Options
#===============================================================================

# Verbose build?
export VERBOSE ?= no

# Release Version?
export RELEASE ?= no

# Installation Prefix
export PREFIX ?= $(HOME)

# Use Docker?
export DOCKER ?= no

# Stall regression tests?
export SUPPRESS_TESTS ?= no

#===============================================================================
# Directories
#===============================================================================

export ROOTDIR    := $(CURDIR)
export BINDIR     := $(ROOTDIR)/bin
export BUILDDIR   := $(ROOTDIR)/build
export CONTRIBDIR := $(ROOTDIR)/contrib
export DOCDIR     := $(ROOTDIR)/doc
export IMGDIR     := $(ROOTDIR)/img
export INCDIR     := $(ROOTDIR)/include
export LIBDIR     := $(ROOTDIR)/lib
export LINKERDIR  := $(BUILDDIR)/$(TARGET)/linker
export MAKEDIR    := $(BUILDDIR)/$(TARGET)/make
export SRCDIR     := $(ROOTDIR)/src
export TOOLSDIR   := $(ROOTDIR)/utils

#===============================================================================
# Libraries and Binaries
#===============================================================================

# Libraries
export BARELIB = barelib-$(TARGET).a
export LIBHAL  = libhal-$(TARGET).a

# Binaries
export EXEC := test-driver.$(TARGET)

#===============================================================================
# Target-Specific Make Rules
#===============================================================================

include $(MAKEDIR)/makefile

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Compiler Options
export CFLAGS += -std=c99 -fno-builtin
export CFLAGS += -Wall -Wextra -Werror -Wa,--warn
export CFLAGS += -Winit-self -Wswitch-default -Wfloat-equal
export CFLAGS += -Wundef -Wshadow -Wuninitialized -Wlogical-op
export CFLAGS += -Wvla # -Wredundant-decls
export CFLAGS += -fno-stack-protector
export CFLAGS += -Wno-unused-function
export CFLAGS += -I $(INCDIR)
export CFLAGS += -D__NANVIX_HAL
export CFLAGS += -D __INTERFACE_CHECK_CORE_AL
export CFLAGS += -D __INTERFACE_CHECK_CLUSTER_AL
export CFLAGS += -D __INTERFACE_CHECK_PROCESSOR_AL
export CFLAGS += -D __INTERFACE_CHECK_TARGET_AL

# Enable sync and portal implementation that uses mailboxes
export CFLAGS += -D__NANVIX_IKC_USES_ONLY_MAILBOX=0

# Additional C Flags
include $(BUILDDIR)/makefile.cflags

# Archiver Options
export ARFLAGS = rc

#===============================================================================

# Image Source
export IMGSRC = $(IMGDIR)/$(TARGET).img

# Image Name
export IMAGE = hal-debug.img

# Builds everything.
all: image

# Make directories
make-dirs:
	@mkdir -p $(BINDIR)
	@mkdir -p $(LIBDIR)

# Builds image.
image: make-dirs
	$(MAKE) -C $(SRCDIR) all
	@bash $(TOOLSDIR)/nanvix-build-image.sh $(IMAGE) $(BINDIR) $(IMGSRC)

# Cleans everything.
clean:
	$(MAKE) -C $(SRCDIR) clean
	@rm -rf $(IMAGE) $(BINDIR)/$(EXECBIN) $(LIBDIR)/$(LIBHAL)

# Builds documentation.
documentation:
	mkdir -p $(DOCDIR)
	doxygen doxygen/doxygen.$(TARGET)

#===============================================================================
# Contrib Install and Uninstall Rules
#===============================================================================

# Builds Kernel Library.
contrib: make-dirs
	$(MAKE) -C $(CONTRIBDIR)/barelib install LIBHAL="" PREFIX=$(ROOTDIR) RELEASE=$(RELEASE)

# Builds Kernel Library headers.
contrib-headers: make-dirs
	$(MAKE) -C $(CONTRIBDIR)/barelib install-headers LIBHAL="" PREFIX=$(ROOTDIR) RELEASE=$(RELEASE)

# Cleans Kernel Library.
contrib-uninstall:
	$(MAKE) -C $(CONTRIBDIR)/barelib uninstall LIBHAL="" PREFIX=$(ROOTDIR) RELEASE=$(RELEASE)

# Cleans Kernel Library headers.
contrib-uninstall-headers:
	$(MAKE) -C $(CONTRIBDIR)/barelib uninstall-headers LIBHAL="" PREFIX=$(ROOTDIR) RELEASE=$(RELEASE)

#===============================================================================
# Install and Uninstall Rules
#===============================================================================

include $(BUILDDIR)/install.mk

#===============================================================================
# Debug and Run Rules
#===============================================================================

include $(BUILDDIR)/run.mk
