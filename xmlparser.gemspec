# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{xmlparser}
  s.version = "0.6.81.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Yoshida Mataso with Jeff Hodges with Yusuke Endoh"]
  s.date = %q{2009-08-02}
  s.description = %q{Expat (XML Parser Toolkit) Module for Ruby

This is a module to access to James Clark's XML Parser
Toolkit "expat" (http://www.jclark.com/xml/expat.html) from
Ruby.

This is a modified version of xmlparser:

- works on Ruby 1.9
- depends on hoe instead of mkrf}
  s.email = ["mame@tsg.ne.jp"]
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = ["Manifest.txt", "History.txt", "README.txt"]
  s.files = ["Manifest.txt", "History.txt", "README.txt", "README", "README.ja", "Rakefile", "ext/extconf.rb", "ext/xmlparser.c", "ext/encoding.h", "lib/sax.rb", "lib/saxdriver.rb", "lib/wget.rb", "lib/xml/dom/builder-ja.rb", "lib/xml/dom/builder.rb", "lib/xml/dom/core.rb", "lib/xml/dom/digest.rb", "lib/xml/dom/visitor.rb", "lib/xml/dom2/attr.rb", "lib/xml/dom2/cdatasection.rb", "lib/xml/dom2/characterdata.rb", "lib/xml/dom2/comment.rb", "lib/xml/dom2/core.rb", "lib/xml/dom2/document.rb", "lib/xml/dom2/documentfragment.rb", "lib/xml/dom2/documenttype.rb", "lib/xml/dom2/dombuilder.rb", "lib/xml/dom2/dombuilderfilter.rb", "lib/xml/dom2/domentityresolver.rb", "lib/xml/dom2/domentityresolverimpl.rb", "lib/xml/dom2/domexception.rb", "lib/xml/dom2/domimplementation.rb", "lib/xml/dom2/dominputsource.rb", "lib/xml/dom2/element.rb", "lib/xml/dom2/entity.rb", "lib/xml/dom2/entityreference.rb", "lib/xml/dom2/namednodemap.rb", "lib/xml/dom2/node.rb", "lib/xml/dom2/nodelist.rb", "lib/xml/dom2/notation.rb", "lib/xml/dom2/processinginstruction.rb", "lib/xml/dom2/text.rb", "lib/xml/dom2/xpath.rb", "lib/xml/encoding-ja.rb", "lib/xml/parser.rb", "lib/xml/parserns.rb", "lib/xml/sax.rb", "lib/xml/saxdriver.rb", "lib/xml/xpath.rb", "lib/xml/xpath.ry", "lib/xmldigest.rb", "lib/xmlencoding-ja.rb", "lib/xmltree.rb", "lib/xmltreebuilder-ja.rb", "lib/xmltreebuilder.rb", "lib/xmltreevisitor.rb", "samples/buildertest.rb", "samples/buildertest2.rb", "samples/digesttest.rb", "samples/digesttest2.rb", "samples/doctype.rb", "samples/doctype.xml", "samples/doctypei.rb", "samples/document.dtd", "samples/dom2/dom2test1.rb", "samples/dom2/dom2test2.rb", "samples/dom2/gtkxpath.rb", "samples/dom2/test1.xml", "samples/dom2/test2.xml", "samples/dtd/ext1.dtd", "samples/dtd/ext2.dtd", "samples/dtd/extdtd.rb", "samples/dtd/extdtd.xml", "samples/expat-1.2/CVS/Entries", "samples/expat-1.2/CVS/Repository", "samples/expat-1.2/CVS/Root", "samples/expat-1.2/ext.ent", "samples/expat-1.2/exttest.rb", "samples/expat-1.2/exttesti.rb", "samples/expat-1.2/hoge.dtd", "samples/expat-1.2/idattr.xml", "samples/expat-1.2/idtest.rb", "samples/expat-1.2/idtest.xml", "samples/expat-1.2/xmlextparser.rb", "samples/gtktree.rb", "samples/idattrtest.rb", "samples/index_euc.xml", "samples/index_jis.xml", "samples/index_noenc.xml", "samples/index_sjis.xml", "samples/index_u16.xml", "samples/index_u8.xml", "samples/my-html.rb", "samples/namespaces/CVS/Entries", "samples/namespaces/CVS/Repository", "samples/namespaces/CVS/Root", "samples/namespaces/namespace1.rb", "samples/namespaces/namespace1.xml", "samples/namespaces/namespace2.rb", "samples/namespaces/namespace2.xml", "samples/nstest.rb", "samples/saxtest.rb", "samples/test/featurelist.rb", "samples/test/skippedentity.rb", "samples/test/useforeigndtd.rb", "samples/treetest.rb", "samples/visitor.rb", "samples/visitortest.rb", "samples/writer.rb", "samples/xmlcheck.rb", "samples/xmlcomments.rb", "samples/xmlevent.rb", "samples/xmliter.rb", "samples/xmlstats.rb", "samples/xpointer.rb", "samples/xpointertest.rb", "Encodings/README.ja", "Encodings/euc-jp.enc", "Encodings/shift_jis.enc"]
  s.homepage = %q{http://www.yoshidam.net/Ruby.html#xmlparser}
  s.rdoc_options = ["--main", "README.txt"]
  s.require_paths = ["lib", "ext"]
  s.rubyforge_project = %q{xmlparser}
  s.rubygems_version = %q{1.3.4}
  s.summary = %q{Expat (XML Parser Toolkit) Module for Ruby  This is a module to access to James Clark's XML Parser Toolkit "expat" (http://www.jclark.com/xml/expat.html) from Ruby}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<hoe>, [">= 2.3.2"])
    else
      s.add_dependency(%q<hoe>, [">= 2.3.2"])
    end
  else
    s.add_dependency(%q<hoe>, [">= 2.3.2"])
  end
end
