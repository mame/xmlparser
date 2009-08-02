/*
 *  Expat (XML Parser Toolkit) wrapper for Ruby
 *  Feb 16, 2004 yoshidam version 0.6.8  taint output string
 *  Feb 16, 2004 yoshidam version 0.6.7  fix buffer overflow
 *  Mar 11, 2003 yoshidam version 0.6.6  fix skippedEntity handler
 *  Sep 20, 2002 yoshidam version 0.6.5  fix reset method
 *  Apr  4, 2002 yoshidam version 0.6.3  change event code values
 *  Oct 10, 2000 yoshidam version 0.6.1  support expat-1.2
 *  Oct  6, 2000 yoshidam version 0.6.0  support expat-1.95.0
 *  Jun 28, 1999 yoshidam version 0.5.18 define initialize for Ruby 1.5
 *  Jun 28, 1999 yoshidam version 0.5.15 support start/endDoctypeDecl
 *  Jun 28, 1999 yoshidam version 0.5.14 support setParamEntityParsing
 *  Apr 28, 1999 yoshidam version 0.5.11 support notStandalone
 *  Mar 29, 1998 yoshidam version 0.5.9  optimize for Ruby 1.3
 *  Mar  8, 1998 yoshidam version 0.5.7  support start/endNamespaceDecl
 *  Jan 14, 1998 yoshidam version 0.5.4  support start/endCdataSection
 *  Jan 10, 1998 yoshidam version 0.5.3  support encoding map
 *  Nov 24, 1998 yoshidam version 0.5.0  support TEST version of expat
 *  Nov  5, 1998 yoshidam version 0.4.18 mIDs are initialized in Init_xmlparser
 *  Oct 28, 1998 yoshidam version 0.4.17 mIDs are stored into static vars
 *  Oct 13, 1998 yoshidam version 0.4.12 debug and speed up myEncodingConv
 *  Oct  7, 1998 yoshidam version 0.4.11 hold internal object into ivar
 *  Sep 18, 1998 yoshidam version 0.4.6
 *  Sep  8, 1998 yoshidam version 0.4.4
 *  Sep  3, 1998 yoshidam version 0.4.3
 *  Sep  1, 1998 yoshidam version 0.4.2
 *  Aug 28, 1998 yoshidam version 0.4.1
 *  Aug 22, 1998 yoshidam version 0.4.0
 *  Jul  6, 1998 yoshidam version 0.2
 *  Jun 30, 1998 yoshidam version 0.1
 *
 *  XML_ENC_PATH: path of encoding map for Perl
 *  HAVE_XML_USEFOREIGNDTD: expat 1.95.5
 *  HAVE_XML_GETFEATURELIST: expat 1.95.5
 *  HAVE_XML_SETSKIPPEDENTITYHANDLER: expat 1.95.4
 *  HAVE_XML_PARSERRESET: expat 1.95.3
 *  HAVE_EXPAT_H: expat 1.95.0
 *  HAVE_XML_SETDOCTYPEDECLHANDLER: expat 19990728
 *  XML_DTD: expat 19990626
 *  NEW_EXPAT: expat 1.1
 */

#include "ruby.h"
#ifdef HAVE_RUBY_IO_H
#include "ruby/io.h"
#else
#include "rubyio.h"
#endif
#include <stdio.h>
#include <ctype.h>
#ifdef HAVE_EXPAT_H
#  include "expat.h"
#else
#  include "xmlparse.h"
#endif
#ifdef XML_ENC_PATH
#  include <limits.h>
#  include <sys/stat.h>
#  include "encoding.h"
#  ifndef PATH_MAX
#    define PATH_MAX 256
#  endif
#endif

static VALUE eXMLParserError;
static VALUE cXMLParser;
static VALUE cXMLEncoding;
static ID id_map;
static ID id_startElementHandler;
static ID id_endElementHandler;
static ID id_characterDataHandler;
static ID id_processingInstructionHandler;
static ID id_defaultHandler;
static ID id_defaultExpandHandler;
static ID id_unparsedEntityDeclHandler;
static ID id_notationDeclHandler;
static ID id_externalEntityRefHandler;
static ID id_unknownEncoding;
static ID id_convert;
#ifdef NEW_EXPAT
static ID id_commentHandler;
static ID id_startCdataSectionHandler;
static ID id_endCdataSectionHandler;
static ID id_startNamespaceDeclHandler;
static ID id_endNamespaceDeclHandler;
static ID id_notStandaloneHandler;
#endif
#ifdef HAVE_XML_SETDOCTYPEDECLHANDLER
static ID id_startDoctypeDeclHandler;
static ID id_endDoctypeDeclHandler;
#endif
#ifdef HAVE_EXPAT_H
static ID id_elementDeclHandler;
static ID id_attlistDeclHandler;
static ID id_xmlDeclHandler;
static ID id_entityDeclHandler;
#endif
#if 0
static ID id_externalParsedEntityDeclHandler;
static ID id_internalParsedEntityDeclHandler;
#endif
#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
static ID id_skippedEntityHandler;
#endif

#define GET_PARSER(obj, parser) \
  Data_Get_Struct(obj, XMLParser, parser)

typedef struct _XMLParser {
  XML_Parser parser;
  int iterator;
  int defaultCurrent;
#ifdef NEW_EXPAT
  const XML_Char** lastAttrs;
#endif
  int tainted;
  VALUE parent;
  char* context;
} XMLParser;

static VALUE symDEFAULT;
static VALUE symSTART_ELEM;
static VALUE symEND_ELEM;
static VALUE symCDATA;
static VALUE symPI;
static VALUE symUNPARSED_ENTITY_DECL;
static VALUE symNOTATION_DECL;
static VALUE symEXTERNAL_ENTITY_REF;
#ifdef NEW_EXPAT
static VALUE symCOMMENT;
static VALUE symSTART_CDATA;
static VALUE symEND_CDATA;
static VALUE symSTART_NAMESPACE_DECL;
static VALUE symEND_NAMESPACE_DECL;
#endif
#ifdef HAVE_XML_SETDOCTYPEDECLHANDLER
static VALUE symSTART_DOCTYPE_DECL;
static VALUE symEND_DOCTYPE_DECL;
#endif
#ifdef HAVE_EXPAT_H
static VALUE symELEMENT_DECL;
static VALUE symATTLIST_DECL;
static VALUE symXML_DECL;
static VALUE symENTITY_DECL;
#endif
#if 0
static VALUE symEXTERNAL_PARSED_ENTITY_DECL;
static VALUE symINTERNAL_PARSED_ENTITY_DECL;
#endif
#if 0
static VALUE symUNKNOWN_ENCODING;
#endif
#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
static VALUE symSKIPPED_ENTITY;
#endif

/* destructor */
static void
XMLParser_free(XMLParser* parser)
{
  /*  fprintf(stderr, "Delete XMLParser: %p->%p\n", parser, parser->parser);*/
  if (parser->parser) {
    XML_ParserFree(parser->parser);
    parser->parser = NULL;
  }
  free(parser);
}

static void
XMLParser_mark(XMLParser* parser)
{
  /*  fprintf(stderr, "Mark XMLParser: %p->%p\n", parser, parser->parser);*/
  if (!NIL_P(parser->parent)) {
    XMLParser* parent;
    GET_PARSER(parser->parent, parent);
    rb_gc_mark(parser->parent);
  }
}

static void
taintParser(XMLParser* parser) {
  parser->tainted |= 1;
  if (!NIL_P(parser->parent) && !parser->context) {
    XMLParser* parent;
    GET_PARSER(parser->parent, parent);
    taintParser(parent);
  }
}

inline static VALUE
taintObject(XMLParser* parser, VALUE obj) {
  if (parser->tainted)
    OBJ_TAINT(obj);
  return obj;
}
#define TO_(o) (taintObject(parser, o))

inline static VALUE
freezeObject(VALUE obj) {
  OBJ_FREEZE(obj);
  return obj;
}
#define FO_(o) (freezeObject(o))


