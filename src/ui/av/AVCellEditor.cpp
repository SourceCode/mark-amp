#include "AVCellEditor.h"

namespace markamp::ui::av
{

/// Default cell editor — stub, returns input as-is.
class DefaultCellEditor final : public IAVCellEditor
{
public:
    [[nodiscard]] auto edit(const markamp::core::av::AVValue& value,
                            const markamp::core::av::AVKey& /*key*/) const -> std::string override
    {
        return value.to_display_string();
    }
};

auto AVCellEditorFactory::create(markamp::core::av::AVKeyType /*type*/) const
    -> std::unique_ptr<IAVCellEditor>
{
    return std::make_unique<DefaultCellEditor>();
}

} // namespace markamp::ui::av
