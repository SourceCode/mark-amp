import os
import re
import math

# Source directory containing .ts theme files
SOURCE_DIR = "/Users/ryanrentfro/code/markamp/docs/themes"
# Output directory for .md theme files
OUTPUT_DIR = "/Users/ryanrentfro/code/markamp/themes"

# Map TS color names to Theme color tokens
COLOR_MAP = {
    "chromeBg": "--bg-app",
    "secondary": "--bg-panel",
    "surface": "--bg-panel", # fallback
    "surfaceHighlight": "--bg-header",
    "windowBg": "--bg-input", # approximate
    "text": "--text-main",
    "textMuted": "--text-muted",
    "primary": "--accent-primary",
    "success": "--accent-secondary", # using success as secondary accent often looks okay
    "border": "--border-light",
    "windowBorder": "--border-dark",
    
    # Editor specific mappings from 'createDefaultEditorTheme' args usually found in the file
    "background": "--editor-bg",
    "text": "--editor-fg", # in editor block
    "selection": "--editor-selection",
    "line": "--editor-line-number",
    "caret": "--editor-cursor",
    "gutter": "--editor-gutter",
}

def parse_ts_theme(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # Extract ID
    id_match = re.search(r"id:\s*'([^']+)'", content)
    if not id_match:
        return None
    theme_id = id_match.group(1)

    # Extract Name
    name_match = re.search(r"name:\s*'([^']+)'", content)
    theme_name = name_match.group(1) if name_match else theme_id

    # Extract Type
    type_match = re.search(r"type:\s*'([^']+)'", content)
    theme_type = type_match.group(1) if type_match else "dark"

    colors = {}

    # Extract Colors block
    colors_block_match = re.search(r"colors:\s*\{([^}]+)\}", content, re.DOTALL)
    if colors_block_match:
        colors_block = colors_block_match.group(1)
        for line in colors_block.split('\n'):
            line = line.strip()
            if not line or line.startswith('//'): continue
            
            # Match key: 'value'
            kv_match = re.match(r"(\w+):\s*'([^']+)'", line)
            if kv_match:
                key, value = kv_match.groups()
                if key in COLOR_MAP:
                    colors[COLOR_MAP[key]] = value

    # Extract Editor colors (often inside createDefaultEditorTheme ref or explicit editor object)
    editor_block_match = re.search(r"editor:\s*(?:createDefaultEditorTheme\(\s*)?\{([^}]+)\}", content, re.DOTALL)
    if editor_block_match:
        editor_block = editor_block_match.group(1)
        for line in editor_block.split('\n'):
            line = line.strip()
            if not line or line.startswith('//'): continue
            
            kv_match = re.match(r"(\w+):\s*'([^']+)'", line)
            if kv_match:
                key, value = kv_match.groups()
                # Use COLOR_MAP but check for editor specific duplicates/overrides
                # We reuse the map but context matters.
                if key == "background": colors["--editor-bg"] = value
                elif key == "text": colors["--editor-fg"] = value
                elif key == "selection": colors["--editor-selection"] = value
                elif key == "line": colors["--editor-line-number"] = value
                elif key == "caret": colors["--editor-cursor"] = value
                elif key == "gutter": colors["--editor-gutter"] = value

    # UI Fallbacks if missing
    if "--list-hover" not in colors and "--accent-primary" in colors:
        colors["--list-hover"] = colors["--accent-primary"] # + alpha handled by engine? No, explicitly.
        # We assume the engine handles alpha if we don't specify, or we can't easily calc here without lib.
        # We will leave it to engine defaults if missing.
    
    return {
        "id": theme_id,
        "name": theme_name,
        "type": theme_type,
        "colors": colors
    }

def generate_md(theme_data, output_path):
    with open(output_path, 'w') as f:
        f.write("---\n")
        f.write(f"id: {theme_data['id']}\n")
        f.write(f"name: {theme_data['name']}\n")
        f.write(f"type: {theme_data['type']}\n")
        f.write("colors:\n")
        for key, value in theme_data['colors'].items():
            f.write(f"  {key}: \"{value}\"\n")
        f.write("---\n")
        f.write("\n")
        f.write(f"<!-- Imported from {theme_data['id']} -->\n")

def main():
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    files = [f for f in os.listdir(SOURCE_DIR) if f.endswith('.ts') and f != "index.ts" and f != "utils.ts" and "test" not in f]
    
    print(f"Found {len(files)} theme files.")
    
    count = 0
    for file in files:
        filepath = os.path.join(SOURCE_DIR, file)
        theme_data = parse_ts_theme(filepath)
        if theme_data:
            output_filename = f"{theme_data['id']}.md"
            output_path = os.path.join(OUTPUT_DIR, output_filename)
            generate_md(theme_data, output_path)
            print(f"Generated {output_filename}")
            count += 1
        else:
            print(f"Skipping {file} (parse error)")

    print(f"Migration complete. {count} themes generated.")

if __name__ == "__main__":
    main()