/* Event handlers for iterator */
static void
iterStartElementHandler(void *recv,
			const XML_Char *name, const XML_Char **atts)
{
  XMLParser* parser;
  VALUE attrhash;

  GET_PARSER(recv, parser);
#ifdef NEW_EXPAT
  parser->lastAttrs = atts;
#endif
  attrhash = rb_hash_new();
  while (*atts) {
    const char* key = *atts++;
    const char* val = *atts++;
    rb_hash_aset(attrhash,
		 FO_(TO_(rb_str_new2((char*)key))),
		 TO_(rb_str_new2((char*)val)));
  }
  
  rb_yield(rb_ary_new3(4, symSTART_ELEM,
		       TO_(rb_str_new2((char*)name)), attrhash, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterEndElementHandler(void *recv,
		      const XML_Char *name)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symEND_ELEM,
		       TO_(rb_str_new2((char*)name)), Qnil, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterCharacterDataHandler(void *recv,
			 const XML_Char *s,
		       int len)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symCDATA,
		       Qnil, TO_(rb_str_new((char*)s, len)), recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterProcessingInstructionHandler(void *recv,
				 const XML_Char *target,
				 const XML_Char *data)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symPI,
		       TO_(rb_str_new2((char*)target)),
		       TO_(rb_str_new2((char*)data)), recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterDefaultHandler(void *recv,
		   const XML_Char *s,
		   int len)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symDEFAULT,
		       Qnil, TO_(rb_str_new((char*)s, len)), recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    /* XML_DefaultCurrent shoould not call in defaultHandler */
    /* XML_DefaultCurrent(parser->parser); */
  }
}

void
iterUnparsedEntityDeclHandler(void *recv,
			      const XML_Char *entityName,
			      const XML_Char *base,
			      const XML_Char *systemId,
			      const XML_Char *publicId,
			      const XML_Char *notationName)
{
  XMLParser* parser;
  VALUE valary;

  GET_PARSER(recv, parser);
  valary = rb_ary_new3(4, (base ? TO_(rb_str_new2((char*)base)) : Qnil),
		       TO_(rb_str_new2((char*)systemId)),
		       (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil),
		       TO_(rb_str_new2((char*)notationName)));
  rb_yield(rb_ary_new3(4, symUNPARSED_ENTITY_DECL,
		       TO_(rb_str_new2((char*)entityName)),
		       valary, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

void
iterNotationDeclHandler(void *recv,
			const XML_Char *notationName,
			const XML_Char *base,
			const XML_Char *systemId,
			const XML_Char *publicId)
{
  XMLParser* parser;
  VALUE valary;

  GET_PARSER(recv, parser);
  valary = rb_ary_new3(3,
		       (base ? TO_(rb_str_new2((char*)base)) : Qnil),
		       (systemId ? TO_(rb_str_new2((char*)systemId)) : Qnil),
		       (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil));
  rb_yield(rb_ary_new3(4, symNOTATION_DECL,
		       TO_(rb_str_new2((char*)notationName)),
		       valary, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

int
iterExternalEntityRefHandler(XML_Parser xmlparser,
			     const XML_Char *context,
			     const XML_Char *base,
			     const XML_Char *systemId,
			     const XML_Char *publicId)
{
  XMLParser* parser;
  VALUE recv;
  VALUE valary;
  VALUE ret;

  recv = (VALUE)XML_GetUserData(xmlparser);
  GET_PARSER(recv, parser);
  valary = rb_ary_new3(3,
		       (base ? TO_(rb_str_new2((char*)base)) : Qnil),
		       (systemId ? TO_(rb_str_new2((char*)systemId)) : Qnil),
		       (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil));
  ret = rb_yield(rb_ary_new3(4, symEXTERNAL_ENTITY_REF,
			     (context ? TO_(rb_str_new2((char*)context)) : Qnil),
			     valary, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
  /* The error status in this iterator block should be returned
     by the exception. */
  return 1;
}

#ifdef NEW_EXPAT
static void
iterCommentHandler(void *recv,
		   const XML_Char *s)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symCOMMENT,
		       Qnil, TO_(rb_str_new2((char*)s)), recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterStartCdataSectionHandler(void *recv)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symSTART_CDATA, Qnil, Qnil, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterEndCdataSectionHandler(void *recv)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symEND_CDATA, Qnil, Qnil, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterStartNamespaceDeclHandler(void *recv,
			      const XML_Char *prefix,
			      const XML_Char *uri)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symSTART_NAMESPACE_DECL,
		       (prefix ? TO_(rb_str_new2((char*)prefix)) : Qnil),
		       (uri ? TO_(rb_str_new2((char*)uri)) : Qnil), recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterEndNamespaceDeclHandler(void *recv,
			    const XML_Char *prefix)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symEND_NAMESPACE_DECL,
		       (prefix ? TO_(rb_str_new2((char*)prefix)) : Qnil),
		       Qnil, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}
#endif

#ifdef HAVE_XML_SETPARAMENTITYPARSING
static void
#ifdef HAVE_EXPAT_H
iterStartDoctypeDeclHandler(void *recv,
			    const XML_Char *doctypeName,
			    const XML_Char *sysid,
			    const XML_Char *pubid,
			    int has_internal_subset)
#else
iterStartDoctypeDeclHandler(void *recv,
			    const XML_Char *doctypeName)
#endif
{
  XMLParser* parser;
  VALUE valary = Qnil;

  GET_PARSER(recv, parser);
#ifdef HAVE_EXPAT_H
  valary = rb_ary_new3(3,
		       (sysid ? TO_(rb_str_new2((char*)sysid)) : Qnil),
		       (pubid ? TO_(rb_str_new2((char*)pubid)) : Qnil),
		       (has_internal_subset ? Qtrue : Qfalse));
#endif
  rb_yield(rb_ary_new3(4, symSTART_DOCTYPE_DECL,
		       TO_(rb_str_new2((char*)doctypeName)),
		       valary, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterEndDoctypeDeclHandler(void *recv)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symEND_DOCTYPE_DECL,
		       Qnil,
		       Qnil, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}
#endif


#ifdef HAVE_EXPAT_H

static VALUE
makeContentArray(XMLParser* parser, XML_Content* model)
{
  static const char* content_type_name[] = {
    NULL, "EMPTY", "ANY", "MIXED", "NAME", "CHOICE", "SEQ"
  };
  static const char* content_quant_name[] = {
    "", "?", "*", "+"
  };
  int i;
  VALUE children = Qnil;
  const char* type_name = content_type_name[model->type];
  const char* quant_name = content_quant_name[model->quant];
  VALUE ret = rb_ary_new3(3,
			  TO_(rb_str_new2((char*)type_name)),
			  TO_(rb_str_new2((char*)quant_name)),
			  (model->name ? TO_(rb_str_new2((char*)model->name)) :
			   Qnil));
  if (model->numchildren > 0) {
    children = rb_ary_new();
    for (i =0; i < model->numchildren; i++) {
      VALUE child = makeContentArray(parser, model->children + i);
      rb_ary_push(children, child);
    }
  }
  rb_ary_push(ret, children);
  return ret;
}



static void
iterElementDeclHandler(void *recv,
		       const XML_Char *name,
		       XML_Content *model)
{
  XMLParser* parser;
  VALUE content;
  GET_PARSER(recv, parser);
  content = makeContentArray(parser, model);
  rb_yield(rb_ary_new3(4, symELEMENT_DECL,
		       TO_(rb_str_new2(name)),
		       content, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterAttlistDeclHandler (void *recv,
			const XML_Char *elname,
			const XML_Char *attname,
			const XML_Char *att_type,
			const XML_Char *dflt,
			int isrequired)
{
  XMLParser* parser;
  VALUE valary;

  GET_PARSER(recv, parser);
  valary = rb_ary_new3(4,
		       TO_(rb_str_new2((char*)attname)),
		       TO_(rb_str_new2((char*)att_type)),
		       (dflt ? TO_(rb_str_new2((char*)dflt)) : Qnil),
		       (isrequired ? Qtrue : Qfalse));
  rb_yield(rb_ary_new3(4, symATTLIST_DECL,
		       TO_(rb_str_new2(elname)),
		       valary, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterXmlDeclHandler (void *recv,
		    const XML_Char *version,
		    const XML_Char *encoding,
		    int standalone)
{
  XMLParser* parser;
  VALUE valary;

  GET_PARSER(recv, parser);
  valary = rb_ary_new3(3,
		       (version ? TO_(rb_str_new2(version)) : Qnil),
		       (encoding ? TO_(rb_str_new2((char*)encoding)) : Qnil),
		       INT2FIX(standalone));
  rb_yield(rb_ary_new3(4, symXML_DECL,
		       Qnil,
		       valary, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterEntityDeclHandler (void *recv,
		       const XML_Char *entityName,
		       int is_parameter_entity,
		       const XML_Char *value,
		       int value_length,
		       const XML_Char *base,
		       const XML_Char *systemId,
		       const XML_Char *publicId,
		       const XML_Char *notationName)
{
  XMLParser* parser;
  VALUE valary;

  GET_PARSER(recv, parser);
  valary = rb_ary_new3(6,
		       (is_parameter_entity ? Qtrue : Qfalse),
		       TO_(rb_str_new((char*)value, value_length)),
		       (base ? TO_(rb_str_new2((char*)base)) : Qnil),
		       (systemId ? TO_(rb_str_new2((char*)systemId)) : Qnil),
		       (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil),
		       (notationName ? TO_(rb_str_new2((char*)notationName))
			: Qnil));
  rb_yield(rb_ary_new3(4, symENTITY_DECL,
		       TO_(rb_str_new2(entityName)),
		       valary, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

#endif

#if 0
static void
iterExternalParsedEntityDeclHandler(void *recv,
				    const XML_Char *entityName,
				    const XML_Char *base,
				    const XML_Char *systemId,
				    const XML_Char *publicId)
{
  XMLParser* parser;
  VALUE valary;

  GET_PARSER(recv, parser);
  valary = rb_ary_new3(3, (base ? TO_(rb_str_new2((char*)base)) : Qnil),
		       TO_(rb_str_new2((char*)systemId)),
		       (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil));
  rb_yield(rb_ary_new3(4, symEXTERNAL_PARSED_ENTITY_DECL,
		       TO_(rb_str_new2((char*)entityName)),
		       valary, recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}

static void
iterInternalParsedEntityDeclHandler(void *recv,
				    const XML_Char *entityName,
				    const XML_Char *replacementText,
				    int replacementTextLength)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symINTERNAL_PARSED_ENTITY_DECL,
		       TO_(rb_str_new2((char*)entityName)),
		       TO_(rb_str_new((char*)replacementText,
				      replacementTextLength)), recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}
#endif

#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
static void
iterSkippedEntityHandler(void *recv,
			 const XML_Char *entityName,
			 int is_parameter_entity)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_yield(rb_ary_new3(4, symSKIPPED_ENTITY,
		       TO_(rb_str_new2((char*)entityName)),
		       INT2FIX(is_parameter_entity), recv));
  if (parser->defaultCurrent) {
    parser->defaultCurrent = 0;
    XML_DefaultCurrent(parser->parser);
  }
}
#endif



/* Event handlers for instance method */
static void
myStartElementHandler(void *recv,
		      const XML_Char *name, const XML_Char **atts)
{
  XMLParser* parser;
  VALUE attrhash;

  GET_PARSER(recv, parser);
#ifdef NEW_EXPAT
  parser->lastAttrs = atts;
#endif
  attrhash = rb_hash_new();
  while (*atts) {
    const char* key = *atts++;
    const char* val = *atts++;
    rb_hash_aset(attrhash,
		 FO_(TO_(rb_str_new2((char*)key))),
		 TO_(rb_str_new2((char*)val)));
  }
  rb_funcall((VALUE)recv, id_startElementHandler, 2,
	     TO_(rb_str_new2((char*)name)), attrhash);
}

static void
myEndElementHandler(void *recv,
		    const XML_Char *name)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_endElementHandler, 1,
	     TO_(rb_str_new2((char*)name)));
}

static void
myCharacterDataHandler(void *recv,
		       const XML_Char *s,
		       int len)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_characterDataHandler, 1,
	     TO_(rb_str_new((char*)s, len)));
}

static void
myProcessingInstructionHandler(void *recv,
			       const XML_Char *target,
			       const XML_Char *data)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_processingInstructionHandler, 2,
	     TO_(rb_str_new2((char*)target)),
	     TO_(rb_str_new2((char*)data)));
}

static void
myDefaultHandler(void *recv,
		 const XML_Char *s,
		 int len)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_defaultHandler, 1,
	     TO_(rb_str_new((char*)s, len)));
}

#ifdef NEW_EXPAT
static void
myDefaultExpandHandler(void *recv,
		 const XML_Char *s,
		 int len)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_defaultExpandHandler, 1,
	     TO_(rb_str_new((char*)s, len)));
}
#endif

void
myUnparsedEntityDeclHandler(void *recv,
			    const XML_Char *entityName,
			    const XML_Char *base,
			    const XML_Char *systemId,
			    const XML_Char *publicId,
			    const XML_Char *notationName)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_unparsedEntityDeclHandler, 5,
	     TO_(rb_str_new2((char*)entityName)),
	     (base ? TO_(rb_str_new2((char*)base)) : Qnil),
	     TO_(rb_str_new2((char*)systemId)),
	     (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil),
	     TO_(rb_str_new2((char*)notationName)));
}

void
myNotationDeclHandler(void *recv,
		      const XML_Char *notationName,
		      const XML_Char *base,
		      const XML_Char *systemId,
		      const XML_Char *publicId)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_notationDeclHandler, 4,
	     TO_(rb_str_new2((char*)notationName)),
	     (base ? TO_(rb_str_new2((char*)base)) : Qnil),
	     (systemId ? TO_(rb_str_new2((char*)systemId)) : Qnil),
	     (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil));
}

int
myExternalEntityRefHandler(XML_Parser xmlparser,
			   const XML_Char *context,
			   const XML_Char *base,
			   const XML_Char *systemId,
			   const XML_Char *publicId)
{
  XMLParser* parser;
  VALUE recv;
  VALUE ret;

  recv = (VALUE)XML_GetUserData(xmlparser);
  GET_PARSER(recv, parser);
  ret = rb_funcall(recv, id_externalEntityRefHandler, 4,
		   (context ? TO_(rb_str_new2((char*)context)): Qnil),
		   (base ? TO_(rb_str_new2((char*)base)) : Qnil),
		   (systemId ? TO_(rb_str_new2((char*)systemId)) : Qnil),
		   (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil));
  /* The error status in this handler should be returned
     by the exception. */
  return Qnil;
}

#ifdef NEW_EXPAT
static void
myCommentHandler(void *recv,
		 const XML_Char *s)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_commentHandler, 1,
	     TO_(rb_str_new2((char*)s)));
}

static void
myStartCdataSectionHandler(void *recv)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_startCdataSectionHandler, 0);
}

static void
myEndCdataSectionHandler(void *recv)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_endCdataSectionHandler, 0);
}

static void
myStartNamespaceDeclHandler(void *recv,
			    const XML_Char *prefix,
			    const XML_Char *uri)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_startNamespaceDeclHandler, 2,
	     (prefix ? TO_(rb_str_new2((char*)prefix)) : Qnil),
	     (uri ? TO_(rb_str_new2((char*)uri)) : Qnil));
}

