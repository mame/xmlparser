#! /usr/local/bin/ruby -Ke

require "xml/dom/core"
include XML::DOM

tree = Document.new(ProcessingInstruction.new("xml",
                                         "version='1.0' encoding='EUC-JP'"),
                    Comment.new("������"),
                    Element.new("Test", [
                                  Attr.new('attr1', "°��1"),
                                  Attr.new('attr2', "°��2")],
                                Element.new("para", nil, "����ˤ���")))
print tree.to_s, "\n"
tree.dump
