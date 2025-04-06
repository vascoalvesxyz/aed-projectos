#!/bin/bash

# Usage: ./highlight.sh mycode.c html
# Output: mycode.html or mycode.rtf

INPUT_FILE="$1"
FORMAT="${2:-html}" # default to html if not specified
OUTPUT_FILE="${INPUT_FILE%.*}.$FORMAT"

if [[ -z "$INPUT_FILE" ]]; then
    echo "Usage: $0 <source-file> [html|rtf]"
    exit 1
fi

if ! command -v pygmentize &> /dev/null; then
    echo "pygmentize not found. Install it with: pip install pygments"
    exit 1
fi

# Choose lexer based on file extension (optional enhancement)
LEXER=$(pygmentize -N "$INPUT_FILE")

pygmentize -f "$FORMAT" -l "$LEXER" -O style=monokai -o "$OUTPUT_FILE" "$INPUT_FILE"

echo "Output written to: $OUTPUT_FILE"