static void
myEndNamespaceDeclHandler(void *recv,
			  const XML_Char *prefix)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_endNamespaceDeclHandler, 1,
	     (prefix ? TO_(rb_str_new2((char*)prefix)) : Qnil));
}

static int
myNotStandaloneHandler(void *recv)
{
  XMLParser* parser;
  VALUE v;

  GET_PARSER(recv, parser);
  v = rb_funcall((VALUE)recv, id_notStandaloneHandler, 0);
  Check_Type(v, T_FIXNUM);
  return FIX2INT(v);
}
#endif

#ifdef HAVE_XML_SETPARAMENTITYPARSING
static void
#ifdef HAVE_EXPAT_H
myStartDoctypeDeclHandler(void *recv,
			  const XML_Char *doctypeName,
			  const XML_Char *sysid,
			  const XML_Char *pubid,
			  int has_internal_subset)
#else
myStartDoctypeDeclHandler(void *recv,
			  const XML_Char *doctypeName)
#endif
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
#ifdef HAVE_EXPAT_H
  rb_funcall((VALUE)recv, id_startDoctypeDeclHandler, 4,
	     TO_(rb_str_new2((char*)doctypeName)),
	     (sysid ? TO_(rb_str_new2((char*)sysid)) : Qnil),
	     (pubid ? TO_(rb_str_new2((char*)pubid)) : Qnil),
	     (has_internal_subset ? Qtrue : Qfalse));
#else
  rb_funcall((VALUE)recv, id_startDoctypeDeclHandler, 4,
	     TO_(rb_str_new2((char*)doctypeName)),
	     Qnil, Qnil, Qfalse);
#endif
}

static void
myEndDoctypeDeclHandler(void *recv)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_endDoctypeDeclHandler, 0);
}
#endif


#ifdef HAVE_EXPAT_H

static void
myElementDeclHandler(void *recv,
		     const XML_Char *name,
		     XML_Content *model)
{
  XMLParser* parser;
  VALUE content;
  GET_PARSER(recv, parser);
  content = makeContentArray(parser, model);
  rb_funcall((VALUE)recv, id_elementDeclHandler, 2,
	     TO_(rb_str_new2(name)), content);
}

static void
myAttlistDeclHandler (void *recv,
		      const XML_Char *elname,
		      const XML_Char *attname,
		      const XML_Char *att_type,
		      const XML_Char *dflt,
		      int isrequired)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_attlistDeclHandler, 5,
	     TO_(rb_str_new2(elname)),
	     TO_(rb_str_new2((char*)attname)),
	     TO_(rb_str_new2((char*)att_type)),
	     (dflt ? TO_(rb_str_new2((char*)dflt)) : Qnil),
	     (isrequired ? Qtrue : Qfalse));
}

static void
myXmlDeclHandler (void *recv,
		  const XML_Char *version,
		  const XML_Char *encoding,
		  int standalone)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_xmlDeclHandler, 3,
	     (version ? TO_(rb_str_new2(version)) : Qnil),
	     (encoding ? TO_(rb_str_new2((char*)encoding)) : Qnil),
	     INT2FIX(standalone));
}

