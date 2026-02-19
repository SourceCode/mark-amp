#include "BoardSerializerModel.h"

#include <algorithm>

namespace markamp::canvas
{

void BoardSerializerModel::set_schema_version(int version)
{
    schema_version_ = std::max(1, version);
}
auto BoardSerializerModel::schema_version() const -> int
{
    return schema_version_;
}

void BoardSerializerModel::add_migration(SchemaMigration migration)
{
    migrations_.push_back(std::move(migration));
}
auto BoardSerializerModel::migrations() const -> const std::vector<SchemaMigration>&
{
    return migrations_;
}

auto BoardSerializerModel::needs_migration(int file_version) const -> bool
{
    return file_version < schema_version_;
}

void BoardSerializerModel::set_load_status(LoadStatus status)
{
    load_status_ = status;
}
auto BoardSerializerModel::load_status() const -> LoadStatus
{
    return load_status_;
}

void BoardSerializerModel::set_recovered_count(int count)
{
    recovered_count_ = std::max(0, count);
}
auto BoardSerializerModel::recovered_count() const -> int
{
    return recovered_count_;
}

void BoardSerializerModel::begin_save()
{
    is_saving_ = true;
    save_succeeded_ = false;
}

void BoardSerializerModel::commit_save()
{
    is_saving_ = false;
    save_succeeded_ = true;
}

void BoardSerializerModel::rollback_save()
{
    is_saving_ = false;
    save_succeeded_ = false;
}

auto BoardSerializerModel::is_saving() const -> bool
{
    return is_saving_;
}
auto BoardSerializerModel::save_succeeded() const -> bool
{
    return save_succeeded_;
}

void BoardSerializerModel::add_diagnostic(SerializerDiagnostic diag)
{
    diagnostics_.push_back(std::move(diag));
}
void BoardSerializerModel::clear_diagnostics()
{
    diagnostics_.clear();
}
auto BoardSerializerModel::diagnostics() const -> const std::vector<SerializerDiagnostic>&
{
    return diagnostics_;
}

auto BoardSerializerModel::error_count() const -> int
{
    return static_cast<int>(std::count_if(diagnostics_.begin(),
                                          diagnostics_.end(),
                                          [](const SerializerDiagnostic& diag)
                                          { return !diag.is_warning; }));
}

} // namespace markamp::canvas
