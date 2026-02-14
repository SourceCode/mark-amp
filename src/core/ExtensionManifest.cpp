#include "ExtensionManifest.h"

#include <nlohmann/json.hpp>

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace markamp::core
{

// ── ActivationEvent ──

auto ActivationEvent::parse(const std::string& raw_event) -> ActivationEvent
{
    ActivationEvent evt;
    evt.raw = raw_event;

    if (raw_event == "*")
    {
        evt.kind = ActivationEventKind::kStar;
        return evt;
    }
    if (raw_event == "onStartupFinished")
    {
        evt.kind = ActivationEventKind::kOnStartupFinished;
        return evt;
    }
    if (raw_event == "onUri")
    {
        evt.kind = ActivationEventKind::kOnUri;
        return evt;
    }

    // Pattern: "prefix:argument"
    const auto colon_pos = raw_event.find(':');
    if (colon_pos == std::string::npos)
    {
        evt.kind = ActivationEventKind::kUnknown;
        return evt;
    }

    const std::string prefix = raw_event.substr(0, colon_pos);
    evt.argument = raw_event.substr(colon_pos + 1);

    if (prefix == "onLanguage")
    {
        evt.kind = ActivationEventKind::kOnLanguage;
    }
    else if (prefix == "onCommand")
    {
        evt.kind = ActivationEventKind::kOnCommand;
    }
    else if (prefix == "onView")
    {
        evt.kind = ActivationEventKind::kOnView;
    }
    else if (prefix == "onFileSystem")
    {
        evt.kind = ActivationEventKind::kOnFileSystem;
    }
    else if (prefix == "onCustomEditor")
    {
        evt.kind = ActivationEventKind::kOnCustomEditor;
    }
    else
    {
        evt.kind = ActivationEventKind::kUnknown;
    }

    return evt;
}

// ── ExtensionIdentifier ──

ExtensionIdentifier::ExtensionIdentifier(const std::string& pub, const std::string& ext_name)
    : publisher_(pub)
    , name_(ext_name)
{
}

auto ExtensionIdentifier::from_string(const std::string& identifier_string) -> ExtensionIdentifier
{
    const auto dot_pos = identifier_string.find('.');
    if (dot_pos == std::string::npos || dot_pos == 0 || dot_pos == identifier_string.size() - 1)
    {
        throw std::runtime_error(
            "Invalid extension identifier format (expected 'publisher.name'): " +
            identifier_string);
    }

    return {identifier_string.substr(0, dot_pos), identifier_string.substr(dot_pos + 1)};
}

auto ExtensionIdentifier::equals(const ExtensionIdentifier& other) const -> bool
{
    return to_lower(publisher_) == to_lower(other.publisher_) &&
           to_lower(name_) == to_lower(other.name_);
}

auto ExtensionIdentifier::to_key() const -> std::string
{
    return to_lower(publisher_) + "." + to_lower(name_);
}

auto ExtensionIdentifier::to_string() const -> std::string
{
    return publisher_ + "." + name_;
}

auto ExtensionIdentifier::to_lower(const std::string& str) -> std::string
{
    std::string result = str;
    std::transform(result.begin(),
                   result.end(),
                   result.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
    return result;
}

// ── ManifestParser helpers ──

namespace
{

auto get_string(const nlohmann::json& obj, const std::string& field_key) -> std::string
{
    if (obj.contains(field_key) && obj[field_key].is_string())
    {
        return obj[field_key].get<std::string>();
    }
    return {};
}

auto get_string_array(const nlohmann::json& obj, const std::string& field_key)
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    if (obj.contains(field_key) && obj[field_key].is_array())
    {
        for (const auto& item : obj[field_key])
        {
            if (item.is_string())
            {
                result.push_back(item.get<std::string>());
            }
        }
    }
    return result;
}

auto parse_commands(const nlohmann::json& contributes) -> std::vector<ExtensionCommand>
{
    std::vector<ExtensionCommand> commands;
    if (!contributes.contains("commands") || !contributes["commands"].is_array())
    {
        return commands;
    }

    for (const auto& cmd : contributes["commands"])
    {
        ExtensionCommand ext_cmd;
        ext_cmd.command = get_string(cmd, "command");
        ext_cmd.title = get_string(cmd, "title");
        ext_cmd.category = get_string(cmd, "category");
        ext_cmd.icon = get_string(cmd, "icon");
        commands.push_back(std::move(ext_cmd));
    }
    return commands;
}

auto parse_keybindings(const nlohmann::json& contributes) -> std::vector<ExtensionKeybinding>
{
    std::vector<ExtensionKeybinding> keybindings;
    if (!contributes.contains("keybindings") || !contributes["keybindings"].is_array())
    {
        return keybindings;
    }

    for (const auto& binding : contributes["keybindings"])
    {
        ExtensionKeybinding ext_kb;
        ext_kb.command = get_string(binding, "command");
        ext_kb.key = get_string(binding, "key");
        ext_kb.mac = get_string(binding, "mac");
        ext_kb.when = get_string(binding, "when");
        keybindings.push_back(std::move(ext_kb));
    }
    return keybindings;
}

auto parse_languages(const nlohmann::json& contributes) -> std::vector<ExtensionLanguage>
{
    std::vector<ExtensionLanguage> languages;
    if (!contributes.contains("languages") || !contributes["languages"].is_array())
    {
        return languages;
    }

    for (const auto& lang : contributes["languages"])
    {
        ExtensionLanguage ext_lang;
        ext_lang.language_id = get_string(lang, "id");
        ext_lang.extensions = get_string_array(lang, "extensions");
        ext_lang.aliases = get_string_array(lang, "aliases");
        ext_lang.configuration = get_string(lang, "configuration");
        languages.push_back(std::move(ext_lang));
    }
    return languages;
}

auto parse_grammars(const nlohmann::json& contributes) -> std::vector<ExtensionGrammar>
{
    std::vector<ExtensionGrammar> grammars;
    if (!contributes.contains("grammars") || !contributes["grammars"].is_array())
    {
        return grammars;
    }

    for (const auto& grammar : contributes["grammars"])
    {
        ExtensionGrammar ext_grammar;
        ext_grammar.language = get_string(grammar, "language");
        ext_grammar.scope_name = get_string(grammar, "scopeName");
        ext_grammar.path = get_string(grammar, "path");
        grammars.push_back(std::move(ext_grammar));
    }
    return grammars;
}

auto parse_themes(const nlohmann::json& contributes) -> std::vector<ExtensionTheme>
{
    std::vector<ExtensionTheme> themes;
    if (!contributes.contains("themes") || !contributes["themes"].is_array())
    {
        return themes;
    }

    for (const auto& theme : contributes["themes"])
    {
        ExtensionTheme ext_theme;
        ext_theme.theme_id = get_string(theme, "id");
        ext_theme.label = get_string(theme, "label");
        ext_theme.ui_theme = get_string(theme, "uiTheme");
        ext_theme.path = get_string(theme, "path");
        themes.push_back(std::move(ext_theme));
    }
    return themes;
}

auto parse_snippets(const nlohmann::json& contributes) -> std::vector<ExtensionSnippet>
{
    std::vector<ExtensionSnippet> snippets;
    if (!contributes.contains("snippets") || !contributes["snippets"].is_array())
    {
        return snippets;
    }

    for (const auto& snippet : contributes["snippets"])
    {
        ExtensionSnippet ext_snippet;
        ext_snippet.language = get_string(snippet, "language");
        ext_snippet.path = get_string(snippet, "path");
        snippets.push_back(std::move(ext_snippet));
    }
    return snippets;
}

auto parse_configuration(const nlohmann::json& contributes) -> std::vector<ExtensionConfiguration>
{
    std::vector<ExtensionConfiguration> configs;

    if (!contributes.contains("configuration"))
    {
        return configs;
    }

    // "configuration" can be either an object or an array of objects
    auto parse_single_config = [](const nlohmann::json& config_obj) -> ExtensionConfiguration
    {
        ExtensionConfiguration ext_config;
        ext_config.title = get_string(config_obj, "title");

        if (config_obj.contains("properties") && config_obj["properties"].is_object())
        {
            for (auto it = config_obj["properties"].begin(); it != config_obj["properties"].end();
                 ++it)
            {
                ExtensionConfiguration::Property prop;
                prop.key = it.key();
                prop.type = get_string(it.value(), "type");
                prop.description = get_string(it.value(), "description");
                prop.default_value = get_string(it.value(), "default");
                prop.enum_values = get_string_array(it.value(), "enum");
                ext_config.properties.push_back(std::move(prop));
            }
        }

        return ext_config;
    };

    const auto& config_node = contributes["configuration"];
    if (config_node.is_array())
    {
        for (const auto& config_obj : config_node)
        {
            configs.push_back(parse_single_config(config_obj));
        }
    }
    else if (config_node.is_object())
    {
        configs.push_back(parse_single_config(config_node));
    }

    return configs;
}

auto get_int(const nlohmann::json& obj, const std::string& field_key, int default_val = 0) -> int
{
    if (obj.contains(field_key) && obj[field_key].is_number_integer())
    {
        return obj[field_key].get<int>();
    }
    return default_val;
}

auto parse_views_containers(const nlohmann::json& contributes)
    -> std::vector<ExtensionViewsContainer>
{
    std::vector<ExtensionViewsContainer> result;
    if (!contributes.contains("viewsContainers") || !contributes["viewsContainers"].is_object())
    {
        return result;
    }
    // viewsContainers is { "activitybar": [...], "panel": [...] }
    for (auto it = contributes["viewsContainers"].begin();
         it != contributes["viewsContainers"].end();
         ++it)
    {
        if (it.value().is_array())
        {
            for (const auto& vc : it.value())
            {
                ExtensionViewsContainer evc;
                evc.container_id = get_string(vc, "id");
                evc.title = get_string(vc, "title");
                evc.icon = get_string(vc, "icon");
                result.push_back(std::move(evc));
            }
        }
    }
    return result;
}

auto parse_views(const nlohmann::json& contributes) -> std::vector<ExtensionView>
{
    std::vector<ExtensionView> result;
    if (!contributes.contains("views") || !contributes["views"].is_object())
    {
        return result;
    }
    // views is { "containerID": [...] }
    for (auto it = contributes["views"].begin(); it != contributes["views"].end(); ++it)
    {
        if (it.value().is_array())
        {
            for (const auto& view : it.value())
            {
                ExtensionView ev;
                ev.view_id = get_string(view, "id");
                ev.name = get_string(view, "name");
                ev.when = get_string(view, "when");
                result.push_back(std::move(ev));
            }
        }
    }
    return result;
}

auto parse_colors(const nlohmann::json& contributes) -> std::vector<ExtensionColor>
{
    std::vector<ExtensionColor> result;
    if (!contributes.contains("colors") || !contributes["colors"].is_array())
    {
        return result;
    }
    for (const auto& color : contributes["colors"])
    {
        ExtensionColor ec;
        ec.color_id = get_string(color, "id");
        ec.description = get_string(color, "description");
        if (color.contains("defaults") && color["defaults"].is_object())
        {
            ec.defaults.dark = get_string(color["defaults"], "dark");
            ec.defaults.light = get_string(color["defaults"], "light");
            ec.defaults.high_contrast = get_string(color["defaults"], "highContrast");
        }
        result.push_back(std::move(ec));
    }
    return result;
}

auto parse_menus(const nlohmann::json& contributes) -> std::vector<ExtensionMenuItem>
{
    std::vector<ExtensionMenuItem> result;
    if (!contributes.contains("menus") || !contributes["menus"].is_object())
    {
        return result;
    }
    // menus is { "menuLocation": [ { command, when, group } ] }
    for (auto it = contributes["menus"].begin(); it != contributes["menus"].end(); ++it)
    {
        if (it.value().is_array())
        {
            for (const auto& menu_item : it.value())
            {
                ExtensionMenuItem emi;
                emi.command = get_string(menu_item, "command");
                emi.when = get_string(menu_item, "when");
                emi.group = get_string(menu_item, "group");
                result.push_back(std::move(emi));
            }
        }
    }
    return result;
}

auto parse_submenus(const nlohmann::json& contributes) -> std::vector<ExtensionSubmenu>
{
    std::vector<ExtensionSubmenu> result;
    if (!contributes.contains("submenus") || !contributes["submenus"].is_array())
    {
        return result;
    }
    for (const auto& sub : contributes["submenus"])
    {
        ExtensionSubmenu es;
        es.submenu_id = get_string(sub, "id");
        es.label = get_string(sub, "label");
        es.icon = get_string(sub, "icon");
        result.push_back(std::move(es));
    }
    return result;
}

auto parse_walkthroughs(const nlohmann::json& contributes) -> std::vector<ExtensionWalkthrough>
{
    std::vector<ExtensionWalkthrough> result;
    if (!contributes.contains("walkthroughs") || !contributes["walkthroughs"].is_array())
    {
        return result;
    }
    for (const auto& wt : contributes["walkthroughs"])
    {
        ExtensionWalkthrough ew;
        ew.walkthrough_id = get_string(wt, "id");
        ew.title = get_string(wt, "title");
        ew.description = get_string(wt, "description");
        ew.icon = get_string(wt, "icon");
        ew.when = get_string(wt, "when");

        if (wt.contains("steps") && wt["steps"].is_array())
        {
            for (const auto& step : wt["steps"])
            {
                ExtensionWalkthroughStep ws;
                ws.step_id = get_string(step, "id");
                ws.title = get_string(step, "title");
                ws.description = get_string(step, "description");
                ws.when = get_string(step, "when");
                ws.completion_events = get_string_array(step, "completionEvents");

                // Media can be object { image, markdown } or string
                if (step.contains("media") && step["media"].is_object())
                {
                    if (step["media"].contains("image"))
                    {
                        ws.media_path = get_string(step["media"], "image");
                        ws.media_type = "image";
                    }
                    else if (step["media"].contains("markdown"))
                    {
                        ws.media_path = get_string(step["media"], "markdown");
                        ws.media_type = "markdown";
                    }
                    else if (step["media"].contains("svg"))
                    {
                        ws.media_path = get_string(step["media"], "svg");
                        ws.media_type = "svg";
                    }
                }

                ew.steps.push_back(std::move(ws));
            }
        }
        result.push_back(std::move(ew));
    }
    return result;
}

auto parse_custom_editors(const nlohmann::json& contributes) -> std::vector<ExtensionCustomEditor>
{
    std::vector<ExtensionCustomEditor> result;
    if (!contributes.contains("customEditors") || !contributes["customEditors"].is_array())
    {
        return result;
    }
    for (const auto& ce : contributes["customEditors"])
    {
        ExtensionCustomEditor ece;
        ece.view_type = get_string(ce, "viewType");
        ece.display_name = get_string(ce, "displayName");
        ece.priority = get_string(ce, "priority");

        if (ce.contains("selector") && ce["selector"].is_array())
        {
            for (const auto& sel : ce["selector"])
            {
                ExtensionCustomEditor::Selector selector;
                selector.file_name_pattern = get_string(sel, "filenamePattern");
                ece.selectors.push_back(std::move(selector));
            }
        }
        result.push_back(std::move(ece));
    }
    return result;
}

auto parse_task_definitions(const nlohmann::json& contributes)
    -> std::vector<ExtensionTaskDefinition>
{
    std::vector<ExtensionTaskDefinition> result;
    if (!contributes.contains("taskDefinitions") || !contributes["taskDefinitions"].is_array())
    {
        return result;
    }
    for (const auto& td : contributes["taskDefinitions"])
    {
        ExtensionTaskDefinition etd;
        etd.type = get_string(td, "type");
        etd.required = get_string_array(td, "required");

        if (td.contains("properties") && td["properties"].is_object())
        {
            for (auto it = td["properties"].begin(); it != td["properties"].end(); ++it)
            {
                std::string prop_type;
                if (it.value().is_object())
                {
                    prop_type = get_string(it.value(), "type");
                }
                etd.properties[it.key()] = prop_type;
            }
        }
        result.push_back(std::move(etd));
    }
    return result;
}

auto parse_problem_patterns(const nlohmann::json& contributes)
    -> std::vector<ExtensionProblemPattern>
{
    std::vector<ExtensionProblemPattern> result;
    if (!contributes.contains("problemPatterns") || !contributes["problemPatterns"].is_array())
    {
        return result;
    }
    for (const auto& pp : contributes["problemPatterns"])
    {
        ExtensionProblemPattern epp;
        epp.name = get_string(pp, "name");
        epp.regexp = get_string(pp, "regexp");
        epp.file = get_int(pp, "file", 1);
        epp.line = get_int(pp, "line", 2);
        epp.column = get_int(pp, "column", 0);
        epp.severity = get_int(pp, "severity", 0);
        epp.message = get_int(pp, "message", 3);
        result.push_back(std::move(epp));
    }
    return result;
}

auto parse_problem_matchers(const nlohmann::json& contributes)
    -> std::vector<ExtensionProblemMatcher>
{
    std::vector<ExtensionProblemMatcher> result;
    if (!contributes.contains("problemMatchers") || !contributes["problemMatchers"].is_array())
    {
        return result;
    }
    for (const auto& pm : contributes["problemMatchers"])
    {
        ExtensionProblemMatcher epm;
        epm.name = get_string(pm, "name");
        epm.owner = get_string(pm, "owner");
        epm.file_location = get_string(pm, "fileLocation");
        epm.source = get_string(pm, "source");

        if (pm.contains("pattern") && pm["pattern"].is_object())
        {
            ExtensionProblemPattern pattern;
            pattern.regexp = get_string(pm["pattern"], "regexp");
            pattern.file = get_int(pm["pattern"], "file", 1);
            pattern.line = get_int(pm["pattern"], "line", 2);
            pattern.column = get_int(pm["pattern"], "column", 0);
            pattern.severity = get_int(pm["pattern"], "severity", 0);
            pattern.message = get_int(pm["pattern"], "message", 3);
            epm.patterns.push_back(std::move(pattern));
        }
        else if (pm.contains("pattern") && pm["pattern"].is_array())
        {
            for (const auto& pp : pm["pattern"])
            {
                ExtensionProblemPattern pattern;
                pattern.regexp = get_string(pp, "regexp");
                pattern.file = get_int(pp, "file", 1);
                pattern.line = get_int(pp, "line", 2);
                pattern.column = get_int(pp, "column", 0);
                pattern.severity = get_int(pp, "severity", 0);
                pattern.message = get_int(pp, "message", 3);
                epm.patterns.push_back(std::move(pattern));
            }
        }
        result.push_back(std::move(epm));
    }
    return result;
}

auto parse_terminal_profiles(const nlohmann::json& contributes)
    -> std::vector<ExtensionTerminalProfile>
{
    std::vector<ExtensionTerminalProfile> result;
    // terminal.profiles is nested: contributes.terminal.profiles
    if (contributes.contains("terminal") && contributes["terminal"].is_object())
    {
        const auto& terminal = contributes["terminal"];
        if (terminal.contains("profiles") && terminal["profiles"].is_array())
        {
            for (const auto& tp : terminal["profiles"])
            {
                ExtensionTerminalProfile etp;
                etp.profile_id = get_string(tp, "id");
                etp.title = get_string(tp, "title");
                etp.icon = get_string(tp, "icon");
                result.push_back(std::move(etp));
            }
        }
    }
    return result;
}

auto parse_status_bar_items(const nlohmann::json& contributes)
    -> std::vector<ExtensionStatusBarItem>
{
    std::vector<ExtensionStatusBarItem> result;
    if (!contributes.contains("statusBarItems"))
    {
        return result;
    }

    const auto& sbi_node = contributes["statusBarItems"];
    auto parse_item = [](const nlohmann::json& item) -> ExtensionStatusBarItem
    {
        ExtensionStatusBarItem esbi;
        esbi.item_id = get_string(item, "id");
        esbi.name = get_string(item, "name");
        esbi.text = get_string(item, "text");
        esbi.tooltip = get_string(item, "tooltip");
        esbi.command = get_string(item, "command");
        esbi.alignment = get_string(item, "alignment");
        esbi.priority = get_int(item, "priority", 0);
        esbi.access_key = get_string(item, "accessibilityInformation");
        return esbi;
    };

    if (sbi_node.is_array())
    {
        for (const auto& item : sbi_node)
        {
            result.push_back(parse_item(item));
        }
    }
    else if (sbi_node.is_object())
    {
        result.push_back(parse_item(sbi_node));
    }
    return result;
}

auto parse_json_validations(const nlohmann::json& contributes)
    -> std::vector<ExtensionJsonValidation>
{
    std::vector<ExtensionJsonValidation> result;
    if (!contributes.contains("jsonValidation") || !contributes["jsonValidation"].is_array())
    {
        return result;
    }
    for (const auto& jv : contributes["jsonValidation"])
    {
        ExtensionJsonValidation ejv;
        ejv.file_match = get_string(jv, "fileMatch");
        ejv.url = get_string(jv, "url");
        result.push_back(std::move(ejv));
    }
    return result;
}

auto parse_icon_themes(const nlohmann::json& contributes) -> std::vector<ExtensionIconTheme>
{
    std::vector<ExtensionIconTheme> result;
    if (!contributes.contains("iconThemes") || !contributes["iconThemes"].is_array())
    {
        return result;
    }
    for (const auto& it : contributes["iconThemes"])
    {
        ExtensionIconTheme eit;
        eit.theme_id = get_string(it, "id");
        eit.label = get_string(it, "label");
        eit.path = get_string(it, "path");
        result.push_back(std::move(eit));
    }
    return result;
}

auto parse_product_icon_themes(const nlohmann::json& contributes)
    -> std::vector<ExtensionProductIconTheme>
{
    std::vector<ExtensionProductIconTheme> result;
    if (!contributes.contains("productIconThemes") || !contributes["productIconThemes"].is_array())
    {
        return result;
    }
    for (const auto& pit : contributes["productIconThemes"])
    {
        ExtensionProductIconTheme epit;
        epit.theme_id = get_string(pit, "id");
        epit.label = get_string(pit, "label");
        epit.path = get_string(pit, "path");
        result.push_back(std::move(epit));
    }
    return result;
}

auto parse_resource_label_formatters(const nlohmann::json& contributes)
    -> std::vector<ExtensionResourceLabelFormatter>
{
    std::vector<ExtensionResourceLabelFormatter> result;
    if (!contributes.contains("resourceLabelFormatters") ||
        !contributes["resourceLabelFormatters"].is_array())
    {
        return result;
    }
    for (const auto& rlf : contributes["resourceLabelFormatters"])
    {
        ExtensionResourceLabelFormatter erlf;
        erlf.scheme = get_string(rlf, "scheme");
        erlf.authority = get_string(rlf, "authority");
        if (rlf.contains("formatting") && rlf["formatting"].is_object())
        {
            erlf.formatting.label = get_string(rlf["formatting"], "label");
            erlf.formatting.separator = get_string(rlf["formatting"], "separator");
            erlf.formatting.strip_path_starting_separator =
                get_string(rlf["formatting"], "stripPathStartingSeparator");
        }
        result.push_back(std::move(erlf));
    }
    return result;
}

auto parse_contributions(const nlohmann::json& root) -> ExtensionContributions
{
    ExtensionContributions contrib;

    if (!root.contains("contributes") || !root["contributes"].is_object())
    {
        return contrib;
    }

    const auto& contributes = root["contributes"];

    // ── Existing contribution parsers ──
    contrib.commands = parse_commands(contributes);
    contrib.keybindings = parse_keybindings(contributes);
    contrib.languages = parse_languages(contributes);
    contrib.grammars = parse_grammars(contributes);
    contrib.themes = parse_themes(contributes);
    contrib.snippets = parse_snippets(contributes);
    contrib.configuration = parse_configuration(contributes);
    contrib.views_containers = parse_views_containers(contributes);
    contrib.views = parse_views(contributes);
    contrib.colors = parse_colors(contributes);
    contrib.menus = parse_menus(contributes);

    // ── New contribution parsers (VS Code parity) ──
    contrib.submenus = parse_submenus(contributes);
    contrib.walkthroughs = parse_walkthroughs(contributes);
    contrib.custom_editors = parse_custom_editors(contributes);
    contrib.task_definitions = parse_task_definitions(contributes);
    contrib.problem_patterns = parse_problem_patterns(contributes);
    contrib.problem_matchers = parse_problem_matchers(contributes);
    contrib.terminal_profiles = parse_terminal_profiles(contributes);
    contrib.status_bar_items = parse_status_bar_items(contributes);
    contrib.json_validations = parse_json_validations(contributes);
    contrib.icon_themes = parse_icon_themes(contributes);
    contrib.product_icon_themes = parse_product_icon_themes(contributes);
    contrib.resource_label_formatters = parse_resource_label_formatters(contributes);

    return contrib;
}

} // anonymous namespace

// ── ManifestParser ──

auto ManifestParser::parse(const std::string& json_string) -> ExtensionManifest
{
    nlohmann::json root;
    try
    {
        root = nlohmann::json::parse(json_string);
    }
    catch (const nlohmann::json::parse_error& parse_err)
    {
        throw std::runtime_error(std::string("Invalid JSON in package.json: ") + parse_err.what());
    }

    if (!root.is_object())
    {
        throw std::runtime_error("package.json root must be a JSON object");
    }

    // -- Required fields --
    const std::string manifest_name = get_string(root, "name");
    if (manifest_name.empty())
    {
        throw std::runtime_error("package.json missing required field 'name'");
    }

    const std::string manifest_version = get_string(root, "version");
    if (manifest_version.empty())
    {
        throw std::runtime_error("package.json missing required field 'version'");
    }

    const std::string manifest_publisher = get_string(root, "publisher");
    if (manifest_publisher.empty())
    {
        throw std::runtime_error("package.json missing required field 'publisher'");
    }

    ExtensionManifest manifest;
    manifest.name = manifest_name;
    manifest.version = manifest_version;
    manifest.publisher = manifest_publisher;

    // -- Optional fields --
    manifest.display_name = get_string(root, "displayName");
    manifest.description = get_string(root, "description");
    manifest.icon = get_string(root, "icon");
    manifest.license = get_string(root, "license");
    manifest.main = get_string(root, "main");

    // -- Engine constraint --
    if (root.contains("engines") && root["engines"].is_object())
    {
        manifest.engines_vscode = get_string(root["engines"], "vscode");
    }

    // -- Activation events --
    const auto raw_events = get_string_array(root, "activationEvents");
    manifest.activation_events.reserve(raw_events.size());
    for (const auto& raw_evt : raw_events)
    {
        manifest.activation_events.push_back(ActivationEvent::parse(raw_evt));
    }

    // -- Categories and keywords --
    manifest.categories = get_string_array(root, "categories");
    manifest.keywords = get_string_array(root, "keywords");

    // -- Dependencies --
    manifest.extension_dependencies = get_string_array(root, "extensionDependencies");
    manifest.extension_pack = get_string_array(root, "extensionPack");

    // -- Contribution points --
    manifest.contributes = parse_contributions(root);

    // -- Repository --
    if (root.contains("repository") && root["repository"].is_object())
    {
        RepositoryInfo repo;
        repo.type = get_string(root["repository"], "type");
        repo.url = get_string(root["repository"], "url");
        manifest.repository = repo;
    }
    else if (root.contains("repository") && root["repository"].is_string())
    {
        RepositoryInfo repo;
        repo.url = root["repository"].get<std::string>();
        manifest.repository = repo;
    }

    manifest.bugs_url = get_string(root, "bugs");

    return manifest;
}

auto ManifestParser::parse_file(const std::string& path) -> ExtensionManifest
{
    std::ifstream file_stream(path);
    if (!file_stream.is_open())
    {
        throw std::runtime_error("Cannot open package.json at: " + path);
    }

    std::ostringstream content;
    content << file_stream.rdbuf();
    return parse(content.str());
}

// ── ExtensionManifest ──

auto ExtensionManifest::identifier() const -> ExtensionIdentifier
{
    return {publisher, name};
}

} // namespace markamp::core