static void
myEntityDeclHandler (void *recv,
		     const XML_Char *entityName,
		     int is_parameter_entity,
		     const XML_Char *value,
		     int value_length,
		     const XML_Char *base,
		     const XML_Char *systemId,
		     const XML_Char *publicId,
		     const XML_Char *notationName)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_entityDeclHandler, 7,
	     TO_(rb_str_new2(entityName)),
	     (is_parameter_entity ? Qtrue : Qfalse),
	     TO_(rb_str_new((char*)value, value_length)),
	     (base ? TO_(rb_str_new2((char*)base)) : Qnil),
	     (systemId ? TO_(rb_str_new2((char*)systemId)) : Qnil),
	     (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil),
	     (notationName ? TO_(rb_str_new2((char*)notationName))
	      : Qnil));
}

#endif

#if 0
static void
myExternalParsedEntityDeclHandler(void *recv,
				  const XML_Char *entityName,
				  const XML_Char *base,
				  const XML_Char *systemId,
				  const XML_Char *publicId)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_externalParsedEntityDeclHandler, 4,
	     TO_(rb_str_new2((char*)entityName)),
	     (base ? TO_(rb_str_new2((char*)base)) : Qnil),
	     TO_(rb_str_new2((char*)systemId)),
	     (publicId ? TO_(rb_str_new2((char*)publicId)) : Qnil));
}

static void
myInternalParsedEntityDeclHandler(void *recv,
				  const XML_Char *entityName,
				  const XML_Char *replacementText,
				  int replacementTextLength)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_internalParsedEntityDeclHandler, 2,
	     TO_(rb_str_new2((char*)entityName)),
	     TO_(rb_str_new((char*)replacementText,
			    replacementTextLength)));
}
#endif


static VALUE
XMLEncoding_map(VALUE obj, VALUE i)
{
  return i;
}

static VALUE
XMLEncoding_convert(VALUE obj, VALUE str)
{
  return INT2FIX('?');
}

static int
myEncodingConv(void *data, const char *s)
{
  VALUE v;
  int len;
  int slen;

  v = rb_ivar_get((VALUE)data, id_map);
  slen = RSTRING_PTR(v)[*(unsigned char*)s];

  v = rb_funcall((VALUE)data, id_convert, 1, rb_str_new((char*)s, -slen));
  switch (TYPE(v)) {
  case T_FIXNUM:
    return FIX2INT(v);
  case T_STRING:
    len = RSTRING_LEN(v);
    if (len == 1) {
      return (unsigned char)*(RSTRING_PTR(v));
    }
    else if (len >= 2) {
      return (unsigned char)*(RSTRING_PTR(v)) |
	(unsigned char)*(RSTRING_PTR(v) + 1) << 8;
    }
  }
  return 0;
}

#if 0
static int
iterUnknownEncodingHandler(void *recv,
			   const XML_Char *name,
			   XML_Encoding *info)
{
  XMLParser* parser;
  VALUE ret;

  if (!rb_method_boundp(CLASS_OF((VALUE)recv), id_unknownEncoding, 0))
    return 0;

  GET_PARSER(recv, parser);
  ret = rb_yield(rb_ary_new3(4, symUNKNOWN_ENCODING,
			     TO_(rb_str_new2((char*)name)), Qnil, recv));
  if (TYPE(ret) == T_OBJECT && rb_obj_is_kind_of(ret, cXMLEncoding)) {
    int i;
    ID mid = rb_intern("map");
    VALUE cmap = rb_str_new(NULL, 256);
    rb_ivar_set(ret, id_map, cmap);

    for (i = 0; i < 256; i++) {
      VALUE m = rb_funcall(ret, mid, 1, INT2FIX(i));
      RSTRING(cmap)->ptr[i] = info->map[i] = FIX2INT(m);
    }
    /* protect object form GC */
    rb_ivar_set(recv, rb_intern("_encoding"), ret);
    info->data = (void*)ret;
    info->convert = myEncodingConv;
    return 1;
  }

  return 0;
}
#endif

#ifdef XML_ENC_PATH
/*
 * Encoding map functions come from XML::Parser Version 2.19
 *
 * Copyright 1998 Larry Wall and Clark Cooper
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the same terms as Perl itself.
 */
static Encinfo*
getEncinfo(char* data, int size)
{
  Encmap_Header* header = (Encmap_Header*)data;
  unsigned short prefixes_size;
  unsigned short bytemap_size;
  Encinfo* ret;
  int i;
  PrefixMap* prefixes;
  unsigned short *bytemap;

  if (size < sizeof(Encmap_Header) || ntohl(header->magic) != ENCMAP_MAGIC)
    return NULL;
  prefixes_size = ntohs(header->pfsize);
  bytemap_size = ntohs(header->bmsize);
  if (size != (sizeof(Encmap_Header) +
	       prefixes_size * sizeof(PrefixMap) +
	       bytemap_size * sizeof(unsigned short)))
    return NULL;
  if ((ret = (Encinfo*)malloc(sizeof(Encinfo))) == NULL) {
    return NULL;
  }
  ret->prefixes_size = prefixes_size;
  ret->bytemap_size = bytemap_size;
  for (i = 0; i < 256; i++)
    ret->firstmap[i] = ntohl(header->map[i]);
  prefixes = (PrefixMap*)(data + sizeof(Encmap_Header));
  bytemap = (unsigned short*)(data + sizeof(Encmap_Header)
			      + sizeof(PrefixMap)*prefixes_size);
  if ((ret->prefixes =
       (PrefixMap*)malloc(sizeof(PrefixMap)*prefixes_size)) == NULL) {
    free(ret);
    return NULL;
  }
  if ((ret->bytemap =
       (unsigned short*)malloc(sizeof(unsigned short)*bytemap_size)) == NULL) {
    free(ret->prefixes);
    free(ret);
    return NULL;
  }
  for (i = 0; i < prefixes_size; i++, prefixes++) {
    ret->prefixes[i].min = prefixes->min;
    ret->prefixes[i].len = prefixes->len;
    ret->prefixes[i].bmap_start = ntohs(prefixes->bmap_start);
    memcpy(ret->prefixes[i].ispfx, prefixes->ispfx,
	   sizeof(prefixes->ispfx) + sizeof(prefixes->ischar));
  }
  for (i = 0; i < bytemap_size; i++)
    ret->bytemap[i] = ntohs(bytemap[i]);

  return ret;
}

static int
convertEncoding(Encinfo* enc, const char* seq)
{
  PrefixMap* curpfx;
  int count;
  int index = 0;

  for (count = 0; count < 4; count++) {
    unsigned char byte = (unsigned char)seq[count];
    unsigned char bndx;
    unsigned char bmsk;
    int offset;

    curpfx = &enc->prefixes[index];
    offset = ((int)byte) - curpfx->min;
    if (offset < 0)
      break;
    if (offset >= curpfx->len && curpfx->len != 0)
      break;

    bndx = byte >> 3;
    bmsk = 1 << (byte & 0x7);

    if (curpfx->ispfx[bndx] & bmsk) {
      index = enc->bytemap[curpfx->bmap_start + offset];
    }
    else if (curpfx->ischar[bndx] & bmsk) {
      return enc->bytemap[curpfx->bmap_start + offset];
    }
    else
      break;
  }

  return -1;
}

static void
releaseEncoding(Encinfo* enc)
{
  if (enc) {
    if (enc->prefixes)
      free(enc->prefixes);
    if (enc->bytemap)
      free(enc->bytemap);
    free(enc);
  }
}

static Encinfo*
findEncoding(const char* encname)
{
  FILE* fp;
  Encinfo* enc;
  struct stat st;
  int size;
  int len;
  char file[PATH_MAX] = "\0";
  const char* p;
  char* buf;
#ifdef DOSISH
  const char sepchar = '\\';
#else
  const char sepchar = '/';
#endif
  const char* const encext = ".enc";

  rb_secure(2);
  /* make map file path */
  if (XML_ENC_PATH != NULL) {
    strncpy(file, XML_ENC_PATH, PATH_MAX - 1);
    file[PATH_MAX - 1] = '\0';
  }
  len = strlen(file);
  if (len > 0 && len < PATH_MAX - 1 && file[len - 1] != sepchar)
    file[len++] = sepchar;
  for (p = encname; *p && len < PATH_MAX - 1; p++, len++) {
    file[len] = tolower(*p);
  }
  file[len] = '\0';
  strncat(file, encext, PATH_MAX - len -1);

  if ((fp = fopen(file, "rb")) == NULL) {
    return NULL;
  }

  /* get file length */
  fstat(fileno(fp), &st);
  size = st.st_size;

  if ((buf = (char*)malloc(size)) == NULL) {
    fclose(fp);
    return NULL;
  }

  fread(buf, 1, size, fp);
  fclose(fp);
  enc = getEncinfo(buf, size);
  free(buf);
  return enc;
}

#endif

