# Copyright (c) 2018(-2026) STMicroelectronics.
# All rights reserved.
#
# This file is part of the TouchGFX 4.27.0 distribution.
#
# This software is licensed under terms that can be found in the LICENSE file in
# the root directory of this software component.
# If no LICENSE file comes with this software, it is provided AS-IS.
#
###############################################################################/
require 'nokogiri'
require 'fileutils'

class XMLWriter
  def write(file_name, xml_doc)
    # Convert newlines to "&#10;" - Nokogiri 1.14+ no longer preserves entity
    # references through native_content=, so we rebuild the node children with
    # explicit EntityReference nodes instead.
    xml_doc.xpath("//Text/Translation").each do |translation_node|
      text = translation_node.text
      next unless text.include?("\n")
      translation_node.children.remove
      parts = text.split("\n", -1)
      parts.each_with_index do |part, i|
        translation_node.add_child(Nokogiri::XML::Text.new(part, xml_doc))
        translation_node.add_child(Nokogiri::XML::EntityReference.new(xml_doc, '#10')) unless i == parts.length - 1
      end
    end

    # Create a formatter for nice output before writing to file
    content = xml_doc.to_xml
    content.gsub!('/>', ' />')

    if File.exist?(file_name)
      old_content = File.read(file_name)
      return if content == old_content
    end

    File.write(file_name, content)
  end
end
