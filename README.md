libfmplug
=========

A C++ library to create FileMaker Pro™ plugins. Works with FM Pro 9, 10, 11, 12 and 13.

License
-------

The libfmplug library is licensed under the GNU Lesser General Public License, version 3 (LGPLv3).

However:
- the samples are licensed under a BSD 3-clause license.
- the FileMaker Pro™ SDK (headers and libraries) is the property of FileMaker, Inc. and can only be used to "enable
 licensees of FileMaker Pro Advanced to compile plug-ins for use with FileMaker products" (refer to the notice in FMWrapper/FMX*.h files)

Please note that if your plugin needs to be linked against the static version of libfmplug, you need to follow [the requirements explained in the GPL FAQ](http://www.gnu.org/licenses/gpl-faq.html#LGPLStaticVsDynamic). [Issue 1](https://github.com/tibogens/libfmplug/issues/1) is the place to discuss this.