static int
myUnknownEncodingHandler(void *recv,
			 const XML_Char *name,
			 XML_Encoding *info)
{
  XMLParser* parser;
  VALUE ret;
  if (!rb_method_boundp(CLASS_OF((VALUE)recv), id_unknownEncoding, 0))
#ifndef XML_ENC_PATH
    return 0;
#else
  {
    Encinfo* enc;

    if ((enc = findEncoding(name)) != NULL) {
      memcpy(info->map, enc->firstmap, sizeof(int)*256);
      info->data = enc;
      info->convert = (int(*)(void*,const char*))convertEncoding;
      info->release = (void(*)(void*))releaseEncoding;
      return 1;
    }
    else
      return 0;
  }
#endif

  GET_PARSER(recv, parser);
  ret = rb_funcall((VALUE)recv, id_unknownEncoding, 1,
		   TO_(rb_str_new2((char*)name)));
  if (TYPE(ret) == T_OBJECT && rb_obj_is_kind_of(ret, cXMLEncoding)) {
    int i;
    ID mid = rb_intern("map");
    VALUE cmap = rb_str_new(NULL, 256);
    rb_ivar_set(ret, id_map, cmap);

    if (OBJ_TAINTED(ret))
      taintParser(parser);
    TO_(cmap);

    for (i = 0; i < 256; i++) {
      VALUE m = rb_funcall(ret, mid, 1, INT2FIX(i));
      RSTRING_PTR(cmap)[i] = info->map[i] = FIX2INT(m);
    }
    /* protect object form GC */
    rb_ivar_set((VALUE)recv, rb_intern("_encoding"), ret);
    info->data = (void*)ret;
    info->convert = myEncodingConv;

    return 1;
  }

  return 0;
}

#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
static void
mySkippedEntityHandler(void *recv,
		       const XML_Char *entityName,
		       int is_parameter_entity)
{
  XMLParser* parser;
  GET_PARSER(recv, parser);
  rb_funcall((VALUE)recv, id_skippedEntityHandler, 2,
	     TO_(rb_str_new2((char*)entityName)),
	     INT2FIX(is_parameter_entity));
}
#endif


/* constructor */
static VALUE
XMLParser_new(int argc, VALUE* argv, VALUE klass)
{
  XMLParser* parser;
  VALUE obj;
  VALUE arg1;
  VALUE arg2;
  VALUE arg3;
  int count;
  char* encoding = NULL;
#ifdef NEW_EXPAT
  char* nssep = NULL;
#endif
  char* context = NULL;
  XMLParser* rootparser = NULL;
  VALUE parent = Qnil;

  count = rb_scan_args(argc, argv, "03", &arg1, &arg2, &arg3);
  if (count == 1) {
    /* new(encoding) */
    if (TYPE(arg1) != T_NIL) {
      Check_Type(arg1, T_STRING); /* encoding */
      encoding = RSTRING_PTR(arg1);
    }
  }
  else if (count == 2) {
    /* new(encoding, nschar) */
    /* new(parser, context) */
#ifdef NEW_EXPAT
    if (TYPE(arg1) != T_DATA) {
      if (TYPE(arg1) != T_NIL) {
	Check_Type(arg1, T_STRING); /* encoding */
	encoding = RSTRING_PTR(arg1);
      }
      Check_Type(arg2, T_STRING); /* nschar */
      nssep = RSTRING_PTR(arg2);
    }
    else {
#endif
      Check_Type(arg1, T_DATA); /* parser */
      GET_PARSER(arg1, rootparser);
      if (!NIL_P(arg2)) {
	Check_Type(arg2, T_STRING); /* context */
	context = RSTRING_PTR(arg2);
      }
      parent = arg1;
#ifdef NEW_EXPAT
    }
#endif
  }
  else if (count == 3) {
    /* new(parser, context, encoding) */
    Check_Type(arg1, T_DATA); /* parser */
    GET_PARSER(arg1, rootparser);
    if (!NIL_P(arg2)) {
      Check_Type(arg2, T_STRING); /* context */
      context = RSTRING_PTR(arg2);
    }
    Check_Type(arg3, T_STRING); /* encoding */
    encoding = RSTRING_PTR(arg3);
    parent = arg1;
  }

  /* create object */
  obj = Data_Make_Struct(klass, XMLParser,
			 XMLParser_mark, XMLParser_free, parser);
  /* create parser */
  if (rootparser == NULL) {
#ifdef NEW_EXPAT
    if (nssep == NULL)
      parser->parser = XML_ParserCreate(encoding);
    else
      parser->parser = XML_ParserCreateNS(encoding, nssep[0]);
#else
    parser->parser = XML_ParserCreate(encoding);
#endif
    parser->tainted = 0;
    parser->context = NULL;
  }
  else {
    parser->parser = XML_ExternalEntityParserCreate(rootparser->parser,
    						    context, encoding);
    /* clear all inhrited handlers,
       because handlers should be set in "parse" method  */
    XML_SetElementHandler(parser->parser, NULL, NULL);
    XML_SetCharacterDataHandler(parser->parser, NULL);
    XML_SetProcessingInstructionHandler(parser->parser, NULL);
    XML_SetDefaultHandler(parser->parser, NULL);
    XML_SetUnparsedEntityDeclHandler(parser->parser, NULL);
    XML_SetNotationDeclHandler(parser->parser, NULL);
    XML_SetExternalEntityRefHandler(parser->parser, NULL);
#ifdef NEW_EXPAT
    XML_SetCommentHandler(parser->parser, NULL);
    XML_SetCdataSectionHandler(parser->parser, NULL, NULL);
    XML_SetNamespaceDeclHandler(parser->parser, NULL, NULL);
    XML_SetNotStandaloneHandler(parser->parser, NULL);
#endif
#ifdef HAVE_XML_SETDOCTYPEDECLHANDLER
    XML_SetDoctypeDeclHandler(parser->parser, NULL, NULL);
#endif
#ifdef HAVE_EXPAT_H
    XML_SetElementDeclHandler(parser->parser, NULL);
    XML_SetAttlistDeclHandler(parser->parser, NULL);
    XML_SetXmlDeclHandler(parser->parser, NULL);
    XML_SetEntityDeclHandler(parser->parser, NULL);
#endif
#if 0
    XML_SetExternalParsedEntityDeclHandler(parser->parser, NULL);
    XML_SetInternalParsedEntityDeclHandler(parser->parser, NULL);
#endif
#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
    XML_SetSkippedEntityHandler(parser->parser, NULL);
#endif
    if (rootparser->tainted)
      parser->tainted |= 1;
    parser->context = context;
  }
  if (!parser->parser)
    rb_raise(eXMLParserError, "cannot create parser");

  /* setting up internal data */
  XML_SetUserData(parser->parser, (void*)obj);
  parser->iterator = 0;
  parser->defaultCurrent = 0;
#ifdef NEW_EXPAT
  parser->lastAttrs = NULL;
#endif
  parser->parent = parent;

  rb_obj_call_init(obj, argc, argv);

  return obj;
}

static VALUE
XMLParser_initialize(VALUE obj)
{
  return Qnil;
}

#ifdef HAVE_XML_PARSERRESET
static VALUE
XMLParser_reset(int argc, VALUE* argv, VALUE obj)
{
  XMLParser* parser;
  VALUE vencoding = Qnil;
  char* encoding = NULL;
  int count;

  count = rb_scan_args(argc, argv, "01", &vencoding);

  GET_PARSER(obj, parser);
  if (count > 0 && TYPE(vencoding) != T_NIL) {
    Check_Type(vencoding, T_STRING);
    encoding = RSTRING_PTR(vencoding);
  }
  XML_ParserReset(parser->parser, encoding);
  /* setting up internal data */
  XML_SetUserData(parser->parser, (void*)obj);
  parser->iterator = 0;
  parser->defaultCurrent = 0;
#ifdef NEW_EXPAT
  parser->lastAttrs = NULL;
#endif
  parser->tainted = 0;

  return obj;
}
#endif

