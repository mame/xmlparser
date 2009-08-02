# -*- ruby -*-

require 'rubygems'
require 'rbconfig'
require 'hoe'

EXT = "ext/xmlparser.#{RbConfig::CONFIG["DLEXT"]}"

class XMLParser
  VERSION = "0.6.81.1"
end

Hoe.spec('xmlparser') do
  self.version = XMLParser::VERSION
  self.rubyforge_name = 'xmlparser' # if different than lowercase project name
  self.developer('Yoshida Mataso with Jeff Hodges with Yusuke Endoh', 'mame@tsg.ne.jp')
  self.spec_extras[:extensions] = "ext/extconf.rb"
  self.clean_globs << EXT << "ext/*.o" << "ext/Makefile" << "ext/mkmf.log"
end

task :test => EXT

file EXT => ["ext/extconf.rb", "ext/xmlparser.c"] do
  Dir.chdir "ext" do
    ruby "extconf.rb"
    sh "make"
  end
end

# vim: syntax=ruby
