# gamepad

Gamepad is a gem I wrote to get the values of my xbox1 controller in a ruby program (to control my Parrot AR drone). I plan to use it for piloting my Parrot AR drone and other robotics stuff. It wasn't designed for use in game libraries, but it might work for you.

Right now only (original) xbox1 controllers are supported. I plan to add PS1 controller support when I have time, and support for other controllers if I ever get any (and also have time).

This is my first adventure into programming a Ruby c extention, and into c itself, so this lib is probably not safe to use in production, but then who needs to interface with an xbox1 controller in production…

Gamepad is a very early dev release made for me to play around with the xbox gamepad from Ruby, the API will likely change. Pull requests welcome.

## Installation

On a mac:

		brew install libusbx

On another platform YMMV. Add this line to your application's Gemfile:

		gem 'gamepad'

And then execute:

		$ bundle

Or install it yourself as:

		$ gem install gamepad

## Usage

Plug your xbox controller into your computer using an xbox -> USB adaptor (they're pretty cheap, I got mine off ebay) then run:

````ruby
require 'gamepad'

gamepad = GameController::Xbox1.new
gamepad.on_changed do |values|
	puts "Changed to #{values.inspect}"
end
````

See examples/.

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request

## TODO

- auto-detect xbox controllers being added/removed
- Handle more than one controller type
- display_xbox_status(libusb_device_handle *handle) crashes when the xbox controller is not connected
- ps1 controller
- Maybe use the async API of libusbx
- Signal handling is kind of odd

## Author

* Will Jessop, @will_j, will@willj.net
