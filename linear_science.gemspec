# -*- encoding: utf-8 -*-
require File.expand_path('../lib/linear_science/version', __FILE__)

Gem::Specification.new do |gem|
  gem.authors       = ["Szymon Sobański\n"]
  gem.email         = ["sobanski.s@gmail.com\n"]
  gem.description   = %q{TODO: Write a gem description}
  gem.summary       = %q{TODO: Write a gem summary}
  gem.homepage      = ""

  gem.files         = `git ls-files`.split($\)
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.name          = "linear_science"
  gem.require_paths = ["lib"]
  gem.version       = LinearScience::VERSION

  gem.add_development_dependency('rspec')
  gem.add_development_dependency('rb-inotify')
end