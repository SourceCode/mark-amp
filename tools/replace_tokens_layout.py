import re

file_path = '/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp'

with open(file_path, 'r') as f:
    content = f.read()

# Mappings for LayoutManager panel creation
def replace_token(match):
    token = match.group(1)
    scoped = ""
    if token == 'BgPanel':
        scoped = '"sidebar.bg"'
    elif token == 'BgApp':
        scoped = '"bg.app"'
    elif token == 'BgHeader':
        scoped = '"sidebar.border"' # or '"bg.header"'
    elif token == 'TextMain':
        scoped = '"text.main"'
    elif token == 'TextMuted':
        scoped = '"text.muted"'
    elif token == 'AccentPrimary':
        scoped = '"accent.primary"'
    elif token == 'BorderLight':
        scoped = '"border.light"'
    else:
        return match.group(0) # don't change if no obvious mapping
        
    full_str = f'theme_engine().resolve_token({scoped}).value_or(theme_engine().color(core::ThemeColorToken::{token}))'
    return full_str

# Regex to find theme_engine().color(core::ThemeColorToken::XXX)
new_content = re.sub(r'theme_engine\(\)\.color\(core::ThemeColorToken::(\w+)\)', replace_token, content)

with open(file_path, 'w') as f:
    f.write(new_content)

print("Updated LayoutManager.cpp")
