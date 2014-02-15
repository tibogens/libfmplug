#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import _winreg

def get_user_plugin_dir(create=True):
	appdata = os.getenv('LOCALAPPDATA')
	fmext = os.path.join(appdata, 'FileMaker', 'Extensions')
	if not os.path.isdir(fmext):
		if not create:
			raise OSError("Use plugin path '%' does not exist" % fmext)
		os.makedirs(fmext)
	return fmext
	
def get_filemaker_dir():
	for fmver in ['13.0', '12.0', '11.0', '10.0', '9.0']:
		try:
			keyname = '\\'.join(['Software','FileMaker','FileMaker Pro',fmver,'Preferences'])
			with _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, keyname) as key:
				value, regtype = _winreg.QueryValueEx(key, 'Main Dictionary')
				if regtype == _winreg.REG_SZ:
					# This actual value is really not interesting to us, but its path
					# is that of FileMaker. The value should look like
					# C:\Program Files (x86)\FileMaker\FileMaker Pro 9\Extensions\Dictionaries\foobar.mpr
					# so we have to climb two directories up.
					path_to_fmpro = os.path.normpath(os.path.join(os.path.dirname(value), '..', '..'))
					if os.path.isfile(os.path.join(path_to_fmpro, 'FileMaker Pro.exe')):
						return path_to_fmpro
		except WindowsError, err:
			pass
	raise OSError("Could not find a valid FileMaker Pro installation")
	
def install_plugin():
	pass
	
def main():
	print "User extension path",get_user_plugin_dir()
	print "Filemaker dir",get_filemaker_dir()
	
if __name__ == '__main__':
	sys.exit(main())