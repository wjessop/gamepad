$:.unshift 'lib'
require 'gamepad'

gamepad = GameController::Xbox1.new
gamepad.on_changed do |values|
	puts "Changed to #{values.inspect}"
end

puts "Done!"
