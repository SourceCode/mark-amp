#include "ui/SvgDocument.h"

#include <cctype>
#include <charconv>
#include <unordered_map>

namespace markamp::ui
{

auto ParseSvgPath(const std::string& d) -> std::vector<SvgPathCommand>
{
    std::vector<SvgPathCommand> commands;

    size_t i = 0;
    while (i < d.size())
    {
        while (i < d.size() && (std::isspace(d[i]) || d[i] == ','))
        {
            i++;
        }

        if (i >= d.size())
        {
            break;
        }

        char c = d[i];
        if (std::isalpha(c))
        {
            SvgPathCommand cmd;
            cmd.type = c;
            i++;

            while (i < d.size())
            {
                while (i < d.size() && (std::isspace(d[i]) || d[i] == ','))
                {
                    i++;
                }

                if (i >= d.size() || std::isalpha(d[i]))
                {
                    break;
                }

                char* endptr = nullptr;
                float val = std::strtof(d.c_str() + i, &endptr);

                if (endptr != d.c_str() + i)
                {
                    cmd.args.push_back(val);
                    i = static_cast<size_t>(endptr - d.c_str());
                }
                else
                {
                    break;
                }
            }

            commands.push_back(std::move(cmd));
        }
        else
        {
            break;
        }
    }

    return commands;
}

static auto try_parse_float(const std::string& s) -> float
{
    try
    {
        return std::stof(s);
    }
    catch (...)
    {
        return 0.0f;
    }
}

auto SvgDocument::parse(const std::string& xml) -> bool
{
    clear();

    size_t pos = 0;
    while ((pos = xml.find('<', pos)) != std::string::npos)
    {
        // Skip closing tags, comments, <?xml ... ?>
        if (pos + 1 < xml.size() &&
            (xml[pos + 1] == '/' || xml[pos + 1] == '!' || xml[pos + 1] == '?'))
        {
            pos = xml.find('>', pos);
            if (pos == std::string::npos)
            {
                break;
            }
            continue;
        }

        size_t end_tag = xml.find('>', pos);
        if (end_tag == std::string::npos)
        {
            break;
        }

        std::string tag_content = xml.substr(pos + 1, end_tag - pos - 1);
        pos = end_tag + 1;

        // Extract tag name
        size_t name_end = 0;
        while (name_end < tag_content.size() && !std::isspace(tag_content[name_end]) &&
               tag_content[name_end] != '/')
        {
            name_end++;
        }

        std::string tag_name = tag_content.substr(0, name_end);

        // Parse attributes into a map
        std::unordered_map<std::string, std::string> attrs;
        size_t attr_pos = name_end;
        while (attr_pos < tag_content.size())
        {
            while (attr_pos < tag_content.size() && std::isspace(tag_content[attr_pos]))
            {
                attr_pos++;
            }
            if (attr_pos >= tag_content.size() || tag_content[attr_pos] == '/')
            {
                break;
            }

            size_t eq_pos = tag_content.find('=', attr_pos);
            if (eq_pos == std::string::npos)
            {
                break;
            }

            std::string key = tag_content.substr(attr_pos, eq_pos - attr_pos);
            while (!key.empty() && std::isspace(key.back()))
            {
                key.pop_back();
            }

            size_t val_start = eq_pos + 1;
            while (val_start < tag_content.size() &&
                   (std::isspace(tag_content[val_start]) || tag_content[val_start] == '\"' ||
                    tag_content[val_start] == '\''))
            {
                val_start++;
            }

            size_t val_end = val_start;
            while (val_end < tag_content.size() && tag_content[val_end] != '\"' &&
                   tag_content[val_end] != '\'')
            {
                val_end++;
            }

            std::string val = tag_content.substr(val_start, val_end - val_start);
            attrs[key] = val;

            attr_pos = val_end + 1;
        }

        if (tag_name == "svg" || tag_name == "SVG")
        {
            if (attrs.count("viewBox"))
            {
                std::vector<float> vb_vals;
                std::string vb_str = attrs["viewBox"];
                size_t vpos = 0;
                while (vpos < vb_str.size())
                {
                    while (vpos < vb_str.size() &&
                           (std::isspace(vb_str[vpos]) || vb_str[vpos] == ','))
                    {
                        vpos++;
                    }
                    if (vpos >= vb_str.size())
                    {
                        break;
                    }
                    char* endptr = nullptr;
                    float v = std::strtof(vb_str.c_str() + vpos, &endptr);
                    if (endptr != vb_str.c_str() + vpos)
                    {
                        vb_vals.push_back(v);
                        vpos = static_cast<size_t>(endptr - vb_str.c_str());
                    }
                    else
                    {
                        break;
                    }
                }
                if (vb_vals.size() == 4)
                {
                    viewBox_ = {vb_vals[0], vb_vals[1], vb_vals[2], vb_vals[3]};
                }
            }
        }
        else if (tag_name == "path")
        {
            SvgPath p;
            if (attrs.count("d"))
            {
                p.d = attrs["d"];
                p.commands = ParseSvgPath(p.d);
            }
            if (attrs.count("fill"))
                p.fill = attrs["fill"];
            if (attrs.count("stroke"))
                p.stroke = attrs["stroke"];
            if (attrs.count("stroke-width"))
                p.stroke_width = try_parse_float(attrs["stroke-width"]);
            add_shape(std::move(p));
        }
        else if (tag_name == "circle")
        {
            SvgCircle c;
            if (attrs.count("cx"))
                c.cx = try_parse_float(attrs["cx"]);
            if (attrs.count("cy"))
                c.cy = try_parse_float(attrs["cy"]);
            if (attrs.count("r"))
                c.r = try_parse_float(attrs["r"]);
            if (attrs.count("fill"))
                c.fill = attrs["fill"];
            if (attrs.count("stroke"))
                c.stroke = attrs["stroke"];
            if (attrs.count("stroke-width"))
                c.stroke_width = try_parse_float(attrs["stroke-width"]);
            add_shape(std::move(c));
        }
        else if (tag_name == "rect")
        {
            SvgRect r;
            if (attrs.count("x"))
                r.x = try_parse_float(attrs["x"]);
            if (attrs.count("y"))
                r.y = try_parse_float(attrs["y"]);
            if (attrs.count("width"))
                r.width = try_parse_float(attrs["width"]);
            if (attrs.count("height"))
                r.height = try_parse_float(attrs["height"]);
            if (attrs.count("rx"))
                r.rx = try_parse_float(attrs["rx"]);
            if (attrs.count("ry"))
                r.ry = try_parse_float(attrs["ry"]);
            if (attrs.count("fill"))
                r.fill = attrs["fill"];
            if (attrs.count("stroke"))
                r.stroke = attrs["stroke"];
            if (attrs.count("stroke-width"))
                r.stroke_width = try_parse_float(attrs["stroke-width"]);
            add_shape(std::move(r));
        }
        else if (tag_name == "line")
        {
            SvgLine l;
            if (attrs.count("x1"))
                l.x1 = try_parse_float(attrs["x1"]);
            if (attrs.count("y1"))
                l.y1 = try_parse_float(attrs["y1"]);
            if (attrs.count("x2"))
                l.x2 = try_parse_float(attrs["x2"]);
            if (attrs.count("y2"))
                l.y2 = try_parse_float(attrs["y2"]);
            if (attrs.count("stroke"))
                l.stroke = attrs["stroke"];
            if (attrs.count("stroke-width"))
                l.stroke_width = try_parse_float(attrs["stroke-width"]);
            add_shape(std::move(l));
        }
        else if (tag_name == "polyline")
        {
            SvgPolyline p;
            if (attrs.count("fill"))
                p.fill = attrs["fill"];
            if (attrs.count("stroke"))
                p.stroke = attrs["stroke"];
            if (attrs.count("stroke-width"))
                p.stroke_width = try_parse_float(attrs["stroke-width"]);
            if (attrs.count("points"))
            {
                std::string pts = attrs["points"];
                size_t ppos = 0;
                while (ppos < pts.size())
                {
                    while (ppos < pts.size() && (std::isspace(pts[ppos]) || pts[ppos] == ','))
                    {
                        ppos++;
                    }
                    if (ppos >= pts.size())
                        break;
                    char* endptr = nullptr;
                    float v = std::strtof(pts.c_str() + ppos, &endptr);
                    if (endptr != pts.c_str() + ppos)
                    {
                        p.points.push_back(v);
                        ppos = static_cast<size_t>(endptr - pts.c_str());
                    }
                    else
                        break;
                }
            }
            add_shape(std::move(p));
        }
    }

    return viewBox_.is_valid() || !shapes_.empty();
}

} // namespace markamp::ui