/* parse method */
static VALUE
XMLParser_parse(int argc, VALUE* argv, VALUE obj)
{
  XMLParser* parser;
  int ret;
  XML_StartElementHandler start = NULL;
  XML_EndElementHandler   end = NULL;
#ifdef NEW_EXPAT
  XML_StartCdataSectionHandler startC = NULL;
  XML_EndCdataSectionHandler   endC = NULL;
  XML_StartNamespaceDeclHandler startNS = NULL;
  XML_EndNamespaceDeclHandler endNS = NULL;
#endif
#ifdef HAVE_XML_SETDOCTYPEDECLHANDLER
  XML_StartDoctypeDeclHandler startDoctype = NULL;
  XML_EndDoctypeDeclHandler endDoctype = NULL;
#endif
  VALUE str;
  VALUE isFinal;
  int final = 1;
  int count;
  int fromStream = 0;
  ID mid = rb_intern("gets");
  ID linebuf = rb_intern("_linebuf");

  count = rb_scan_args(argc, argv, "02", &str, &isFinal);
  /* If "str" has public "gets" method, it will be considered *stream* */
  if (!rb_obj_is_kind_of(str, rb_cString) &&
      rb_method_boundp(CLASS_OF(str), mid, 1)) {
    fromStream = 1;
  }
  else if (!NIL_P(str)) {
    Check_Type(str, T_STRING);
  }
  if (count >= 2) {
    if (isFinal == Qtrue)
      final = 1;
    else if (isFinal == Qfalse)
      final = 0;
    else
      rb_raise(rb_eTypeError, "not valid value");
  }

  GET_PARSER(obj, parser);

  parser->iterator = rb_block_given_p();

  /* Setup event handlers */

  /* Call as iterator */
  if (parser->iterator) {
    XML_SetElementHandler(parser->parser,
			  iterStartElementHandler, iterEndElementHandler);
    XML_SetCharacterDataHandler(parser->parser,
				iterCharacterDataHandler);
    XML_SetProcessingInstructionHandler(parser->parser,
					iterProcessingInstructionHandler);
    /* check dummy default handler */
#ifdef NEW_EXPAT
    if (rb_method_boundp(CLASS_OF(obj), id_defaultExpandHandler, 0))
      XML_SetDefaultHandlerExpand(parser->parser, iterDefaultHandler);
    else
#endif
    if (rb_method_boundp(CLASS_OF(obj), id_defaultHandler, 0))
      XML_SetDefaultHandler(parser->parser, iterDefaultHandler);

    if (rb_method_boundp(CLASS_OF(obj), id_unparsedEntityDeclHandler, 0))
      XML_SetUnparsedEntityDeclHandler(parser->parser,
				       iterUnparsedEntityDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_notationDeclHandler, 0))
      XML_SetNotationDeclHandler(parser->parser,
				 iterNotationDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_externalEntityRefHandler, 0))
      XML_SetExternalEntityRefHandler(parser->parser,
				      iterExternalEntityRefHandler);
#ifdef NEW_EXPAT
    if (rb_method_boundp(CLASS_OF(obj), id_commentHandler, 0))
      XML_SetCommentHandler(parser->parser, iterCommentHandler);

    if (rb_method_boundp(CLASS_OF(obj), id_startCdataSectionHandler, 0))
      startC = iterStartCdataSectionHandler;
    if (rb_method_boundp(CLASS_OF(obj), id_endCdataSectionHandler, 0))
      endC = iterEndCdataSectionHandler;
    if (startC || endC)
      XML_SetCdataSectionHandler(parser->parser, startC, endC);

    if (rb_method_boundp(CLASS_OF(obj), id_startNamespaceDeclHandler, 0))
      startNS = iterStartNamespaceDeclHandler;
    if (rb_method_boundp(CLASS_OF(obj), id_endNamespaceDeclHandler, 0))
      endNS = iterEndNamespaceDeclHandler;
    if (startNS || endNS)
      XML_SetNamespaceDeclHandler(parser->parser, startNS, endNS);
    if (rb_method_boundp(CLASS_OF(obj), id_notStandaloneHandler, 0))
      XML_SetNotStandaloneHandler(parser->parser, myNotStandaloneHandler);
#endif
#ifdef HAVE_XML_SETDOCTYPEDECLHANDLER
    if (rb_method_boundp(CLASS_OF(obj), id_startDoctypeDeclHandler, 0))
      startDoctype = iterStartDoctypeDeclHandler;
    if (rb_method_boundp(CLASS_OF(obj), id_endDoctypeDeclHandler, 0))
      endDoctype = iterEndDoctypeDeclHandler;
    if (startDoctype || endDoctype)
      XML_SetDoctypeDeclHandler(parser->parser, startDoctype, endDoctype);
#endif
#ifdef HAVE_EXPAT_H
    if (rb_method_boundp(CLASS_OF(obj), id_elementDeclHandler, 0))
      XML_SetElementDeclHandler(parser->parser, iterElementDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_attlistDeclHandler, 0))
      XML_SetAttlistDeclHandler(parser->parser, iterAttlistDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_xmlDeclHandler, 0))
      XML_SetXmlDeclHandler(parser->parser, iterXmlDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_entityDeclHandler, 0))
      XML_SetEntityDeclHandler(parser->parser, iterEntityDeclHandler);
#endif
#if 0
    if (rb_method_boundp(CLASS_OF(obj), id_externalParsedEntityDeclHandler, 0))
      XML_SetExternalParsedEntityDeclHandler(parser->parser,
					     iterExternalParsedEntityDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_internalParsedEntityDeclHandler, 0))
      XML_SetInternalParsedEntityDeclHandler(parser->parser,
					     iterInternalParsedEntityDeclHandler);
#endif
    /* Call non-iterator version of UnknownEncoding handler,
       because the porcedure block often returns the unexpected value. */
    XML_SetUnknownEncodingHandler(parser->parser,
				  myUnknownEncodingHandler,
				  (void*)obj);
#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
    if (rb_method_boundp(CLASS_OF(obj), id_skippedEntityHandler, 0))
      XML_SetSkippedEntityHandler(parser->parser, iterSkippedEntityHandler);
#endif
  }
  /* Call as not iterator */
  else {
    if (rb_method_boundp(CLASS_OF(obj), id_startElementHandler, 0))
      start = myStartElementHandler;
    if (rb_method_boundp(CLASS_OF(obj), id_endElementHandler, 0))
      end = myEndElementHandler;
    if (start || end)
      XML_SetElementHandler(parser->parser, start, end);
    if (rb_method_boundp(CLASS_OF(obj), id_characterDataHandler, 0))
      XML_SetCharacterDataHandler(parser->parser,
				  myCharacterDataHandler);
    if (rb_method_boundp(CLASS_OF(obj),
			 id_processingInstructionHandler, 0))
      XML_SetProcessingInstructionHandler(parser->parser,
					  myProcessingInstructionHandler);
#ifdef NEW_EXPAT
    if (rb_method_boundp(CLASS_OF(obj), id_defaultExpandHandler, 0))
      XML_SetDefaultHandlerExpand(parser->parser, myDefaultExpandHandler);
    else
#endif
    if (rb_method_boundp(CLASS_OF(obj), id_defaultHandler, 0)) {
      XML_SetDefaultHandler(parser->parser, myDefaultHandler);
    }
    if (rb_method_boundp(CLASS_OF(obj), id_unparsedEntityDeclHandler, 0))
      XML_SetUnparsedEntityDeclHandler(parser->parser,
				     myUnparsedEntityDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_notationDeclHandler, 0))
      XML_SetNotationDeclHandler(parser->parser,
			       myNotationDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_externalEntityRefHandler, 0))
      XML_SetExternalEntityRefHandler(parser->parser,
				      myExternalEntityRefHandler);
#ifdef NEW_EXPAT
    if (rb_method_boundp(CLASS_OF(obj), id_commentHandler, 0))
      XML_SetCommentHandler(parser->parser, myCommentHandler);

    if (rb_method_boundp(CLASS_OF(obj), id_startCdataSectionHandler, 0))
      startC = myStartCdataSectionHandler;
    if (rb_method_boundp(CLASS_OF(obj), id_endCdataSectionHandler, 0))
      endC = myEndCdataSectionHandler;
    if (startC || endC)
      XML_SetCdataSectionHandler(parser->parser, startC, endC);

    if (rb_method_boundp(CLASS_OF(obj), id_startNamespaceDeclHandler, 0))
      startNS = myStartNamespaceDeclHandler;
    if (rb_method_boundp(CLASS_OF(obj), id_endNamespaceDeclHandler, 0))
      endNS = myEndNamespaceDeclHandler;
    if (startNS || endNS)
      XML_SetNamespaceDeclHandler(parser->parser, startNS, endNS);
    if (rb_method_boundp(CLASS_OF(obj), id_notStandaloneHandler, 0))
      XML_SetNotStandaloneHandler(parser->parser, myNotStandaloneHandler);
#endif
#ifdef HAVE_XML_SETDOCTYPEDECLHANDLER
    if (rb_method_boundp(CLASS_OF(obj), id_startDoctypeDeclHandler, 0))
      startDoctype = myStartDoctypeDeclHandler;
    if (rb_method_boundp(CLASS_OF(obj), id_endDoctypeDeclHandler, 0))
      endDoctype = myEndDoctypeDeclHandler;
    if (startDoctype || endDoctype)
      XML_SetDoctypeDeclHandler(parser->parser, startDoctype, endDoctype);
#endif
#ifdef HAVE_EXPAT_H
    if (rb_method_boundp(CLASS_OF(obj), id_elementDeclHandler, 0))
      XML_SetElementDeclHandler(parser->parser, myElementDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_attlistDeclHandler, 0))
      XML_SetAttlistDeclHandler(parser->parser, myAttlistDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_xmlDeclHandler, 0))
      XML_SetXmlDeclHandler(parser->parser, myXmlDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_entityDeclHandler, 0))
      XML_SetEntityDeclHandler(parser->parser, myEntityDeclHandler);
