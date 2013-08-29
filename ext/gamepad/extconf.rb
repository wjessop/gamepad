ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require 'mkmf'

LIBDIR      = RbConfig::CONFIG['libdir'] + '/gamepad'
INCLUDEDIR  = RbConfig::CONFIG['includedir']
# RbConfig::CONFIG['CC'] = '/usr/bin/clang -v'

HEADER_DIRS = [
	# Then search /usr/local for people that installed from source
	'/usr/local/include',
	
	# Check the ruby install locations
	INCLUDEDIR,
	
	# Finally fall back to /usr
	'/usr/include'
]

LIB_DIRS = [
	# Then search /usr/local for people that installed from source
	'/usr/local/lib',
	
	# Check the ruby install locations
	LIBDIR,
	
	# Finally fall back to /usr
	'/usr/lib'
]

dir_config('gamepad', HEADER_DIRS, LIB_DIRS)

unless find_header('libusb-1.0/libusb.h')
	abort "libusb is missing.  please install libusb"
end

unless find_library('usb-1.0', 'libusb_init')
	abort "libusb is missing.  please install libusb"
end

create_makefile('gamepad/gamepad')
