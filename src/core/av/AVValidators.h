#pragma once

#include "AVColumnType.h"
#include "AVTypes.h"
#include "AVValue.h"

#include <string>

namespace markamp::core::av
{

[[nodiscard]] auto validate_type_match(const AVValue& value) -> AVValidationResult;
[[nodiscard]] auto validate_text(const AVValueText& text, int max_length = 65536)
    -> AVValidationResult;
[[nodiscard]] auto validate_number(const AVValueNumber& number) -> AVValidationResult;
[[nodiscard]] auto validate_date(const AVValueDate& date) -> AVValidationResult;
[[nodiscard]] auto validate_url(const AVValueURL& url_val) -> AVValidationResult;
[[nodiscard]] auto validate_email(const AVValueEmail& email) -> AVValidationResult;
[[nodiscard]] auto validate_phone(const AVValuePhone& phone) -> AVValidationResult;
[[nodiscard]] auto validate_select(const AVValueSelect& select, const AVKey& key)
    -> AVValidationResult;
[[nodiscard]] auto validate_mselect(const AVValueMSelect& mselect, const AVKey& key)
    -> AVValidationResult;
[[nodiscard]] auto validate_masset(const AVValueMAsset& masset) -> AVValidationResult;
[[nodiscard]] auto validate_relation(const AVValueRelation& relation) -> AVValidationResult;

} // namespace markamp::core::av
