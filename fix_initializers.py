import re

with open('src/ui/StatusBarPanel.cpp', 'r') as f:
    content = f.read()

# Add pragma to RebuildItems
target_start = 'void StatusBarPanel::RebuildItems()\n{'
pragma_start = '#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wmissing-field-initializers"\n'
content = content.replace(target_start, pragma_start + target_start)

# Add pragma pop at the end of the method
# We need to find the end of RebuildItems()
# It's at the end of the file or before the next method.
# Since it's the last method, we can just append if it's the last brace, but let's be safe.
# Find the next closing brace at column 0 after RebuildItems
start_idx = content.find(target_start)
end_idx = content.find('\n}\n', start_idx)
if end_idx != -1:
    content = content[:end_idx] + '\n}\n#pragma GCC diagnostic pop\n' + content[end_idx+3:]

with open('src/ui/StatusBarPanel.cpp', 'w') as f:
    f.write(content)