#endif
#if 0
    if (rb_method_boundp(CLASS_OF(obj), id_externalParsedEntityDeclHandler, 0))
      XML_SetExternalParsedEntityDeclHandler(parser->parser,
					     myExternalParsedEntityDeclHandler);
    if (rb_method_boundp(CLASS_OF(obj), id_internalParsedEntityDeclHandler, 0))
      XML_SetInternalParsedEntityDeclHandler(parser->parser,
					     myInternalParsedEntityDeclHandler);
#endif
    XML_SetUnknownEncodingHandler(parser->parser,
				  myUnknownEncodingHandler,
				  (void*)obj);
#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
    if (rb_method_boundp(CLASS_OF(obj), id_skippedEntityHandler, 0))
      XML_SetSkippedEntityHandler(parser->parser, mySkippedEntityHandler);
#endif
  }

  /* Parse from stream (probably slightly slow) */
  if (fromStream) {
    VALUE buf;

    if (OBJ_TAINTED(str))
      taintParser(parser);
    do {
      buf = rb_funcall(str, mid, 0);
      if (!NIL_P(buf)) {
	Check_Type(buf, T_STRING);
	if (OBJ_TAINTED(buf))
	  taintParser(parser);
	rb_ivar_set(obj, linebuf, buf); /* protect buf from GC (reasonable?)*/
	ret = XML_Parse(parser->parser,
			RSTRING_PTR(buf), RSTRING_LEN(buf), 0);
      }
      else {
	ret = XML_Parse(parser->parser, NULL, 0, 1);
      }
      if (!ret) {
	int err = XML_GetErrorCode(parser->parser);
	const char* errStr = XML_ErrorString(err);
	rb_raise(eXMLParserError, (char*)errStr);
      }
    } while (!NIL_P(buf));
    return Qnil;
  }

  /* Parse string */
  if (!NIL_P(str)) {
    if (OBJ_TAINTED(str))
      taintParser(parser);
    ret = XML_Parse(parser->parser,
		    RSTRING_PTR(str), RSTRING_LEN(str), final);
  }
  else
    ret = XML_Parse(parser->parser, NULL, 0, final);
  if (!ret) {
    int err = XML_GetErrorCode(parser->parser);
    const char* errStr = XML_ErrorString(err);
    rb_raise(eXMLParserError, (char*)errStr);
  }

  return Qnil;
}

/* done method */
static VALUE
XMLParser_done(VALUE obj)
{
  XMLParser* parser;

  GET_PARSER(obj, parser);
  if (parser->parser) {
    XML_ParserFree(parser->parser);
    parser->parser = NULL;
  }
  return Qnil;
}

/* defaultCurrent method */
static VALUE
XMLParser_defaultCurrent(VALUE obj)
{
  XMLParser* parser;

  GET_PARSER(obj, parser);
  if (!(parser->iterator)) {
    XML_DefaultCurrent(parser->parser);
  }
  else {
    parser->defaultCurrent = 1;
  }
  return Qnil;
}

/* line method */
static VALUE
XMLParser_getCurrentLineNumber(VALUE obj)
{
  XMLParser* parser;
  int line;

  GET_PARSER(obj, parser);
  line = XML_GetCurrentLineNumber(parser->parser);

  return INT2FIX(line);
}

/* column method */
static VALUE
XMLParser_getCurrentColumnNumber(VALUE obj)
{
  XMLParser* parser;
  int column;

  GET_PARSER(obj, parser);
  column = XML_GetCurrentColumnNumber(parser->parser);

  return INT2FIX(column);
}

/* byte index method */
static VALUE
XMLParser_getCurrentByteIndex(VALUE obj)
{
  XMLParser* parser;
  long pos;

  GET_PARSER(obj, parser);
  pos = XML_GetCurrentByteIndex(parser->parser);

  return INT2FIX(pos);
}

/* set URI base */
static VALUE
XMLParser_setBase(VALUE obj, VALUE base)
{
  XMLParser* parser;
  int ret;

  Check_Type(base, T_STRING);
  GET_PARSER(obj, parser);
  if (OBJ_TAINTED(base))
    taintParser(parser);
  ret = XML_SetBase(parser->parser, RSTRING_PTR(base));

  return INT2FIX(ret);
}

/* get URI base */
static VALUE
XMLParser_getBase(VALUE obj)
{
  XMLParser* parser;
  const XML_Char* ret;

  GET_PARSER(obj, parser);
  ret = XML_GetBase(parser->parser);
  if (!ret)
    return Qnil;

  return TO_(rb_str_new2((char*)ret));
}

#ifdef NEW_EXPAT
#if 0
static VALUE
XMLParser_getSpecifiedAttributes(VALUE obj)
{
  XMLParser* parser;
  int count;
  const XML_Char** atts;
  VALUE attrhash;

  GET_PARSER(obj, parser);
  atts = parser->lastAttrs;
  if (!atts)
    return Qnil;
  count = XML_GetSpecifiedAttributeCount(parser->parser)/2;
  attrhash = rb_hash_new();
  while (*atts) {
    const char* key = *atts++;
    atts++;
    rb_hash_aset(attrhash, FO_(TO_(rb_str_new2((char*)key))),
		 (count-- > 0) ? Qtrue: Qfalse);
  }

  return attrhash;
}
#else
static VALUE
XMLParser_getSpecifiedAttributes(VALUE obj)
{
  XMLParser* parser;
  int i, count;
  const XML_Char** atts;
  VALUE attrarray;

  GET_PARSER(obj, parser);
  atts = parser->lastAttrs;
  if (!atts)
    return Qnil;
  count = XML_GetSpecifiedAttributeCount(parser->parser)/2;
  attrarray = rb_ary_new2(count);
  for (i = 0; i < count; i++, atts+=2) {
    const char* key = *atts;
    rb_ary_push(attrarray, TO_(rb_str_new2((char*)key)));
  }

  return attrarray;
}
#endif

static VALUE
XMLParser_getCurrentByteCount(VALUE obj)
{
  XMLParser* parser;

  GET_PARSER(obj, parser);
  return INT2FIX(XML_GetCurrentByteCount(parser->parser));
}
#endif

#ifdef XML_DTD
static VALUE
XMLParser_setParamEntityParsing(VALUE obj, VALUE parsing)
{
  XMLParser* parser;
  int ret;

  Check_Type(parsing, T_FIXNUM);
  GET_PARSER(obj, parser);
  ret = XML_SetParamEntityParsing(parser->parser, FIX2INT(parsing));

  return INT2FIX(ret);
}
#endif

static VALUE
XMLParser_s_expatVersion(VALUE obj)
{
#if defined(HAVE_EXPAT_H)
  return rb_str_new2(XML_ExpatVersion());
#elif defined(EXPAT_1_2)
  return rb_str_new2("1.2");
#elif defined(NEW_EXPAT)
  return rb_str_new2("1.1");
#else
  return rb_str_new2("1.0");
#endif
}

#ifdef HAVE_EXPAT_H
static VALUE
XMLParser_setReturnNSTriplet(VALUE obj, VALUE do_nst)
{
  XMLParser* parser;
  int nst;

  GET_PARSER(obj, parser);
  switch (TYPE(do_nst)) {
  case T_TRUE:
    nst = 1;
    break;
  case T_FALSE:
    nst = 0;
    break;
  case T_FIXNUM:
    nst = FIX2INT(do_nst);
    break;
  default:
    rb_raise(rb_eTypeError, "not valid value");
  }
  XML_SetReturnNSTriplet(parser->parser, nst);

  return Qnil;
}


static VALUE
XMLParser_getInputContext(VALUE obj)
{
  XMLParser* parser;
  const char* buffer;
  int offset;
  int size;
  VALUE ret = Qnil;

  GET_PARSER(obj, parser);
  buffer = XML_GetInputContext(parser->parser,
			       &offset,
			       &size);
  if (buffer && size > 0) {
    ret = rb_ary_new3(2,
		      TO_(rb_str_new(buffer, size)),
		      INT2FIX(offset));
  }

  return ret;
}


static VALUE
XMLParser_getIdAttrribute(VALUE obj)
{
  XMLParser* parser;
  int idattr;
  const XML_Char** atts;

  GET_PARSER(obj, parser);
  atts = parser->lastAttrs;
  if (!atts)
    return Qnil;
  idattr = XML_GetIdAttributeIndex(parser->parser);
  if (idattr < 0)
    return Qnil;
  return TO_(rb_str_new2((char*)atts[idattr]));
}
#endif

#ifdef HAVE_XML_USEFOREIGNDTD
static VALUE
XMLParser_useForeignDTD(VALUE obj, VALUE useDTD)
{
  XMLParser* parser;
  int dtd;
  int ret;

  GET_PARSER(obj, parser);
  switch (TYPE(useDTD)) {
  case T_TRUE:
    dtd = 1;
    break;
  case T_FALSE:
    dtd = 0;
    break;
  case T_FIXNUM:
    dtd = FIX2INT(useDTD);
    break;
  default:
    rb_raise(rb_eTypeError, "not valid value");
  }
  ret = XML_UseForeignDTD(parser->parser, dtd);

  return INT2FIX(ret);
}
#endif

