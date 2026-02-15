#pragma once

#include "core/av/AVTypes.h"
#include "core/av/AVValue.h"

#include <memory>
#include <string>

namespace markamp::ui::av
{

/// Interface for cell editors (stub).
class IAVCellEditor
{
public:
    virtual ~IAVCellEditor() = default;
    [[nodiscard]] virtual auto edit(const markamp::core::av::AVValue& value,
                                    const markamp::core::av::AVKey& key) const -> std::string = 0;
};

/// Factory for creating cell editors by type (stub).
class AVCellEditorFactory
{
public:
    AVCellEditorFactory() = default;
    [[nodiscard]] auto create(markamp::core::av::AVKeyType type) const
        -> std::unique_ptr<IAVCellEditor>;
};

} // namespace markamp::ui::av
