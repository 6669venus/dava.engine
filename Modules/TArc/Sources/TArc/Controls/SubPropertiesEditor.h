#pragma once

#include "TArc/Controls/ControlProxy.h"
#include <Base/Vector.h>

#include <QWidget>

namespace DAVA
{
namespace TArc
{
class SubPropertiesEditor : public ControlProxyImpl<QWidget>
{
public:
    enum class Fields : uint32
    {
        Value,
        IsReadOnly,
        FieldCount
    };

    SubPropertiesEditor(const ControlDescriptorBuilder<Fields>& fields, DataWrappersProcessor* wrappersProcessor, Reflection model, QWidget* parent = nullptr);
    SubPropertiesEditor(const ControlDescriptorBuilder<Fields>& fields, ContextAccessor* accessor, Reflection model, QWidget* parent = nullptr);

protected:
    void UpdateControl(const ControlDescriptor& descriptor) override;
    void OnDataChanged(const DataWrapper& wrapper, const Vector<Any>& fields) override;

    template <typename T>
    void SetupControl(T* accessor);

    Reflection GetCopyModel(const DataContext* ctx);

private:
    Any valueCopy;
    Reflection copyModel;
    DataWrapper copyModelWrapper;
};

} // namespace TArc
} // namespace DAVA