#ifdef HAVE_XML_GETFEATURELIST
static VALUE
XMLParser_s_getFeatureList(VALUE obj)
{
  const XML_Feature* list;
  VALUE ret = rb_hash_new();

  list = XML_GetFeatureList();
  while (list && list->feature) {
    rb_hash_aset(ret, FO_(rb_str_new2(list->name)), INT2NUM(list->value));
    list++;
  }

  return ret;
}
#endif

void
Init_xmlparser()
{
  VALUE mXML;

  eXMLParserError = rb_define_class("XMLParserError", rb_eStandardError);
  cXMLParser = rb_define_class("XMLParser", rb_cObject);
  cXMLEncoding = rb_define_class("XMLEncoding", rb_cObject);

  /* Class name aliases */
  if (rb_const_defined(rb_cObject, rb_intern("XML")) == Qtrue)
    mXML = rb_const_get(rb_cObject, rb_intern("XML"));
  else
    mXML = rb_define_module("XML");
  rb_define_const(mXML, "ParserError", eXMLParserError);
  rb_define_const(cXMLParser, "Error", eXMLParserError);
  rb_define_const(mXML, "Parser", cXMLParser);
  rb_define_const(mXML, "Encoding", cXMLEncoding);

  rb_define_singleton_method(cXMLParser, "new", XMLParser_new, -1);
  rb_define_singleton_method(cXMLParser, "expatVersion",
			     XMLParser_s_expatVersion, 0);
  rb_define_method(cXMLParser, "initialize", XMLParser_initialize, -1);
  rb_define_method(cXMLParser, "parse", XMLParser_parse, -1);
  rb_define_method(cXMLParser, "done", XMLParser_done, 0);
  rb_define_method(cXMLParser, "defaultCurrent",  XMLParser_defaultCurrent, 0);
  rb_define_method(cXMLParser, "line", XMLParser_getCurrentLineNumber, 0);
  rb_define_method(cXMLParser, "column", XMLParser_getCurrentColumnNumber, 0);
  rb_define_method(cXMLParser, "byteIndex", XMLParser_getCurrentByteIndex, 0);
  rb_define_method(cXMLParser, "setBase", XMLParser_setBase, 1);
  rb_define_method(cXMLParser, "getBase", XMLParser_getBase, 0);
#ifdef NEW_EXPAT
  rb_define_method(cXMLParser, "getSpecifiedAttributes",
		   XMLParser_getSpecifiedAttributes, 0);
  rb_define_method(cXMLParser, "byteCount", XMLParser_getCurrentByteCount, 0);
#endif
#ifdef XML_DTD
  rb_define_method(cXMLParser, "setParamEntityParsing",
		   XMLParser_setParamEntityParsing, 1);
#endif
#ifdef HAVE_EXPAT_H
  rb_define_method(cXMLParser, "setReturnNSTriplet",
		   XMLParser_setReturnNSTriplet, 1);
  rb_define_method(cXMLParser, "getInputContext",
		   XMLParser_getInputContext, 0);
  rb_define_method(cXMLParser, "getIdAttribute",
		   XMLParser_getIdAttrribute, 0);
#endif

#ifdef HAVE_XML_PARSERRESET
  rb_define_method(cXMLParser, "reset", XMLParser_reset, -1);
#endif

  rb_define_method(cXMLEncoding, "map", XMLEncoding_map, 1);
  rb_define_method(cXMLEncoding, "convert", XMLEncoding_convert, 1);

#ifdef HAVE_XML_USEFOREIGNDTD
  rb_define_method(cXMLParser, "useForeignDTD",
		   XMLParser_useForeignDTD, 1);
#endif
#ifdef HAVE_XML_GETFEATURELIST
  rb_define_singleton_method(cXMLParser, "getFeatureList",
			     XMLParser_s_getFeatureList, 0);
#endif

#define DEFINE_EVENT_CODE(klass, name) \
  rb_define_const(klass, #name, sym##name = ID2SYM(rb_intern(#name)))

  DEFINE_EVENT_CODE(cXMLParser, START_ELEM);
  DEFINE_EVENT_CODE(cXMLParser, END_ELEM);
  DEFINE_EVENT_CODE(cXMLParser, CDATA);
  DEFINE_EVENT_CODE(cXMLParser, PI);
  DEFINE_EVENT_CODE(cXMLParser, DEFAULT);
  DEFINE_EVENT_CODE(cXMLParser, UNPARSED_ENTITY_DECL);
  DEFINE_EVENT_CODE(cXMLParser, NOTATION_DECL);
  DEFINE_EVENT_CODE(cXMLParser, EXTERNAL_ENTITY_REF);
#ifdef NEW_EXPAT
  DEFINE_EVENT_CODE(cXMLParser, COMMENT);
  DEFINE_EVENT_CODE(cXMLParser, START_CDATA);
  DEFINE_EVENT_CODE(cXMLParser, END_CDATA);
  DEFINE_EVENT_CODE(cXMLParser, START_NAMESPACE_DECL);
  DEFINE_EVENT_CODE(cXMLParser, END_NAMESPACE_DECL);
#endif
#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
  DEFINE_EVENT_CODE(cXMLParser, SKIPPED_ENTITY);
#endif
#ifdef XML_DTD
  rb_define_const(cXMLParser, "PARAM_ENTITY_PARSING_NEVER",
		  XML_PARAM_ENTITY_PARSING_NEVER);
  rb_define_const(cXMLParser, "PARAM_ENTITY_PARSING_UNLESS_STANDALONE",
		  XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
  rb_define_const(cXMLParser, "PARAM_ENTITY_PARSING_ALWAYS",
		  XML_PARAM_ENTITY_PARSING_ALWAYS);
#endif
#ifdef HAVE_XML_SETDOCTYPEDECLHANDLER
  DEFINE_EVENT_CODE(cXMLParser, START_DOCTYPE_DECL);
  DEFINE_EVENT_CODE(cXMLParser, END_DOCTYPE_DECL);
#endif
#ifdef HAVE_EXPAT_H
  DEFINE_EVENT_CODE(cXMLParser, ELEMENT_DECL);
  DEFINE_EVENT_CODE(cXMLParser, ATTLIST_DECL);
  DEFINE_EVENT_CODE(cXMLParser, XML_DECL);
  DEFINE_EVENT_CODE(cXMLParser, ENTITY_DECL);
#endif
#if 0
  DEFINE_EVENT_CODE(cXMLParser, EXTERNAL_PARSED_ENTITY_DECL);
  DEFINE_EVENT_CODE(cXMLParser, INTERNAL_PARSED_ENTITY_DECL);
#endif
#if 0
  DEFINE_EVENT_CODE(cXMLParser, UNKNOWN_ENCODING);
#endif

  id_map = rb_intern("_map");
  id_startElementHandler = rb_intern("startElement");
  id_endElementHandler = rb_intern("endElement");
  id_characterDataHandler = rb_intern("character");
  id_processingInstructionHandler = rb_intern("processingInstruction");
  id_defaultHandler = rb_intern("default");
  id_unparsedEntityDeclHandler = rb_intern("unparsedEntityDecl");
  id_notationDeclHandler = rb_intern("notationDecl");
  id_externalEntityRefHandler = rb_intern("externalEntityRef");
#ifdef NEW_EXPAT
  id_defaultExpandHandler = rb_intern("defaultExpand");
  id_commentHandler = rb_intern("comment");
  id_startCdataSectionHandler = rb_intern("startCdata");
  id_endCdataSectionHandler = rb_intern("endCdata");
  id_startNamespaceDeclHandler = rb_intern("startNamespaceDecl");
  id_endNamespaceDeclHandler = rb_intern("endNamespaceDecl");
  id_notStandaloneHandler = rb_intern("notStandalone");
#endif
#ifdef HAVE_XML_SETDOCTYPEDECLHANDLER
  id_startDoctypeDeclHandler = rb_intern("startDoctypeDecl");
  id_endDoctypeDeclHandler = rb_intern("endDoctypeDecl");
#endif
  id_unknownEncoding = rb_intern("unknownEncoding");
  id_convert = rb_intern("convert");
#ifdef HAVE_EXPAT_H
  id_elementDeclHandler = rb_intern("elementDecl");
  id_attlistDeclHandler = rb_intern("attlistDecl");
  id_xmlDeclHandler = rb_intern("xmlDecl");
  id_entityDeclHandler = rb_intern("entityDecl");
#endif
#if 0
  id_externalParsedEntityDeclHandler = rb_intern("externalParsedEntityDecl");
  id_internalParsedEntityDeclHandler = rb_intern("internalParsedEntityDecl");
#endif
#ifdef HAVE_XML_SETSKIPPEDENTITYHANDLER
  id_skippedEntityHandler = rb_intern("skippedEntity");
#endif
}
