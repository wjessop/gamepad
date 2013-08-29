# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'gamepad/version'

Gem::Specification.new do |spec|
	spec.name          = "gamepad"
	spec.version       = Gamepad::VERSION
	spec.authors       = ["Will Jessop"]
	spec.email         = ["will@willj.net"]
	spec.description   = %q{Get values from an xbox1 gamepad}
	spec.summary       = %q{Get values from an xbox1 gamepad}
	spec.homepage      = "https://github.com/wjessop/gamepad"
	spec.license       = "MIT"

	spec.files         = `git ls-files`.split($/)
	spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
	spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
	spec.require_paths = ["lib"]

	spec.add_development_dependency "bundler", "~> 1.3"
	spec.add_development_dependency "rake"
	spec.add_development_dependency "rake-compiler"
end
