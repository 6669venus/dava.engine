#ifndef __QUICKED_CHANGE_PROPERTY_VALUE_COMMAND_H__
#define __QUICKED_CHANGE_PROPERTY_VALUE_COMMAND_H__

#include "FileSystem/VariantType.h"
#include "EditorSystems/EditorSystemsManager.h"

#include "Command/Command.h"

class PackageNode;
class ControlNode;
class AbstractProperty;

class ChangePropertyValueCommand : public DAVA::Command
{
public:
    ChangePropertyValueCommand(PackageNode* root, ControlNode* node, AbstractProperty* property, const DAVA::VariantType& newValue);
    ChangePropertyValueCommand(PackageNode* root, ControlNode* node, AbstractProperty* property, const DAVA::VariantType& newValue, const DAVA::VariantType& oldValue);
    ~ChangePropertyValueCommand() override = default;

    void Redo() override;
    void Undo() override;

private:
    DAVA::VariantType GetValueFromProperty(AbstractProperty* property);
    PackageNode* root = nullptr;
    ControlNode* node = nullptr;
    AbstractProperty* property = nullptr;
    DAVA::VariantType oldValue;
    DAVA::VariantType newValue;
};

#endif // __QUICKED_CHANGE_PROPERTY_VALUE_COMMAND_H__
