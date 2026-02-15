#include "AVCellRenderer.h"

namespace markamp::ui::av
{

/// Default cell renderer — returns display string.
class DefaultCellRenderer final : public IAVCellRenderer
{
public:
    [[nodiscard]] auto render(const markamp::core::av::AVValue& value,
                              const markamp::core::av::AVKey& /*key*/) const -> std::string override
    {
        return value.to_display_string();
    }
};

auto AVCellRendererFactory::create(markamp::core::av::AVKeyType /*type*/) const
    -> std::unique_ptr<IAVCellRenderer>
{
    return std::make_unique<DefaultCellRenderer>();
}

} // namespace markamp::ui::av
