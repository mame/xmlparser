#
# ruby extconf.rb
#         --with-perl-enc-map[=/path/to/enc-map]
#         --with-expat-dir=/path/to/expat
#         --with-expat-lib=/path/to/expat/lib
#         --with-expat-include=/path/to/expat/include
#
require 'mkmf'

cwd=`pwd`.chomp!
perl= ENV['PERL'] || 'perl'

## Encoding maps may be stored in $perl_archlib/XML/Parser/Encodins/
#perl_archlib = '/usr/lib/perl5/site_perl/5.005/i586-linux'
#perl_archlib = '/usr/local/lib'
perl_archlib = `#{perl} -e 'use Config; print $Config{"archlib"}'`
xml_enc_path = with_config("perl-enc-map")
if xml_enc_path == true
  xml_enc_path = perl_archlib + "/XML/Parser/Encodings"
end

##$CFLAGS="-I#{cwd}/expat/xmlparse -I#{cwd}/expat/xmltok" +
##  ' -DXML_ENC_PATH=getenv\(\"XML_ENC_PATH\"\)' +
##  " -DNEW_EXPAT"
#$CFLAGS = "-I#{cwd}/expat/xmlparse -I#{cwd}/expat/xmltok"
#$LDFLAGS = "-L#{cwd}/expat/xmlparse -Wl,-rpath,/usr/local/lib"
#$LDFLAGS = "-L#{cwd}/expat/xmlparse"
dir_config("expat")
#dir_config("xmltok")
#dir_config("xmlparse")
if xml_enc_path
  $CFLAGS += " -DXML_ENC_PATH=\\\"#{xml_enc_path}\\\""
end

#if have_header("xmlparse.h") || have_header("expat.h")
if have_header("expat.h") || have_header("xmlparse.h")
  if have_library("expat", "XML_ParserCreate") ||
      have_library("xmltok", "XML_ParserCreate")
    if have_func("XML_SetNotStandaloneHandler")
      $CFLAGS += " -DNEW_EXPAT"
    end
    if have_func("XML_SetParamEntityParsing")
      $CFLAGS += " -DXML_DTD"
    end
#    if have_func("XML_SetExternalParsedEntityDeclHandler")
#      $CFLAGS += " -DEXPAT_1_2"
#    end
    have_func("XML_SetDoctypeDeclHandler")
    have_func("XML_ParserReset")
    have_func("XML_SetSkippedEntityHandler")
    have_func("XML_GetFeatureList")
    have_func("XML_UseForeignDTD")
    have_func("XML_GetIdAttributeIndex")
    have_library("socket", "ntohl")
    have_library("wsock32") if RUBY_PLATFORM =~ /mswin32|mingw/
    create_makefile("xmlparser")
  end
end
