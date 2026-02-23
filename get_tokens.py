import re

tokens = []
with open("/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h", "r") as f:
    text = f.read()
    match = re.search(r'enum class ThemeColorToken\s*\{([^}]+)\}', text)
    if match:
        body = match.group(1)
        for line in body.split("\n"):
            line = line.split("//")[0].strip()
            if line and not line.startswith("enum"):
                tok = line.strip(", ")
                if tok:
                    tokens.append(tok)

for i, t in enumerate(tokens):
    print(f"Token {i}: {t}")
