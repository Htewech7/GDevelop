/*
 * GDevelop Core
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights
 * reserved. This project is released under the MIT License.
 */
#include "InstructionMetadata.h"
#include <algorithm>
#include "GDCore/CommonTools.h"
#include "GDCore/Serialization/SerializerElement.h"
#include "GDCore/Tools/Localization.h"

namespace gd {
InstructionMetadata::InstructionMetadata()
    : sentence("Unknown or unsupported instruction"),  // Avoid translating this
                                                       // string, so that it's
                                                       // safe and *fast* to use
                                                       // a InstructionMetadata.
      canHaveSubInstructions(false),
      hidden(true),
      usageComplexity(5),
      isPrivate(false),
      isObjectInstruction(false),
      isBehaviorInstruction(false) {}

InstructionMetadata::InstructionMetadata(const gd::String& extensionNamespace_,
                                         const gd::String& name_,
                                         const gd::String& fullname_,
                                         const gd::String& description_,
                                         const gd::String& sentence_,
                                         const gd::String& group_,
                                         const gd::String& icon_,
                                         const gd::String& smallIcon_)
    : fullname(fullname_),
      description(description_),
      helpPath(""),
      sentence(sentence_),
      group(group_),
      iconFilename(icon_),
      smallIconFilename(smallIcon_),
      canHaveSubInstructions(false),
      extensionNamespace(extensionNamespace_),
      hidden(false),
      usageComplexity(5),
      isPrivate(false),
      isObjectInstruction(false),
      isBehaviorInstruction(false) {}

ParameterMetadata::ParameterMetadata() : optional(false), codeOnly(false) {}

InstructionMetadata& InstructionMetadata::AddParameter(
    const gd::String& type,
    const gd::String& description,
    const gd::String& optionalObjectType,
    bool parameterIsOptional) {
  ParameterMetadata info;
  info.type = type;
  info.description = description;
  info.codeOnly = false;
  info.optional = parameterIsOptional;
  info.supplementaryInformation =
      // For objects/behavior, the supplementary information
      // parameter is an object/behavior type...
      (gd::ParameterMetadata::IsObject(type) ||
       gd::ParameterMetadata::IsBehavior(type))
          ? (optionalObjectType.empty()
                 ? ""
                 : extensionNamespace +
                       optionalObjectType  //... so prefix it with the extension
                                           // namespace.
             )
          : optionalObjectType;  // Otherwise don't change anything

  // TODO: Assert against optionalObjectType === "emsc" (when running with
  // Emscripten), and warn about a missing argument when calling addParameter.

  parameters.push_back(info);
  return *this;
}

InstructionMetadata& InstructionMetadata::AddCodeOnlyParameter(
    const gd::String& type, const gd::String& supplementaryInformation) {
  ParameterMetadata info;
  info.type = type;
  info.codeOnly = true;
  info.supplementaryInformation = supplementaryInformation;

  parameters.push_back(info);
  return *this;
}

InstructionMetadata& InstructionMetadata::UseStandardOperatorParameters(
    const gd::String& type) {
  SetManipulatedType(type);

  AddParameter("operator", _("Modification's sign"));
  AddParameter(type == "number" ? "expression" : type, _("Value"));
  size_t operatorParamIndex = parameters.size() - 2;
  size_t valueParamIndex = parameters.size() - 1;

  if (isObjectInstruction || isBehaviorInstruction) {
    gd::String templateSentence =
        _("Change <subject> of _PARAM0_: <operator> <value>");

    sentence =
        templateSentence.FindAndReplace("<subject>", sentence)
            .FindAndReplace(
                "<operator>",
                "_PARAM" + gd::String::From(operatorParamIndex) + "_")
            .FindAndReplace("<value>",
                            "_PARAM" + gd::String::From(valueParamIndex) + "_");
  } else {
    gd::String templateSentence = _("Change <subject>: <operator> <value>");

    sentence =
        templateSentence.FindAndReplace("<subject>", sentence)
            .FindAndReplace(
                "<operator>",
                "_PARAM" + gd::String::From(operatorParamIndex) + "_")
            .FindAndReplace("<value>",
                            "_PARAM" + gd::String::From(valueParamIndex) + "_");
  }

  return *this;
}

InstructionMetadata&
InstructionMetadata::UseStandardRelationalOperatorParameters(
    const gd::String& type) {
  SetManipulatedType(type);

  AddParameter("relationalOperator", _("Sign of the test"));
  AddParameter(type == "number" ? "expression" : type, _("Value to compare"));
  size_t operatorParamIndex = parameters.size() - 2;
  size_t valueParamIndex = parameters.size() - 1;

  if (isObjectInstruction || isBehaviorInstruction) {
    gd::String templateSentence = _("<subject> of _PARAM0_ <operator> <value>");

    sentence =
        templateSentence.FindAndReplace("<subject>", sentence)
            .FindAndReplace(
                "<operator>",
                "_PARAM" + gd::String::From(operatorParamIndex) + "_")
            .FindAndReplace("<value>",
                            "_PARAM" + gd::String::From(valueParamIndex) + "_");
  } else {
    gd::String templateSentence = _("<subject> <operator> <value>");

    sentence =
        templateSentence.FindAndReplace("<subject>", sentence)
            .FindAndReplace(
                "<operator>",
                "_PARAM" + gd::String::From(operatorParamIndex) + "_")
            .FindAndReplace("<value>",
                            "_PARAM" + gd::String::From(valueParamIndex) + "_");
  }

  return *this;
}

void ParameterMetadata::SerializeTo(SerializerElement& element) const {
  element.SetAttribute("type", type);
  element.SetAttribute("supplementaryInformation", supplementaryInformation);
  element.SetAttribute("optional", optional);
  element.SetAttribute("description", description);
  element.SetAttribute("longDescription", longDescription);
  element.SetAttribute("codeOnly", codeOnly);
  element.SetAttribute("defaultValue", defaultValue);
  element.SetAttribute("name", name);
}

void ParameterMetadata::UnserializeFrom(const SerializerElement& element) {
  type = element.GetStringAttribute("type");
  supplementaryInformation =
      element.GetStringAttribute("supplementaryInformation");
  optional = element.GetBoolAttribute("optional");
  description = element.GetStringAttribute("description");
  longDescription = element.GetStringAttribute("longDescription");
  codeOnly = element.GetBoolAttribute("codeOnly");
  defaultValue = element.GetStringAttribute("defaultValue");
  name = element.GetStringAttribute("name");
}
}  // namespace gd
