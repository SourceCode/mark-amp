#pragma once

#include "core/av/AVTypes.h"
#include "core/av/AVValue.h"

#include <memory>
#include <string>

namespace markamp::ui::av
{

/// Interface for cell renderers (stub).
class IAVCellRenderer
{
public:
    virtual ~IAVCellRenderer() = default;
    [[nodiscard]] virtual auto render(const markamp::core::av::AVValue& value,
                                      const markamp::core::av::AVKey& key) const -> std::string = 0;
};

/// Factory for creating cell renderers by type (stub).
class AVCellRendererFactory
{
public:
    AVCellRendererFactory() = default;
    [[nodiscard]] auto create(markamp::core::av::AVKeyType type) const
        -> std::unique_ptr<IAVCellRenderer>;
};

} // namespace markamp::ui::av
